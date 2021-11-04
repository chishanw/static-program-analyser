#include "QueryEvaluator.h"

#include <../../autotester/src/AbstractWrapper.h>

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/Global.h"

using namespace std;
using namespace query;

QueryEvaluator::QueryEvaluator(PKB* pkb, QueryOptimizer* optimizer)
    : followsEvaluator(pkb),
      parentEvaluator(pkb),
      usesEvaluator(pkb),
      modifiesEvaluator(pkb),
      callsEvaluator(pkb),
      nextEvaluator(pkb),
      affectsEvaluator(pkb),
      patternEvaluator(pkb),
      withEvaluator(pkb) {
  this->pkb = pkb;
  this->optimizer = optimizer;
  areAllClausesTrue = true;
  finalQueryResults = {};
  groupQueryResults = {};
  queryResultsSynonyms = {};

  QueryEvaluatorCache* onDemandRsCache = new QueryEvaluatorCache();
  nextEvaluator.attachCache(onDemandRsCache);
}

FinalQueryResults QueryEvaluator::evaluateQuery(
    unordered_map<string, DesignEntity> synonymMap, SelectClause select) {
  this->synonymMap = synonymMap;
  finalQueryResults.clear();

  while (true) {
    optional<GroupDetails> optGroupDetails = optimizer->GetNextGroupDetails();
    if (!optGroupDetails.has_value()) {
      break;
    }

    while (true) {
      SynonymCountsTable synonymCounts = getSynonymCounts();
      optional<ConditionClause> optClause =
          optimizer->GetNextClause(synonymCounts);
      if (!optClause.has_value()) {
        break;
      }

      ConditionClause clause = optClause.value();
      if (clause.conditionClauseType == ConditionClauseType::SUCH_THAT) {
        evaluateSuchThatClause(clause.suchThatClause);
      } else if (clause.conditionClauseType == ConditionClauseType::PATTERN) {
        evaluatePatternClause(clause.patternClause);
      } else {
        evaluateWithClause(clause.withClause);
      }

      if (!areAllClausesTrue) {
        clauseSynonymValuesTable.clear();
        // early termination as soon as any clause is false
        // if select bool, false
        if (select.selectType == SelectType::BOOLEAN) {
          return {{FALSE_SELECT_BOOL_RESULT}};
        } else {
          return {};
        }
      }

      if (AbstractWrapper::GlobalStop) {
        // check if TLE after each clause evaluation
        // return whatever results we can
        return getSelectSynonymFinalResults(select);
      }
      clauseSynonymValuesTable.clear();
    }

    GroupDetails groupDetails = optGroupDetails.value();
    if (groupDetails.isBooleanGroup) {
      groupQueryResults.clear();
      continue;
    }
    // after each grp, keep only IntermediateQueryResult with the select syns
    filterGroupResultsBySelectSynonyms(groupDetails.selectedSynonyms);
    mergeGroupResultsIntoFinalResults();
    // clean up group data
    filterQuerySynonymsBySelectSynonyms(groupDetails.selectedSynonyms);
    groupQueryResults.clear();
  }

  return getSelectSynonymFinalResults(select);
}

