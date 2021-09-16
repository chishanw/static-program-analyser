#include "QueryEvaluator.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/Global.h"
#include "FollowsEvaluator.h"

using namespace query;

QueryEvaluator::QueryEvaluator(PKB* pkb)
    : followsEvaluator(pkb),
      parentEvaluator(pkb),
      usesEvaluator(pkb),
      modifiesEvaluator(pkb),
      patternEvaluator(pkb) {
  this->pkb = pkb;
  areAllClausesTrue = true;
  queryResults = {};
  currentQueryResults = {};
  queryResultsSynonyms = {};
}

unordered_set<int> QueryEvaluator::evaluateQuery(
    unordered_map<string, DesignEntity> synonymMap, SelectClause select) {
  this->synonymMap = synonymMap;

  Synonym selectSynonym = select.selectSynonym;
  vector<ConditionClause> conditionClauses = select.conditionClauses;

  for (auto clause : conditionClauses) {
    if (clause.conditionClauseType == ConditionClauseType::SUCH_THAT) {
      evaluateSuchThatClause(clause.suchThatClause);
    } else {
      evaluatePatternClause(clause.patternClause);
    }

    if (!areAllClausesTrue) {
      // early termination as soon as any clause is false
      return {};
    }
  }

  return getSelectSynonymFinalResults(selectSynonym.name);
}

void QueryEvaluator::evaluateSuchThatClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;

  switch (clause.relationshipType) {
    case RelationshipType::FOLLOWS:
      evaluateFollowsClause(clause);
      break;

    case RelationshipType::FOLLOWS_T:
      evaluateFollowsTClause(clause);
      break;

    case RelationshipType::PARENT:
      evaluateParentClause(clause);
      break;

    case RelationshipType::PARENT_T:
      evaluateParentTClause(clause);
      break;

    case RelationshipType::USES_S:
      evaluateUsesSClause(clause);
      break;

    case RelationshipType::MODIFIES_S:
      evaluateModifiesSClause(clause);
      break;

    default:
      break;
  }
}

