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

QueryEvaluator::QueryEvaluator(PKB* pkb) : followsEvaluator(pkb) {
  this->pkb = pkb;
  areAllClausesTrue = true;
  queryResults = {};
  currentQueryResults = {};
  queryResultsSynonyms = {};
}

unordered_set<int> QueryEvaluator::evaluateQuery(
    unordered_map<string, DesignEntity> querySynonymList, SelectClause select) {
  Synonym selectSynonym = select.selectSynonym;
  vector<SuchThatClause> suchThatClauses = select.suchThatClauses;
  vector<PatternClause> patternClauses = select.patternClauses;

  for (auto clause : select.suchThatClauses) {
    evaluateSuchThatClause(clause);

    if (!areAllClausesTrue) {
      // early termination as soon as any clause is false
      return {};
    }
  }

  return getSelectSynonymFinalResults(querySynonymList, selectSynonym.name);
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
    incomingResults = evaluateRefSuchThat(relationshipType, left, right);
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = evaluateRefPairSuchThat(relationshipType, left, right);
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  if (currentQueryResults.empty()) {
    initializeQueryResults(incomingResults, left, right);
  } else {
    addIncomingResult(incomingResults, left, right);
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
    incomingResults = evaluateRefSuchThat(relationshipType, left, right);
  } else {  // 1 syn & 1 wildcard | 2 syn | 2 wildcards
    // evaluate clause that returns a vector of ref pairs
    incomingResults = evaluateRefPairSuchThat(relationshipType, left, right);
  }

  if (incomingResults.empty()) {
    areAllClausesTrue = false;
    return;
  }

  if (currentQueryResults.empty()) {
    initializeQueryResults(incomingResults, left, right);
  } else {
    addIncomingResult(incomingResults, left, right);
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

void QueryEvaluator::addIncomingResult(vector<vector<int>> incomingResults,
                                       const Param& left, const Param& right) {
  vector<string> incomingResultsSynonyms = {};
  bool isLeftParamSynonym = false;
  bool isRightParamSynonym = false;
  bool isLeftSynInQueryResults = false;
  bool isRightSynInQueryResults = false;

  if (left.type == ParamType::SYNONYM) {
    incomingResultsSynonyms.push_back(left.value);
    isLeftParamSynonym = true;
    isLeftSynInQueryResults = queryResultsSynonyms.count(left.value) > 0;
  }
  if (right.type == ParamType::SYNONYM) {
    incomingResultsSynonyms.push_back(right.value);
    isRightParamSynonym = true;
    isRightSynInQueryResults = queryResultsSynonyms.count(right.value) > 0;
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
      DMOprintErrMsgAndExit("[QE][addIncomingResult] shouldn't reach here");
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

        if (queryResult.count(synonymName) > 0) {
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

vector<vector<int>> QueryEvaluator::evaluateRefSuchThat(RelationshipType type,
                                                        const Param& left,
                                                        const Param& right) {
  vector<int> results = {};
  switch (type) {
    case RelationshipType::FOLLOWS:
      results = followsEvaluator.evaluateStmtFollows(left, right);
      break;
    case RelationshipType::FOLLOWS_T:
      results = followsEvaluator.evaluateStmtFollowsT(left, right);
      break;
    default:
      cout << "We will do other types in following iterations\n";
      return {};
  }

  vector<vector<int>> formattedResults = {};
  for (int res : results) {
    formattedResults.push_back({res});
  }

  return formattedResults;
}

vector<vector<int>> QueryEvaluator::evaluateRefPairSuchThat(
    RelationshipType type, const Param& left, const Param& right) {
  switch (type) {
    case RelationshipType::FOLLOWS:
      return followsEvaluator.evaluateStmtPairFollows(left, right);
    case RelationshipType::FOLLOWS_T: {
      vector<pair<int, vector<int>>> results =
          followsEvaluator.evaluateStmtPairFollowsT(left, right);

      // might abstract out in future
      vector<vector<int>> formattedResults = {};
      for (auto pair : results) {
        for (int second : pair.second) {
          formattedResults.push_back({pair.first, second});
        }
      }
      return formattedResults;
    }
    default:
      cout << "We will do other types in following iterations\n";
      return {};
  }
}

bool QueryEvaluator::isBoolClause(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;
  return (leftType == ParamType::LITERAL && rightType == ParamType::LITERAL) ||
         (leftType == ParamType::LITERAL && rightType == ParamType::WILDCARD) ||
         (leftType == ParamType::WILDCARD && rightType == ParamType::LITERAL) ||
         (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD);
}

bool QueryEvaluator::isRefClause(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;
  return (leftType == ParamType::SYNONYM && rightType == ParamType::LITERAL) ||
         (leftType == ParamType::LITERAL && rightType == ParamType::SYNONYM);
}

unordered_set<int> QueryEvaluator::getAllValuesOfSynonym(
    unordered_map<string, DesignEntity> allQuerySynonyms, string synonymName) {
  DesignEntity designEntity = allQuerySynonyms.find(synonymName)->second;
  vector<int> results;
  switch (designEntity) {
    case DesignEntity::STATEMENT:
      results = pkb->getAllStatements();
      return unordered_set<int>(results.begin(), results.end());
    default:
      cout << "Todo: READ, PRINT, CALL, WHILE, IF, ASSIGN, VARIABLE, "
              "CONSTANT, "
              "PROCEDURE\n";
      return {};
  }
}

unordered_set<int> QueryEvaluator::getSelectSynonymFinalResults(
    unordered_map<string, DesignEntity> allQuerySynonyms, string synonymName) {
  if (queryResultsSynonyms.count(synonymName) < 1) {
    return getAllValuesOfSynonym(allQuerySynonyms, synonymName);
  }
  unordered_set<int> results = {};
  // after iter 1 should be selecting from queryResults
  for (unordered_map<string, int> res : currentQueryResults) {
    if (res.count(synonymName) > 0) {
      results.insert(res[synonymName]);
    }
  }
  return results;
}