/* Evaluate Such That Clauses -------------------------------------------- */
void QueryEvaluator::evaluateSuchThatClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;
  auto relationshipType = clause.relationshipType;

  pair<ParamType, ParamType> paramTypesCombo = make_pair(left.type, right.type);
  bool isBoolClause = getIsBoolClause(relationshipType, paramTypesCombo);
  vector<pair<ParamType, ParamType>> pairParamTypesCombos = {
      {ParamType::SYNONYM, ParamType::SYNONYM},
      {ParamType::SYNONYM, ParamType::WILDCARD},
      {ParamType::WILDCARD, ParamType::SYNONYM}};
  bool isPairClause =
      find(pairParamTypesCombos.begin(), pairParamTypesCombos.end(),
           paramTypesCombo) != pairParamTypesCombos.end();

  // evaluate clause that returns a boolean
  if (isBoolClause) {
    areAllClausesTrue = callSubEvaluatorBool(relationshipType, left, right);
    return;
  }

  ClauseIncomingResults incomingResults;
  // the rest of clauses has at least one synonym
  // convert clauses with synonyms to bool clauses if possible
  // by taking INTEGER/NAME_LITERAL results from previous clauses
  auto newParams = getParamsWithPrevResults(left, right);
  if (!newParams.empty()) {
    // if there is any results that can be reused from previous clauses
    for (auto newParam : newParams) {
      Param newLeft = get<0>(newParam);
      Param newRight = get<1>(newParam);
      // if not fully converted to literals & wildcards
      if (newLeft.type == ParamType::SYNONYM ||
          newRight.type == ParamType::SYNONYM) {
        ClauseIncomingResults newResults =
            callSubEvaluatorRef(relationshipType, newLeft, newRight);
        incomingResults.insert(incomingResults.end(), newResults.begin(),
                               newResults.end());
        continue;
      }

      // fully converted to literals & wildcards
      bool isClauseTrue =
          callSubEvaluatorBool(relationshipType, newLeft, newRight);
      if (isClauseTrue) {
        ParamPosition paramPosition = get<2>(newParam);
        switch (paramPosition) {
          case ParamPosition::BOTH:
            incomingResults.push_back(
                {getIndexFromLiteral(left.value, newLeft.value),
                 getIndexFromLiteral(right.value, newRight.value)});
            break;
          case ParamPosition::LEFT:
            incomingResults.push_back(
                {getIndexFromLiteral(left.value, newLeft.value)});
            break;
          case ParamPosition::RIGHT:
            incomingResults.push_back(
                {getIndexFromLiteral(right.value, newRight.value)});
            break;
          default:
            break;
        }
      }
    }
    if (incomingResults.empty()) {
      areAllClausesTrue = false;
      return;
    }
  } else {
    if (isPairClause) {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
      // evaluate clause that returns a vector of ref pairs
      incomingResults = callSubEvaluatorPair(relationshipType, left, right);
    } else {  // one synonym
      // evaluate clause that returns a vector of single refs
      incomingResults = callSubEvaluatorRef(relationshipType, left, right);
    }
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

bool QueryEvaluator::callSubEvaluatorBool(RelationshipType relationshipType,
                                          const Param& left,
                                          const Param& right) {
  switch (relationshipType) {
    case RelationshipType::FOLLOWS:
      return followsEvaluator.evaluateBoolFollows(left, right);
    case RelationshipType::FOLLOWS_T:
      return followsEvaluator.evaluateBoolFollowsT(left, right);
    case RelationshipType::PARENT:
      return parentEvaluator.evaluateBoolParent(left, right);
    case RelationshipType::PARENT_T:
      return parentEvaluator.evaluateBoolParentT(left, right);
    case RelationshipType::MODIFIES_S:
      return modifiesEvaluator.evaluateBoolModifiesS(left, right);
    case RelationshipType::MODIFIES_P:
      return modifiesEvaluator.evaluateBoolModifiesP(left, right);
    case RelationshipType::USES_S:
      return usesEvaluator.evaluateBoolUsesS(left, right);
    case RelationshipType::USES_P:
      return usesEvaluator.evaluateBoolUsesP(left, right);
    case RelationshipType::CALLS:
      return callsEvaluator.evaluateBoolCalls(left, right);
    case RelationshipType::CALLS_T:
      return callsEvaluator.evaluateBoolCallsT(left, right);
    case RelationshipType::NEXT:
    case RelationshipType::NEXT_BIP:
      return nextEvaluator.evaluateBoolNextNextBip(relationshipType, left,
                                                   right);
    case RelationshipType::NEXT_T:
    case RelationshipType::NEXT_BIP_T:
      return nextEvaluator.evaluateBoolNextTNextBipT(relationshipType, left,
                                                     right);
    case RelationshipType::AFFECTS:
      return affectsEvaluator.evaluateBoolAffects(left, right);
    default:
      return false;
  }
}

ClauseIncomingResults QueryEvaluator::callSubEvaluatorRef(
    RelationshipType relationshipType, const Param& left, const Param& right) {
  unordered_set<STMT_NO> refResults = {};
  switch (relationshipType) {
    case RelationshipType::FOLLOWS:
      refResults = followsEvaluator.evaluateStmtFollows(left, right);
      break;
    case RelationshipType::FOLLOWS_T:
      refResults = followsEvaluator.evaluateStmtFollowsT(left, right);
      break;
    case RelationshipType::PARENT:
      refResults = parentEvaluator.evaluateStmtParent(left, right);
      break;
    case RelationshipType::PARENT_T:
      refResults = parentEvaluator.evaluateStmtParentT(left, right);
      break;
    case RelationshipType::MODIFIES_S:
      refResults = modifiesEvaluator.evaluateModifiesS(left, right);
      break;
    case RelationshipType::MODIFIES_P:
      refResults = modifiesEvaluator.evaluateModifiesP(left, right);
      break;
    case RelationshipType::USES_S:
      refResults = usesEvaluator.evaluateUsesS(left, right);
      break;
    case RelationshipType::USES_P:
      refResults = usesEvaluator.evaluateUsesP(left, right);
      break;
    case RelationshipType::CALLS:
      refResults = callsEvaluator.evaluateProcCalls(left, right);
      break;
    case RelationshipType::CALLS_T:
      refResults = callsEvaluator.evaluateProcCallsT(left, right);
      break;
    case RelationshipType::NEXT:
    case RelationshipType::NEXT_BIP:
      refResults =
          nextEvaluator.evaluateNextNextBip(relationshipType, left, right);
      break;
    case RelationshipType::NEXT_T:
    case RelationshipType::NEXT_BIP_T:
      refResults =
          nextEvaluator.evaluateNextTNextBipT(relationshipType, left, right);
      break;
    case RelationshipType::AFFECTS:
      refResults = affectsEvaluator.evaluateStmtAffects(left, right);
      break;
    default:
      return {};
  }
  return formatRefResults(refResults);
}

ClauseIncomingResults QueryEvaluator::callSubEvaluatorPair(
    RelationshipType relationshipType, const Param& left, const Param& right) {
  switch (relationshipType) {
    case RelationshipType::FOLLOWS:
      return followsEvaluator.evaluateStmtPairFollows(left, right);
    case RelationshipType::FOLLOWS_T:
      return followsEvaluator.evaluateStmtPairFollowsT(left, right);
    case RelationshipType::PARENT:
      return parentEvaluator.evaluateStmtPairParent(left, right);
    case RelationshipType::PARENT_T:
      return parentEvaluator.evaluateStmtPairParentT(left, right);
    case RelationshipType::MODIFIES_S:
      return modifiesEvaluator.evaluatePairModifiesS(left, right);
    case RelationshipType::MODIFIES_P:
      return modifiesEvaluator.evaluatePairModifiesP(left, right);
    case RelationshipType::USES_S:
      return usesEvaluator.evaluatePairUsesS(left, right);
    case RelationshipType::USES_P:
      return usesEvaluator.evaluatePairUsesP(left, right);
    case RelationshipType::CALLS:
      return callsEvaluator.evaluateProcPairCalls(left, right);
    case RelationshipType::CALLS_T:
      return callsEvaluator.evaluateProcPairCallsT(left, right);
    case RelationshipType::NEXT:
    case RelationshipType::NEXT_BIP:
      return nextEvaluator.evaluatePairNextNextBip(relationshipType, left,
                                                   right);
    case RelationshipType::NEXT_T:
    case RelationshipType::NEXT_BIP_T:
      return nextEvaluator.evaluatePairNextTNextBipT(relationshipType, left,
                                                     right);
    case RelationshipType::AFFECTS:
      return affectsEvaluator.evaluatePairAffects();
    default:
      return {};
  }
}

/* Evaluate Pattern Clauses ---------------------------------------------- */
void QueryEvaluator::evaluatePatternClause(PatternClause clause) {
  Synonym matchSynonym = clause.matchSynonym;
  Param varParam = clause.leftParam;
  PatternExpr patternExpr = clause.patternExpr;
  DesignEntity designEntity = clause.matchSynonym.entity;

  ClauseIncomingResults incomingResults;
  bool isRefClause = varParam.type == ParamType::NAME_LITERAL ||
                     varParam.type == ParamType::WILDCARD;

  if (isRefClause) {
    incomingResults =
        callPatternSubEvaluatorRef(designEntity, varParam, patternExpr);
  } else {
    incomingResults =
        callPatternSubEvaluatorPair(designEntity, varParam, patternExpr);
  }

  Param patternSynonymParam = {ParamType::SYNONYM, matchSynonym.name};
  filterAndAddIncomingResults(incomingResults, patternSynonymParam, varParam);
}

ClauseIncomingResults QueryEvaluator::callPatternSubEvaluatorRef(
    DesignEntity designEntity, const Param& varParam,
    const PatternExpr& patternExpr) {
  unordered_set<int> refResults = {};
  switch (designEntity) {
    case DesignEntity::ASSIGN:
      refResults =
          patternEvaluator.evaluateAssignPattern(varParam, patternExpr);
      break;
    case DesignEntity::IF:
      refResults = patternEvaluator.evaluateIfPattern(varParam);
      break;
    case DesignEntity::WHILE:
      refResults = patternEvaluator.evaluateWhilePattern(varParam);
      break;
    default:
      return {};
  }
  return formatRefResults(refResults);
}

ClauseIncomingResults QueryEvaluator::callPatternSubEvaluatorPair(
    DesignEntity designEntity, const Param& varParam,
    const PatternExpr& patternExpr) {
  switch (designEntity) {
    case DesignEntity::ASSIGN:
      return patternEvaluator.evaluateAssignPairPattern(varParam, patternExpr);
    case DesignEntity::IF:
      return patternEvaluator.evaluateIfPairPattern(varParam);
    case DesignEntity::WHILE:
      return patternEvaluator.evaluateWhilePairPattern(varParam);
    default:
      return {};
  }
}

/* Evaluate With Clauses -------------------------------------------------- */
void QueryEvaluator::evaluateWithClause(WithClause clause) {
  Param left = clause.leftParam;
  Param right = clause.rightParam;

  bool isLeftParamSynonym = false;
  bool isRightParamSynonym = false;
  vector<vector<int>> leftSynoynmValues = {};
  vector<vector<int>> rightSynoynmValues = {};
  vector<vector<int>> leftAndRightSynonymValues = {};

  unordered_set<ParamType> synonymTypes = {
      ParamType::ATTRIBUTE_PROC_NAME, ParamType::ATTRIBUTE_VAR_NAME,
      ParamType::ATTRIBUTE_VALUE, ParamType::ATTRIBUTE_STMT_NUM,
      ParamType::SYNONYM};

  if (synonymTypes.find(left.type) != synonymTypes.end()) {
    isLeftParamSynonym = true;
    unordered_set<STMT_NO> allValues = getAllValuesOfSynonym(left.value);
    for (auto value : allValues) {
      leftSynoynmValues.push_back({value});
    }
  }

  if (synonymTypes.find(right.type) != synonymTypes.end()) {
    isRightParamSynonym = true;
    unordered_set<STMT_NO> allValues = getAllValuesOfSynonym(right.value);
    for (auto value : allValues) {
      rightSynoynmValues.push_back({value});
    }
  }
  if (isLeftParamSynonym && isRightParamSynonym) {
    for (auto leftStmt : leftSynoynmValues) {
      for (auto rightStmt : rightSynoynmValues) {
        leftAndRightSynonymValues.push_back({leftStmt[0], rightStmt[0]});
      }
    }
  }

  if (groupQueryResults.empty()) {
    // initialize results with all possible left and/or right synonym values
    if (isLeftParamSynonym && isRightParamSynonym) {
      initializeQueryResults(leftAndRightSynonymValues,
                             {ParamType::SYNONYM, left.value},
                             {ParamType::SYNONYM, right.value});
    } else if (isLeftParamSynonym) {
      initializeQueryResults(leftSynoynmValues,
                             {ParamType::SYNONYM, left.value}, right);
    } else if (isRightParamSynonym) {
      initializeQueryResults(rightSynoynmValues, left,
                             {ParamType::SYNONYM, right.value});
    }
  } else {
    bool isLeftSynInQueryResults =
        queryResultsSynonyms.find(left.value) != queryResultsSynonyms.end();
    bool isRightSynInQueryResults =
        queryResultsSynonyms.find(right.value) != queryResultsSynonyms.end();

    // add all possible left and/or right synonym values if not yet in results
    if (isLeftParamSynonym && !isLeftSynInQueryResults && isRightParamSynonym &&
        !isRightSynInQueryResults) {
      queryResultsSynonyms.insert(left.value);
      queryResultsSynonyms.insert(right.value);
      crossProduct(leftAndRightSynonymValues, {left.value, right.value});
    } else if (isLeftParamSynonym && !isLeftSynInQueryResults) {
      queryResultsSynonyms.insert(left.value);
      crossProduct(leftSynoynmValues, {left.value});
    } else if (isRightParamSynonym && !isRightSynInQueryResults) {
      queryResultsSynonyms.insert(right.value);
      crossProduct(rightSynoynmValues, {right.value});
    }
  }

  auto evaluatedResults = withEvaluator.evaluateAttributes(
      left, right, synonymMap, groupQueryResults);
  bool isClauseTrue = get<0>(evaluatedResults);
  vector<IntermediateQueryResult> newQueryResults = get<1>(evaluatedResults);
  SynonymValuesTable newSynonymValuesTable = get<2>(evaluatedResults);
  if (newQueryResults != groupQueryResults) {
    clauseSynonymValuesTable = newSynonymValuesTable;
  }

  if (!isClauseTrue) {
    areAllClausesTrue = false;
    return;
  }

  groupQueryResults = newQueryResults;
}

/* Filter And Merge Results For Each Clause ------------------------------- */
void QueryEvaluator::filterAndAddIncomingResults(
    ClauseIncomingResults incomingResults, const Param& left,
    const Param& right) {
  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }
  ClauseIncomingResults filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);

  if (filteredIncomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  clauseSynonymValuesTable.clear();
  if (groupQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::initializeQueryResults(
    ClauseIncomingResults incomingResults, const Param& left,
    const Param& right) {
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    queryResultsSynonyms.insert(left.value);
    queryResultsSynonyms.insert(right.value);

    for (vector<int> incomingResult : incomingResults) {
      groupQueryResults.push_back({{left.value, incomingResult.front()},
                                   {right.value, incomingResult.back()}});
      clauseSynonymValuesTable[left.value].insert(incomingResult.front());
      clauseSynonymValuesTable[right.value].insert(incomingResult.back());
    }
  } else if (left.type == ParamType::SYNONYM) {
    queryResultsSynonyms.insert(left.value);

    for (vector<int> incomingResult : incomingResults) {
      groupQueryResults.push_back({{left.value, incomingResult.front()}});
      clauseSynonymValuesTable[left.value].insert(incomingResult.front());
    }
  } else {
    queryResultsSynonyms.insert(right.value);

    for (vector<int> incomingResult : incomingResults) {
      groupQueryResults.push_back({{right.value, incomingResult.back()}});
      clauseSynonymValuesTable[right.value].insert(incomingResult.back());
    }
  }
}

ClauseIncomingResults QueryEvaluator::filterIncomingResults(
    ClauseIncomingResults incomingResults, const Param& left,
    const Param& right) {
  ClauseIncomingResults finalResults = {};

  if ((left.type == ParamType::SYNONYM) && (right.type == ParamType::SYNONYM)) {
    ClauseIncomingResults filteredResults = {};

    // if both params of a clause are the same synonym
    if (left.value == right.value) {
      for (vector<int> incomingResult : incomingResults) {
        if (incomingResult[0] == incomingResult[1]) {
          filteredResults.push_back(incomingResult);
        }
      }
    } else {
      filteredResults = incomingResults;
    }

    for (vector<int> incomingResult : filteredResults) {
      bool isLeftSynCorrectDesignEntity = checkIsCorrectDesignEntity(
          incomingResult.front(), synonymMap[left.value]);
      bool isRightSynCorrectDesignEntity = checkIsCorrectDesignEntity(
          incomingResult.back(), synonymMap[right.value]);
      if (isLeftSynCorrectDesignEntity && isRightSynCorrectDesignEntity) {
        finalResults.push_back(incomingResult);
      }
    }
    return finalResults;
  }

  if (left.type == ParamType::SYNONYM) {
    for (vector<int> incomingResult : incomingResults) {
      bool isLeftSynCorrectDesignEntity = checkIsCorrectDesignEntity(
          incomingResult.front(), synonymMap[left.value]);
      if (isLeftSynCorrectDesignEntity) {
        finalResults.push_back(incomingResult);
      }
    }
    return finalResults;
  }

  // right.type == ParamType::SYNONYM
  for (vector<int> incomingResult : incomingResults) {
    bool isRightSynCorrectDesignEntity = checkIsCorrectDesignEntity(
        incomingResult.back(), synonymMap[right.value]);
    if (isRightSynCorrectDesignEntity) {
      finalResults.push_back(incomingResult);
    }
  }
  return finalResults;
}

void QueryEvaluator::addIncomingResults(ClauseIncomingResults incomingResults,
                                        const Param& left, const Param& right) {
  bool isLeftParamSynonym = false;
  bool isRightParamSynonym = false;
  bool isLeftSynInQueryResults = false;
  bool isRightSynInQueryResults = false;

  if (left.type == ParamType::SYNONYM) {
    isLeftParamSynonym = true;
    isLeftSynInQueryResults =
        queryResultsSynonyms.find(left.value) != queryResultsSynonyms.end();

    // add new synonym to queryResultsSynonyms
    if (!isLeftSynInQueryResults) {
      queryResultsSynonyms.insert(left.value);
    }
  }
  if (right.type == ParamType::SYNONYM) {
    isRightParamSynonym = true;
    isRightSynInQueryResults =
        queryResultsSynonyms.find(right.value) != queryResultsSynonyms.end();

    // add new synonym to queryResultsSynonyms
    if (!isRightSynInQueryResults) {
      queryResultsSynonyms.insert(right.value);
    }
  }

  if (isLeftParamSynonym && isRightParamSynonym) {
    if (isLeftSynInQueryResults && isRightSynInQueryResults) {
      return filter(incomingResults, {left.value, right.value});
    } else if (isLeftSynInQueryResults || isRightSynInQueryResults) {
      return innerJoin(incomingResults, {left.value, right.value});
    } else {
      return crossProduct(incomingResults, {left.value, right.value});
    }
  }

  // continue the if block above
  if (isLeftParamSynonym) {
    if (isLeftSynInQueryResults) {
      return filter(incomingResults, {left.value});
    }

    return crossProduct(incomingResults, {left.value});

  } else if (isRightParamSynonym) {
    if (isRightSynInQueryResults) {
      return filter(incomingResults, {right.value});
    }
    return crossProduct(incomingResults, {right.value});

  } else {
    // both wild cards, do nothing, whether incomingResults is empty is alr
    // checked before calling this method
    if (incomingResults.empty()) {
      DMOprintErrMsgAndExit("[QE][addIncomingResults] shouldn't reach here");
    }
  }
}

/* Main Algos to Merge Clause Results -------------------------------------- */
void QueryEvaluator::filter(ClauseIncomingResults incomingResults,
                            vector<string> incomingResultsSynonyms) {
  vector<IntermediateQueryResult> newQueryResults = {};

  for (int i = 0; i < groupQueryResults.size(); i++) {
    IntermediateQueryResult queryResult = groupQueryResults[i];

    for (vector<int> incomingResult : incomingResults) {
      filterValidQueryResults(&newQueryResults, queryResult, incomingResult,
                              incomingResultsSynonyms);
    }
  }
  groupQueryResults = newQueryResults;
  if (groupQueryResults.empty()) {
    areAllClausesTrue = false;
    return;
  }
}

void QueryEvaluator::filterValidQueryResults(
    vector<IntermediateQueryResult>* newQueryResults,
    IntermediateQueryResult queryResult, vector<int> incomingResult,
    vector<string> incomingResultsSynonyms) {
  IntermediateQueryResult newQueryResult = queryResult;
  bool isValidQueryResult = true;

  for (int i = 0; i < incomingResult.size(); i++) {
    int value = incomingResult[i];
    string synonymName = incomingResultsSynonyms[i];

    if (queryResult[synonymName] != value) {
      isValidQueryResult = false;
      break;
    }
  }

  if (isValidQueryResult) {
    newQueryResults->push_back(newQueryResult);
    insertClauseSynonymValue(newQueryResult);
  }
}

void QueryEvaluator::innerJoin(ClauseIncomingResults incomingResults,
                               vector<string> incomingResultsSynonyms) {
  vector<IntermediateQueryResult> newQueryResults = {};
  for (int i = 0; i < groupQueryResults.size(); i++) {
    IntermediateQueryResult queryResult = groupQueryResults[i];

    for (vector<int> incomingResult : incomingResults) {
      innerJoinValidQueryResults(&newQueryResults, queryResult, incomingResult,
                                 incomingResultsSynonyms);
    }
  }

  groupQueryResults = newQueryResults;
  if (groupQueryResults.empty()) {
    areAllClausesTrue = false;
    return;
  }
}

void QueryEvaluator::innerJoinValidQueryResults(
    vector<IntermediateQueryResult>* newQueryResults,
    IntermediateQueryResult queryResult, vector<int> incomingResult,
    vector<string> incomingResultsSynonyms) {
  IntermediateQueryResult newQueryResult = queryResult;
  bool isValidQueryResult = true;

  for (int i = 0; i < incomingResult.size(); i++) {
    int value = incomingResult[i];
    string synonymName = incomingResultsSynonyms[i];

    if (queryResult.find(synonymName) != queryResult.end()) {
      if (queryResult[synonymName] != value) {
        isValidQueryResult = false;
        break;
      }
    } else {
      newQueryResult[synonymName] = value;
    }
  }

  if (isValidQueryResult) {
    newQueryResults->push_back(newQueryResult);
    insertClauseSynonymValue(newQueryResult);
  }
}

void QueryEvaluator::crossProduct(ClauseIncomingResults incomingResults,
                                  vector<string> incomingResultsSynonyms) {
  vector<IntermediateQueryResult> newQueryResults;

  for (IntermediateQueryResult queryResult : groupQueryResults) {
    for (vector<int> incomingResult : incomingResults) {
      IntermediateQueryResult newQueryResult = queryResult;

      for (int i = 0; i < incomingResult.size(); i++) {
        int value = incomingResult[i];
        string synonymName = incomingResultsSynonyms[i];
        newQueryResult[synonymName] = value;
      }

      newQueryResults.push_back(newQueryResult);
      insertClauseSynonymValue(newQueryResult);
    }
  }

  groupQueryResults = newQueryResults;
  if (groupQueryResults.empty()) {
    areAllClausesTrue = false;
    return;
  }
}

/* Query Optimization Related Methods --------------------------------------- */
SynonymCountsTable QueryEvaluator::getSynonymCounts() {
  SynonymCountsTable synonymCounts = {};
  for (auto synonymToValues : clauseSynonymValuesTable) {
    synonymCounts[synonymToValues.first] = synonymToValues.second.size();
  }
  return synonymCounts;
}

void QueryEvaluator::insertClauseSynonymValue(
    IntermediateQueryResult queryResult) {
  for (auto synonymValuePair : queryResult) {
    string synonym = synonymValuePair.first;
    if (clauseSynonymValuesTable.find(synonym) ==
        clauseSynonymValuesTable.end()) {
      clauseSynonymValuesTable[synonym] = {};
    }
    clauseSynonymValuesTable[synonym].insert(synonymValuePair.second);
  }
}

void QueryEvaluator::filterGroupResultsBySelectSynonyms(
    const vector<Synonym>& selectedSynonyms) {
  vector<IntermediateQueryResult> filteredQueryResults = {};
  for (auto queryResult : groupQueryResults) {
    IntermediateQueryResult newResult = {};
    for (auto synonym : selectedSynonyms) {
      string synonymName = synonym.name;
      if (queryResult.find(synonymName) != queryResult.end()) {
        newResult.insert({synonymName, queryResult.at(synonymName)});
      }
    }
    filteredQueryResults.push_back(newResult);
  }
  groupQueryResults = filteredQueryResults;
}

void QueryEvaluator::mergeGroupResultsIntoFinalResults() {
  if (finalQueryResults.empty()) {
    finalQueryResults = groupQueryResults;
    return;
  }

  vector<IntermediateQueryResult> newFinalQueryResults = {};
  for (auto finalRes : finalQueryResults) {
    for (auto intermediateRes : groupQueryResults) {
      IntermediateQueryResult finalCopy = finalRes;
      IntermediateQueryResult intermediateCopy = intermediateRes;
      finalCopy.merge(intermediateCopy);
      newFinalQueryResults.push_back(finalCopy);
    }
  }
  finalQueryResults = newFinalQueryResults;
}

void QueryEvaluator::filterQuerySynonymsBySelectSynonyms(
    const vector<Synonym>& selectedSynonyms) {
  unordered_set<string> filteredSynonyms = {};
  for (auto synonym : selectedSynonyms) {
    if (queryResultsSynonyms.find(synonym.name) != queryResultsSynonyms.end()) {
      filteredSynonyms.insert(synonym.name);
    }
  }
  queryResultsSynonyms = filteredSynonyms;
}

/* Helpers to Evaluate Based on Previous Clauses ------------------------ */
vector<tuple<Param, Param, ParamPosition>>
QueryEvaluator::getParamsWithPrevResults(const Param& left,
                                         const Param& right) {
  vector<tuple<Param, Param, ParamPosition>> newParams = {};

  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    for (auto result : groupQueryResults) {
      if (result.find(left.value) != result.end() &&
          result.find(right.value) != result.end()) {
        auto leftLiteral =
            getParamTypeAndLiteralFromIndex(left.value, result.at(left.value));
        auto rightLiteral = getParamTypeAndLiteralFromIndex(
            right.value, result.at(right.value));
        Param newLeft = {leftLiteral.first, leftLiteral.second};
        Param newRight = {rightLiteral.first, rightLiteral.second};
        newParams.push_back(make_tuple(newLeft, newRight, ParamPosition::BOTH));
      }
    }

    if (newParams.empty()) {
      // try to get individual s1 || s2 results if s1 && s2 not available
      addLeftParamFromPrevResults(left, right, newParams);
      addRightParamFromPrevResults(left, right, newParams);
    }
  } else if (left.type == ParamType::SYNONYM) {
    addLeftParamFromPrevResults(left, right, newParams);
  } else {
    addRightParamFromPrevResults(left, right, newParams);
  }
  return newParams;
}