void QueryEvaluator::evaluateFollowsClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  // evaluate clause that returns a boolean
  if (isBoolClause(left, right)) {  // both literal/wildcard
    areAllClausesTrue = followsEvaluator.evaluateBoolFollows(left, right);
    return;
  }

  // the rest of clauses has at least one synonym
  vector<vector<int>> incomingResults;
  if (isRefClause(left, right)) {  // 1 syn & 1 literal
    // evaluate clause that returns a vector of single refs
    incomingResults =
        formatRefResults(followsEvaluator.evaluateStmtFollows(left, right));
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = followsEvaluator.evaluateStmtPairFollows(left, right);
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);
  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::evaluateFollowsTClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  // evaluate clause that returns a boolean
  if (isBoolClause(left, right)) {  // both literal/wildcard
    areAllClausesTrue = followsEvaluator.evaluateBoolFollowsT(left, right);
    return;
  }

  // the rest of clauses has at least one synonym
  vector<vector<int>> incomingResults;
  if (isRefClause(left, right)) {  // 1 syn & 1 literal
    // evaluate clause that returns a vector of single refs
    incomingResults =
        formatRefResults(followsEvaluator.evaluateStmtFollowsT(left, right));
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = formatRefPairResults(
        followsEvaluator.evaluateStmtPairFollowsT(left, right));
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);
  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::evaluateParentClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  // evaluate clause that returns a boolean
  if (isBoolClause(left, right)) {  // both literal/wildcard
    areAllClausesTrue = parentEvaluator.evaluateBoolParent(left, right);
    return;
  }

  // the rest of clauses has at least one synonym
  vector<vector<int>> incomingResults;
  if (isRefClause(left, right)) {  // 1 syn & 1 literal
    // evaluate clause that returns a vector of single refs
    incomingResults =
        formatRefResults(parentEvaluator.evaluateStmtParent(left, right));
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = formatRefPairResults(
        parentEvaluator.evaluateStmtPairParent(left, right));
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);
  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::evaluateParentTClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  // evaluate clause that returns a boolean
  if (isBoolClause(left, right)) {  // both literal/wildcard
    areAllClausesTrue = parentEvaluator.evaluateBoolParentT(left, right);
    return;
  }

  // the rest of clauses has at least one synonym
  vector<vector<int>> incomingResults;
  if (isRefClause(left, right)) {  // 1 syn & 1 literal
    // evaluate clause that returns a vector of single refs
    incomingResults =
        formatRefResults(parentEvaluator.evaluateStmtParentT(left, right));
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = formatRefPairResults(
        parentEvaluator.evaluateStmtPairParentT(left, right));
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);
  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::evaluateUsesSClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  // evaluate clause that returns a boolean
  if (isBoolClause(left, right)) {  // both literal/wildcard
    areAllClausesTrue = usesEvaluator.evaluateBoolUsesS(left, right);
    return;
  }

  // the rest of clauses has at least one synonym
  vector<vector<int>> incomingResults;
  if (isRefClause(left, right)) {  // 1 syn & 1 literal
    // evaluate clause that returns a vector of single refs
    incomingResults =
        formatRefResults(usesEvaluator.evaluateUsesS(left, right));

  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults =
        formatRefPairResults(usesEvaluator.evaluatePairUsesS(left, right));
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);
  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::evaluateModifiesSClause(SuchThatClause clause) {
  auto relationshipType = clause.relationshipType;
  auto left = clause.leftParam;
  auto right = clause.rightParam;

  // evaluate clause that returns a boolean
  if (isBoolClause(left, right)) {  // both literal/wildcard
    areAllClausesTrue = modifiesEvaluator.evaluateBoolModifiesS(left, right);
    return;
  }

  // the rest of clauses has at least one synonym
  vector<vector<int>> incomingResults;
  if (isRefClause(left, right)) {  // 1 syn & 1 literal
    // evaluate clause that returns a vector of single refs
    incomingResults =
        formatRefResults(modifiesEvaluator.evaluateModifiesS(left, right));
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = formatRefPairResults(
        modifiesEvaluator.evaluatePairModifiesS(left, right));
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, left, right);
  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, left, right);
  } else {
    addIncomingResults(filteredIncomingResults, left, right);
  }
}

