#include "QueryEvaluator.h"

#include <../../autotester/src/AbstractWrapper.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/Global.h"
#include "FollowsEvaluator.h"

using namespace std;
using namespace query;

QueryEvaluator::QueryEvaluator(PKB* pkb)
    : followsEvaluator(pkb),
      parentEvaluator(pkb),
      usesEvaluator(pkb),
      modifiesEvaluator(pkb),
      callsEvaluator(pkb),
      nextEvaluator(pkb),
      patternEvaluator(pkb),
      withEvaluator(pkb) {
  this->pkb = pkb;
  areAllClausesTrue = true;
  queryResults = {};
  currentQueryResults = {};
  queryResultsSynonyms = {};
}

vector<vector<int>> QueryEvaluator::evaluateQuery(
    unordered_map<string, DesignEntity> synonymMap, SelectClause select) {
  this->synonymMap = synonymMap;

  vector<ConditionClause> conditionClauses = select.conditionClauses;

  for (auto clause : conditionClauses) {
    if (clause.conditionClauseType == ConditionClauseType::SUCH_THAT) {
      evaluateSuchThatClause(clause.suchThatClause);
    } else if (clause.conditionClauseType == ConditionClauseType::PATTERN) {
      evaluatePatternClause(clause.patternClause);
    } else {
      evaluateWithClause(clause.withClause);
    }

    if (!areAllClausesTrue) {
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
  }

  return getSelectSynonymFinalResults(select);
}

/* Evaluate Such That Clauses -------------------------------------------- */
void QueryEvaluator::evaluateSuchThatClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;

  switch (clause.relationshipType) {
    case RelationshipType::FOLLOWS:
      return evaluateFollowsClause(clause);
    case RelationshipType::FOLLOWS_T:
      return evaluateFollowsTClause(clause);
    case RelationshipType::PARENT:
      return evaluateParentClause(clause);
    case RelationshipType::PARENT_T:
      return evaluateParentTClause(clause);
    case RelationshipType::USES_S:
      return evaluateUsesSClause(clause);
    case RelationshipType::USES_P:
      return evaluateUsesPClause(clause);
    case RelationshipType::MODIFIES_S:
      return evaluateModifiesSClause(clause);
    case RelationshipType::MODIFIES_P:
      return evaluateModifiesPClause(clause);
    case RelationshipType::CALLS:
      return evaluateCallsClause(clause);
    case RelationshipType::CALLS_T:
      return evaluateCallsTClause(clause);
    case RelationshipType::NEXT:
      return evaluateNextClause(clause);
    case RelationshipType::NEXT_T:
      return evaluateNextTClause(clause);
    default:
      DMOprintErrMsgAndExit(
          "[QueryEvaluator][evaluateSuchThatClause] invalid relationship type");
  }
}