void QueryEvaluator::addLeftParamFromPrevResults(
    const Param& left, const Param& right,
    vector<tuple<Param, Param, ParamPosition>>& newParams) {
  for (auto result : clauseSynonymValuesTable[left.value]) {
    auto leftLiteral = getParamTypeAndLiteralFromIndex(left.value, result);
    Param newLeft = {leftLiteral.first, leftLiteral.second};
    newParams.push_back(make_tuple(newLeft, right, ParamPosition::LEFT));
  }
}

void QueryEvaluator::addRightParamFromPrevResults(
    const Param& left, const Param& right,
    vector<tuple<Param, Param, ParamPosition>>& newParams) {
  for (auto result : clauseSynonymValuesTable[right.value]) {
    auto rightLiteral = getParamTypeAndLiteralFromIndex(right.value, result);
    Param newRight = {rightLiteral.first, rightLiteral.second};
    newParams.push_back(make_tuple(left, newRight, ParamPosition::RIGHT));
  }
}

bool QueryEvaluator::getIsBoolClause(
    RelationshipType relationshipType,
    pair<ParamType, ParamType> paramTypesCombo) {
  vector<pair<ParamType, ParamType>> boolParamTypesCombos = {};
  switch (relationshipType) {
    case RelationshipType::FOLLOWS:
    case RelationshipType::FOLLOWS_T:
    case RelationshipType::PARENT:
    case RelationshipType::PARENT_T:
    case RelationshipType::NEXT:
    case RelationshipType::NEXT_BIP:
    case RelationshipType::NEXT_T:
    case RelationshipType::NEXT_BIP_T:
    case RelationshipType::AFFECTS:
      boolParamTypesCombos = {
          {ParamType::INTEGER_LITERAL, ParamType::INTEGER_LITERAL},
          {ParamType::INTEGER_LITERAL, ParamType::WILDCARD},
          {ParamType::WILDCARD, ParamType::INTEGER_LITERAL},
          {ParamType::WILDCARD, ParamType::WILDCARD}};
      break;
    case RelationshipType::USES_S:
    case RelationshipType::MODIFIES_S:
      boolParamTypesCombos = {
          {ParamType::INTEGER_LITERAL, ParamType::NAME_LITERAL},
          {ParamType::INTEGER_LITERAL, ParamType::WILDCARD}};
      break;
    case RelationshipType::USES_P:
    case RelationshipType::MODIFIES_P:
      boolParamTypesCombos = {
          {ParamType::NAME_LITERAL, ParamType::NAME_LITERAL},
          {ParamType::NAME_LITERAL, ParamType::WILDCARD}};
      break;
    case RelationshipType::CALLS:
    case RelationshipType::CALLS_T:
      boolParamTypesCombos = {
          {ParamType::NAME_LITERAL, ParamType::NAME_LITERAL},
          {ParamType::NAME_LITERAL, ParamType::WILDCARD},
          {ParamType::WILDCARD, ParamType::NAME_LITERAL},
          {ParamType::WILDCARD, ParamType::WILDCARD}};
      break;
    default:
      break;
  }
  return find(boolParamTypesCombos.begin(), boolParamTypesCombos.end(),
              paramTypesCombo) != boolParamTypesCombos.end();
}