void QueryEvaluator::evaluatePatternClause(PatternClause clause) {
  Synonym matchSynonym = clause.matchSynonym;
  Param varParam = clause.leftParam;
  PatternExpr patternExpr = clause.patternExpr;

  vector<vector<int>> incomingResults;

  if (varParam.type == ParamType::NAME_LITERAL ||
      varParam.type == ParamType::WILDCARD) {
    incomingResults = formatRefResults(
        patternEvaluator.evaluateAssignPattern(varParam, patternExpr));
  } else {
    // synonym
    incomingResults =
        patternEvaluator.evaluateAssignPairPattern(varParam, patternExpr);
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  Param assignSynonymParam = {ParamType::SYNONYM, matchSynonym.name};

  vector<vector<int>> filteredIncomingResults =
      filterIncomingResults(incomingResults, assignSynonymParam, varParam);
  if (currentQueryResults.empty()) {
    initializeQueryResults(filteredIncomingResults, assignSynonymParam,
                           varParam);
  } else {
    addIncomingResults(filteredIncomingResults, assignSynonymParam, varParam);
  }
}

void QueryEvaluator::initializeQueryResults(vector<vector<int>> incomingResults,
                                            const Param& left,
                                            const Param& right) {
  vector<string> incomingResultsSynonyms = {};
  if (left.type == ParamType::SYNONYM) {
    incomingResultsSynonyms.push_back(left.value);
    queryResultsSynonyms.insert(left.value);
  }
  if (right.type == ParamType::SYNONYM) {
    incomingResultsSynonyms.push_back(right.value);
    queryResultsSynonyms.insert(right.value);
  }
  if (incomingResultsSynonyms.size() == 2) {
    for (vector<int> incomingResult : incomingResults) {
      currentQueryResults.push_back(
          {{incomingResultsSynonyms[0], incomingResult[0]},
           {incomingResultsSynonyms[1], incomingResult[1]}});
    }
  } else {
    for (vector<int> incomingResult : incomingResults) {
      currentQueryResults.push_back(
          {{incomingResultsSynonyms[0], incomingResult[0]}});
    }
  }
}

vector<vector<int>> QueryEvaluator::filterIncomingResults(
    vector<vector<int>> incomingResults, const Param& left,
    const Param& right) {
  vector<vector<int>> filteredResults = {};

  if ((left.type == ParamType::SYNONYM) && (right.type == ParamType::SYNONYM)) {
    for (vector<int> incomingResult : incomingResults) {
      bool isLeftSynCorrectDesignEntity = checkIsCorrectDesignEntity(
          incomingResult.front(), synonymMap[left.value]);
      bool isRightSynCorrectDesignEntity = checkIsCorrectDesignEntity(
          incomingResult.back(), synonymMap[right.value]);
      if (isLeftSynCorrectDesignEntity && isRightSynCorrectDesignEntity) {
        filteredResults.push_back(incomingResult);
      }
    }
    return filteredResults;
  }

  if (left.type == ParamType::SYNONYM) {
    for (vector<int> incomingResult : incomingResults) {
      bool isLeftSynCorrectDesignEntity = checkIsCorrectDesignEntity(
          incomingResult.front(), synonymMap[left.value]);
      if (isLeftSynCorrectDesignEntity) {
        filteredResults.push_back(incomingResult);
      }
    }
    return filteredResults;
  }

  // right.type == ParamType::SYNONYM
  for (vector<int> incomingResult : incomingResults) {
    bool isRightSynCorrectDesignEntity = checkIsCorrectDesignEntity(
        incomingResult.front(), synonymMap[right.value]);
    if (isRightSynCorrectDesignEntity) {
      filteredResults.push_back(incomingResult);
    }
  }
  return filteredResults;
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
  }
  if (right.type == ParamType::SYNONYM) {
    isRightParamSynonym = true;
    isRightSynInQueryResults =
        queryResultsSynonyms.find(right.value) != queryResultsSynonyms.end();
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

    return crossProduct(incomingResults, {left.value, right.value});

  } else if (isRightParamSynonym) {
    if (isRightSynInQueryResults) {
      return filter(incomingResults, {right.value});
    }

    return crossProduct(incomingResults, {left.value, right.value});

  } else {
    // both wild cards, do nothing, whether incomingResults is empty is alr
    // checked before calling this method
    if (incomingResults.empty()) {
      DMOprintErrMsgAndExit("[QE][addIncomingResults] shouldn't reach here");
    }
  }
}

void QueryEvaluator::filter(vector<vector<int>> incomingResults,
                            vector<string> incomingResultsSynonyms) {
  vector<unordered_map<string, int>> newQueryResults = {};

  for (int i = 0; i < currentQueryResults.size(); i++) {
    unordered_map<string, int> queryResult = currentQueryResults[i];

    for (vector<int> incomingResult : incomingResults) {
      unordered_map<string, int> newQueryResult = queryResult;
      bool isValidQueryResult = true;

      for (int j = 0; j < incomingResult.size(); j++) {
        int value = incomingResult[j];
        string synonymName = incomingResultsSynonyms[j];

        if (queryResult[synonymName] != value) {
          isValidQueryResult = false;
          break;
        }
      }

      if (isValidQueryResult) {
        newQueryResults.push_back(newQueryResult);
      }
    }
  }
  currentQueryResults = newQueryResults;
}