void QueryEvaluator::evaluateFollowsClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::INTEGER_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  // evaluate clause that returns a boolean
  if (isBoolClause) {  // both literal/wildcard
    areAllClausesTrue = followsEvaluator.evaluateBoolFollows(left, right);
    return;
  }

  // the rest of clauses has at least one synonym
  vector<vector<int>> incomingResults;
  if (isRefClause) {  // 1 syn & 1 literal
    // evaluate clause that returns a vector of single refs
    incomingResults =
        formatRefResults(followsEvaluator.evaluateStmtFollows(left, right));
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = followsEvaluator.evaluateStmtPairFollows(left, right);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateFollowsTClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::INTEGER_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = followsEvaluator.evaluateBoolFollowsT(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(followsEvaluator.evaluateStmtFollowsT(left, right));
  } else {
    incomingResults = formatRefPairResults(
        followsEvaluator.evaluateStmtPairFollowsT(left, right), left.type,
        right.type);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateParentClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::INTEGER_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = parentEvaluator.evaluateBoolParent(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(parentEvaluator.evaluateStmtParent(left, right));
  } else {
    incomingResults = formatRefPairResults(
        parentEvaluator.evaluateStmtPairParent(left, right), left.type,
        right.type);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateParentTClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::INTEGER_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = parentEvaluator.evaluateBoolParentT(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(parentEvaluator.evaluateStmtParentT(left, right));
  } else {
    incomingResults = formatRefPairResults(
        parentEvaluator.evaluateStmtPairParentT(left, right), left.type,
        right.type);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateUsesSClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::NAME_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = usesEvaluator.evaluateBoolUsesS(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(usesEvaluator.evaluateUsesS(left, right));

  } else {
    incomingResults = formatRefPairResults(
        usesEvaluator.evaluatePairUsesS(left, right), left.type, right.type);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateUsesPClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::NAME_LITERAL) ||
                     (left.type == ParamType::NAME_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = usesEvaluator.evaluateBoolUsesP(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(usesEvaluator.evaluateUsesP(left, right));

  } else {
    incomingResults = formatRefPairResults(
        usesEvaluator.evaluatePairUsesP(left, right), left.type, right.type);
  }
  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateModifiesSClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::NAME_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = modifiesEvaluator.evaluateBoolModifiesS(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(modifiesEvaluator.evaluateModifiesS(left, right));
  } else {
    incomingResults = formatRefPairResults(
        modifiesEvaluator.evaluatePairModifiesS(left, right), left.type,
        right.type);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateModifiesPClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::NAME_LITERAL) ||
                     (left.type == ParamType::NAME_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = modifiesEvaluator.evaluateBoolModifiesP(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(modifiesEvaluator.evaluateModifiesP(left, right));
  } else {
    incomingResults = formatRefPairResults(
        modifiesEvaluator.evaluatePairModifiesP(left, right), left.type,
        right.type);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateCallsClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::NAME_LITERAL) ||
                     (left.type == ParamType::NAME_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = callsEvaluator.evaluateBoolCalls(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(callsEvaluator.evaluateProcCalls(left, right));
  } else {
    incomingResults = callsEvaluator.evaluateProcPairCalls(left, right);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateCallsTClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::NAME_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::NAME_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::NAME_LITERAL) ||
                     (left.type == ParamType::NAME_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = callsEvaluator.evaluateBoolCallsT(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(callsEvaluator.evaluateProcCallsT(left, right));
  } else {
    incomingResults = callsEvaluator.evaluateProcPairCallsT(left, right);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateNextClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::INTEGER_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = nextEvaluator.evaluateBoolNext(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults = formatRefResults(nextEvaluator.evaluateNext(left, right));
  } else {
    incomingResults = nextEvaluator.evaluatePairNext(left, right);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

void QueryEvaluator::evaluateNextTClause(SuchThatClause clause) {
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  bool isBoolClause =
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::INTEGER_LITERAL &&
       right.type == ParamType::WILDCARD) ||
      (left.type == ParamType::WILDCARD &&
       right.type == ParamType::INTEGER_LITERAL) ||
      (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD);
  bool isRefClause = (left.type == ParamType::SYNONYM &&
                      right.type == ParamType::INTEGER_LITERAL) ||
                     (left.type == ParamType::INTEGER_LITERAL &&
                      right.type == ParamType::SYNONYM);

  if (isBoolClause) {
    areAllClausesTrue = nextEvaluator.evaluateBoolNextT(left, right);
    return;
  }

  vector<vector<int>> incomingResults;
  if (isRefClause) {
    incomingResults =
        formatRefResults(nextEvaluator.evaluateNextT(left, right));
  } else {
    incomingResults = nextEvaluator.evaluatePairNextT(left, right);
  }

  filterAndAddIncomingResults(incomingResults, left, right);
}

/* Evaluate Pattern Clauses ------------------------------------------------- */
void QueryEvaluator::evaluatePatternClause(PatternClause clause) {
  switch (clause.matchSynonym.entity) {
    case DesignEntity::ASSIGN:
      return evaluateAssignPatternClause(clause);
    case DesignEntity::IF:
      return evaluateIfPatternClause(clause);
    case DesignEntity::WHILE:
      return evaluateWhilePatternClause(clause);
    default:
      DMOprintErrMsgAndExit(
          "[QueryEvaluator][evaluatePatternClause] invalid ptt design entity");
  }
}

void QueryEvaluator::evaluateAssignPatternClause(PatternClause clause) {
  Synonym matchSynonym = clause.matchSynonym;
  Param varParam = clause.leftParam;
  PatternExpr patternExpr = clause.patternExpr;

  vector<vector<int>> incomingResults;

  if (varParam.type == ParamType::NAME_LITERAL ||
      varParam.type == ParamType::WILDCARD) {
    incomingResults = formatRefResults(
        patternEvaluator.evaluateAssignPattern(varParam, patternExpr));
  } else {
    incomingResults =
        patternEvaluator.evaluateAssignPairPattern(varParam, patternExpr);
  }

  Param assignSynonymParam = {ParamType::SYNONYM, matchSynonym.name};
  filterAndAddIncomingResults(incomingResults, assignSynonymParam, varParam);
}

void QueryEvaluator::evaluateIfPatternClause(PatternClause clause) {
  Synonym matchSynonym = clause.matchSynonym;
  Param varParam = clause.leftParam;

  vector<vector<int>> incomingResults;

  if (varParam.type == ParamType::NAME_LITERAL ||
      varParam.type == ParamType::WILDCARD) {
    incomingResults =
        formatRefResults(patternEvaluator.evaluateIfPattern(varParam));
  } else {
    incomingResults = patternEvaluator.evaluateIfPairPattern(varParam);
  }

  Param ifSynonymParam = {ParamType::SYNONYM, matchSynonym.name};
  filterAndAddIncomingResults(incomingResults, ifSynonymParam, varParam);
}

void QueryEvaluator::evaluateWhilePatternClause(PatternClause clause) {
  Synonym matchSynonym = clause.matchSynonym;
  Param varParam = clause.leftParam;

  vector<vector<int>> incomingResults;

  if (varParam.type == ParamType::NAME_LITERAL ||
      varParam.type == ParamType::WILDCARD) {
    incomingResults =
        formatRefResults(patternEvaluator.evaluateWhilePattern(varParam));
  } else {
    incomingResults = patternEvaluator.evaluateWhilePairPattern(varParam);
  }

  Param whileSynonymParam = {ParamType::SYNONYM, matchSynonym.name};
  filterAndAddIncomingResults(incomingResults, whileSynonymParam, varParam);
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

  if (currentQueryResults.empty()) {
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

  pair<bool, vector<QueryResult>> evaluatedResults =
      withEvaluator.evaluateAttributes(left, right, synonymMap,
                                       currentQueryResults);
  bool isClauseTrue = evaluatedResults.first;
  vector<QueryResult> newQueryResults = evaluatedResults.second;

  if (!isClauseTrue) {
    areAllClausesTrue = false;
    return;
  } else {
    currentQueryResults = newQueryResults;
  }
}

/* Filter And Merge Results For Each Clause --------------------------------- */
void QueryEvaluator::filterAndAddIncomingResults(
    vector<vector<int>> incomingResults, const Param& left,
    const Param& right) {
  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);

  if (filteredIncomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::initializeQueryResults(vector<vector<int>> incomingResults,
                                            const Param& left,
                                            const Param& right) {
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    queryResultsSynonyms.insert(left.value);
    queryResultsSynonyms.insert(right.value);

    for (vector<int> incomingResult : incomingResults) {
      currentQueryResults.push_back(
          {{left.value, incomingResult[0]}, {right.value, incomingResult[1]}});
    }
  } else if (left.type == ParamType::SYNONYM) {
    queryResultsSynonyms.insert(left.value);

    for (vector<int> incomingResult : incomingResults) {
      currentQueryResults.push_back({{left.value, incomingResult[0]}});
    }
  } else {
    queryResultsSynonyms.insert(right.value);

    for (vector<int> incomingResult : incomingResults) {
      if (incomingResult.size() == 1) {
        currentQueryResults.push_back({{right.value, incomingResult[0]}});
      } else {
        currentQueryResults.push_back({{right.value, incomingResult[1]}});
      }
    }
  }
}

vector<vector<int>> QueryEvaluator::filterIncomingResults(
    vector<vector<int>> incomingResults, const Param& left,
    const Param& right) {
  vector<vector<int>> finalResults = {};

  if ((left.type == ParamType::SYNONYM) && (right.type == ParamType::SYNONYM)) {
    vector<vector<int>> filteredResults = {};

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

void QueryEvaluator::addIncomingResults(vector<vector<int>> incomingResults,
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

/* Main Algos to Merge Clause Results --------------------------------------- */
void QueryEvaluator::filter(vector<vector<int>> incomingResults,
                            vector<string> incomingResultsSynonyms) {
  vector<QueryResult> newQueryResults = {};

  for (int i = 0; i < currentQueryResults.size(); i++) {
    QueryResult queryResult = currentQueryResults[i];

    for (vector<int> incomingResult : incomingResults) {
      filterValidQueryResults(&newQueryResults, queryResult, incomingResult,
                              incomingResultsSynonyms);
    }
  }
  currentQueryResults = newQueryResults;
  if (currentQueryResults.empty()) {
    areAllClausesTrue = false;
    return;
  }
}

void QueryEvaluator::filterValidQueryResults(
    vector<QueryResult>* newQueryResults, QueryResult queryResult,
    vector<int> incomingResult, vector<string> incomingResultsSynonyms) {
  QueryResult newQueryResult = queryResult;
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
  }
}

void QueryEvaluator::innerJoin(vector<vector<int>> incomingResults,
                               vector<string> incomingResultsSynonyms) {
  vector<QueryResult> newQueryResults = {};
  for (int i = 0; i < currentQueryResults.size(); i++) {
    QueryResult queryResult = currentQueryResults[i];

    for (vector<int> incomingResult : incomingResults) {
      innerJoinValidQueryResults(&newQueryResults, queryResult, incomingResult,
                                 incomingResultsSynonyms);
    }
  }

  currentQueryResults = newQueryResults;
  if (currentQueryResults.empty()) {
    areAllClausesTrue = false;
    return;
  }
}

void QueryEvaluator::innerJoinValidQueryResults(
    vector<QueryResult>* newQueryResults, QueryResult queryResult,
    vector<int> incomingResult, vector<string> incomingResultsSynonyms) {
  QueryResult newQueryResult = queryResult;
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
  }
}

void QueryEvaluator::crossProduct(vector<vector<int>> incomingResults,
                                  vector<string> incomingResultsSynonyms) {
  vector<QueryResult> newQueryResults;

  for (QueryResult queryResult : currentQueryResults) {
    for (vector<int> incomingResult : incomingResults) {
      QueryResult newQueryResult = queryResult;

      for (int i = 0; i < incomingResult.size(); i++) {
        int value = incomingResult[i];
        string synonymName = incomingResultsSynonyms[i];
        newQueryResult[synonymName] = value;
      }

      newQueryResults.push_back(newQueryResult);
    }
  }

  currentQueryResults = newQueryResults;
  if (currentQueryResults.empty()) {
    areAllClausesTrue = false;
    return;
  }
}

/* Miscellaneous Helper Functions ------------------------------------------ */
vector<vector<int>> QueryEvaluator::formatRefResults(
    unordered_set<int> results) {
  vector<vector<int>> formattedResults = {};
  for (int res : results) {
    formattedResults.push_back({res});
  }

  return formattedResults;
}

vector<vector<int>> QueryEvaluator::formatRefPairResults(
    vector<pair<int, vector<int>>> results, ParamType leftType,
    ParamType rightType) {
  vector<vector<int>> formattedResults = {};

  // extract and return results for only the synonyms in the clause
  if (leftType == ParamType::SYNONYM && rightType == ParamType::SYNONYM) {
    for (auto res : results) {
      for (auto second : res.second) {
        formattedResults.push_back({res.first, second});
      }
    }
    return formattedResults;
  }

  if (leftType == ParamType::SYNONYM) {
    for (auto res : results) {
      formattedResults.push_back({res.first});
    }
    return formattedResults;
  }

  // rightType == ParamType::SYNONYM
  for (auto res : results) {
    for (auto second : res.second) {
      formattedResults.push_back({second});
    }
  }
  return formattedResults;
}

unordered_set<int> QueryEvaluator::getAllValuesOfSynonym(string synonymName) {
  DesignEntity designEntity = synonymMap.find(synonymName)->second;
  switch (designEntity) {
    case DesignEntity::STATEMENT:
      return pkb->getAllStmts();
    case DesignEntity::PROG_LINE:
      return pkb->getAllStmts();
    case DesignEntity::READ:
      return pkb->getAllReadStmts();
    case DesignEntity::PRINT:
      return pkb->getAllPrintStmts();
    case DesignEntity::CALL:
      return pkb->getAllCallStmts();
    case DesignEntity::WHILE:
      return pkb->getAllWhileStmts();
    case DesignEntity::IF:
      return pkb->getAllIfStmts();
    case DesignEntity::ASSIGN:
      return pkb->getAllAssignStmts();
    case DesignEntity::VARIABLE:
      return pkb->getAllVariables();
    case DesignEntity::PROCEDURE:
      return pkb->getAllProcedures();
    case DesignEntity::CONSTANT:
      return pkb->getAllConstants();
    default:
      DMOprintErrMsgAndExit(
          "[QE][getAllValuesOfSynonyms] invalid design entity");
      return {};
  }
}

vector<vector<int>> QueryEvaluator::getSelectSynonymFinalResults(
    SelectClause select) {
  vector<vector<int>> finalResults = {};

  if (select.selectType == SelectType::BOOLEAN) {
    return {{TRUE_SELECT_BOOL_RESULT}};
  }

  if (select.selectType == SelectType::SYNONYMS) {
    for (auto synonym : select.selectSynonyms) {
      if (queryResultsSynonyms.find(synonym.name) ==
          queryResultsSynonyms.end()) {
        unordered_set<int> allValues = getAllValuesOfSynonym(synonym.name);
        vector<vector<int>> incomingResults = {};
        for (int value : allValues) {
          incomingResults.push_back({value});
        }
        filterAndAddIncomingResults(incomingResults,
                                    {ParamType::SYNONYM, synonym.name},
                                    {ParamType::WILDCARD, "_"});
      }
    }

    for (auto result : currentQueryResults) {
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
      return pkb->isReadStmt(result);
    case DesignEntity::PRINT:
      return pkb->isPrintStmt(result);
    case DesignEntity::CALL:
      return pkb->isCallStmt(result);
    case DesignEntity::WHILE:
      return pkb->isWhileStmt(result);
    case DesignEntity::IF:
      return pkb->isIfStmt(result);
    case DesignEntity::ASSIGN:
      return pkb->isAssignStmt(result);
    default:
      return true;
  }
}