pair<ParamType, string> QueryEvaluator::getParamTypeAndLiteralFromIndex(
    string synonymName, int index) {
  DesignEntity designEntity = synonymMap.at(synonymName);
  switch (designEntity) {
    case DesignEntity::VARIABLE:
      return make_pair(ParamType::NAME_LITERAL,
                       pkb->getElementAt(TableType::VAR_TABLE, index));
    case DesignEntity::PROCEDURE:
      return make_pair(ParamType::NAME_LITERAL,
                       pkb->getElementAt(TableType::PROC_TABLE, index));
    case DesignEntity::CONSTANT:
      return make_pair(ParamType::NAME_LITERAL,
                       pkb->getElementAt(TableType::CONST_TABLE, index));
    default:
      return make_pair(ParamType::INTEGER_LITERAL, to_string(index));
  }
}

int QueryEvaluator::getIndexFromLiteral(string synonymName, string literal) {
  DesignEntity designEntity = synonymMap.at(synonymName);
  switch (designEntity) {
    case DesignEntity::VARIABLE:
      return pkb->getIndexOf(TableType::VAR_TABLE, literal);
    case DesignEntity::PROCEDURE:
      return pkb->getIndexOf(TableType::PROC_TABLE, literal);
    case DesignEntity::CONSTANT:
      return pkb->getIndexOf(TableType::CONST_TABLE, literal);
    default:
      return stoi(literal);
  }
}