void QueryEvaluator::innerJoin(vector<vector<int>> incomingResults,
                               vector<string> incomingResultsSynonyms) {
  vector<unordered_map<string, int>> newQueryResults = {};
  for (int i = 0; i < currentQueryResults.size(); i++) {
    unordered_map<string, int> queryResult = currentQueryResults[i];

    for (vector<int> incomingResult : incomingResults) {
      unordered_map<string, int> newQueryResult = queryResult;
      bool isValidQueryResult = true;

      // TODO(qe): not optimised for the data shape returned from pkb right now,
      // amy need to swap the for loops above and below
      for (int j = 0; j < incomingResult.size(); j++) {
        int value = incomingResult[j];
        string synonymName = incomingResultsSynonyms[j];

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
        newQueryResults.push_back(newQueryResult);
      }
    }
  }

  currentQueryResults = newQueryResults;
}

void QueryEvaluator::crossProduct(vector<vector<int>> incomingResult,
                                  vector<string> incomingResultsSynonyms) {
  // add new synonyms to queryResultsSynonyms
  for (string synonym : incomingResultsSynonyms) {
    queryResultsSynonyms.insert(synonym);
  }

  vector<unordered_map<string, int>> newQueryResults;

  for (unordered_map<string, int> queryResult : currentQueryResults) {
    for (vector<int> res : incomingResult) {
      unordered_map<string, int> newQueryResult = queryResult;

      for (int i = 0; i < res.size(); i++) {
        int value = res[i];
        string synonymName = incomingResultsSynonyms[i];
        newQueryResult[synonymName] = value;
      }

      newQueryResults.push_back(newQueryResult);
    }
  }

  currentQueryResults = newQueryResults;
}

bool QueryEvaluator::isBoolClause(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;
  return (leftType == ParamType::INTEGER_LITERAL &&
          rightType == ParamType::INTEGER_LITERAL) ||
         (leftType == ParamType::INTEGER_LITERAL &&
          rightType == ParamType::WILDCARD) ||
         (leftType == ParamType::WILDCARD &&
          rightType == ParamType::INTEGER_LITERAL) ||
         (leftType == ParamType::WILDCARD &&
          rightType == ParamType::WILDCARD) ||
         (leftType == ParamType::INTEGER_LITERAL &&
          rightType == ParamType::NAME_LITERAL);
}

bool QueryEvaluator::isRefClause(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;
  return (leftType == ParamType::SYNONYM &&
          rightType == ParamType::INTEGER_LITERAL) ||
         (leftType == ParamType::INTEGER_LITERAL &&
          rightType == ParamType::SYNONYM) ||
         (leftType == ParamType::SYNONYM &&
          rightType == ParamType::NAME_LITERAL);
}

vector<vector<int>> QueryEvaluator::formatRefResults(
    unordered_set<int> results) {
  vector<vector<int>> formattedResults = {};
  for (int res : results) {
    formattedResults.push_back({res});
  }

  return formattedResults;
}

vector<vector<int>> QueryEvaluator::formatRefPairResults(
    vector<pair<int, vector<int>>> results) {
  vector<vector<int>> formattedResults = {};
  for (auto pair : results) {
    for (int second : pair.second) {
      formattedResults.push_back({pair.first, second});
    }
  }
  return formattedResults;
}

unordered_set<int> QueryEvaluator::getAllValuesOfSynonym(string synonymName) {
  DesignEntity designEntity = synonymMap.find(synonymName)->second;
  switch (designEntity) {
    case DesignEntity::STATEMENT:
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

unordered_set<int> QueryEvaluator::getSelectSynonymFinalResults(
    string synonymName) {
  if (queryResultsSynonyms.find(synonymName) == queryResultsSynonyms.end()) {
    return getAllValuesOfSynonym(synonymName);
  }
  unordered_set<int> results = {};
  // after iter 1 should be selecting from queryResults
  for (unordered_map<string, int> res : currentQueryResults) {
    if (res.find(synonymName) != res.end()) {
      results.insert(res[synonymName]);
    }
  }
  return results;
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