/* Miscellaneous Functions ---------------------------------------------- */
ClauseIncomingResults QueryEvaluator::formatRefResults(
    unordered_set<int> results) {
  ClauseIncomingResults formattedResults = {};
  for (int res : results) {
    formattedResults.push_back({res});
  }

  return formattedResults;
}

unordered_set<int> QueryEvaluator::getAllValuesOfSynonym(string synonymName) {
  DesignEntity designEntity = synonymMap.at(synonymName);
  switch (designEntity) {
    case DesignEntity::STATEMENT:
      return pkb->getAllStmts(DesignEntity::STATEMENT);
    case DesignEntity::PROG_LINE:
      return pkb->getAllStmts(DesignEntity::STATEMENT);
    case DesignEntity::READ:
      return pkb->getAllStmts(DesignEntity::READ);
    case DesignEntity::PRINT:
      return pkb->getAllStmts(DesignEntity::PRINT);
    case DesignEntity::CALL:
      return pkb->getAllStmts(DesignEntity::CALL);
    case DesignEntity::WHILE:
      return pkb->getAllStmts(DesignEntity::WHILE);
    case DesignEntity::IF:
      return pkb->getAllStmts(DesignEntity::IF);
    case DesignEntity::ASSIGN:
      return pkb->getAllStmts(DesignEntity::ASSIGN);
    case DesignEntity::VARIABLE:
      return pkb->getAllElementsAt(TableType::VAR_TABLE);
    case DesignEntity::PROCEDURE:
      return pkb->getAllElementsAt(TableType::PROC_TABLE);
    case DesignEntity::CONSTANT:
      return pkb->getAllElementsAt(TableType::CONST_TABLE);
    default:
      DMOprintErrMsgAndExit(
          "[QE][getAllValuesOfSynonyms] invalid design entity");
      return {};
  }
}

FinalQueryResults QueryEvaluator::getSelectSynonymFinalResults(
    SelectClause select) {
  FinalQueryResults finalResults = {};

  if (select.selectType == SelectType::BOOLEAN) {
    return {{TRUE_SELECT_BOOL_RESULT}};
  }

  if (select.selectType == SelectType::SYNONYMS) {
    for (auto synonym : select.selectSynonyms) {
      if (queryResultsSynonyms.find(synonym.name) ==
          queryResultsSynonyms.end()) {
        unordered_set<int> allValues = getAllValuesOfSynonym(synonym.name);
        for (int value : allValues) {
          groupQueryResults.push_back({{synonym.name, value}});
        }
        mergeGroupResultsIntoFinalResults();
        groupQueryResults.clear();
      }
    }

    for (auto result : finalQueryResults) {
      vector<int> currTupleResult = {};
      for (auto synonym : select.selectSynonyms) {
        currTupleResult.push_back(result[synonym.name]);
      }
      finalResults.push_back(currTupleResult);
    }
  }

  return finalResults;
}

bool QueryEvaluator::checkIsCorrectDesignEntity(int result,
                                                DesignEntity designEntity) {
  switch (designEntity) {
    case DesignEntity::READ:
      return pkb->isStmt(DesignEntity::READ, result);
    case DesignEntity::PRINT:
      return pkb->isStmt(DesignEntity::PRINT, result);
    case DesignEntity::CALL:
      return pkb->isStmt(DesignEntity::CALL, result);
    case DesignEntity::WHILE:
      return pkb->isStmt(DesignEntity::WHILE, result);
    case DesignEntity::IF:
      return pkb->isStmt(DesignEntity::IF, result);
    case DesignEntity::ASSIGN:
      return pkb->isStmt(DesignEntity::ASSIGN, result);
    default:
      return true;
  }
}
