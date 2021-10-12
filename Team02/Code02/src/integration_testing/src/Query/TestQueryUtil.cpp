#include "TestQueryUtil.h"

using namespace std;
using namespace query;

void TestQueryUtil::AddSuchThatClause(vector<ConditionClause>& clauses,
                                      RelationshipType type,
                                      ParamType leftParamType,
                                      string leftParamVal,
                                      ParamType rightParamType,
                                      string rightParamVal) {
  SuchThatClause stClause = {
      type, {leftParamType, leftParamVal}, {rightParamType, rightParamVal}};

  ConditionClause conditionClause = {
      stClause, {}, {}, ConditionClauseType::SUCH_THAT};

  clauses.push_back(conditionClause);
}

void TestQueryUtil::AddPatternClause(vector<ConditionClause>& clauses,
                                     Synonym patternSynonym,
                                     ParamType leftParamType,
                                     string leftParamVal, PatternExpr expr) {
  PatternClause pClause = {patternSynonym, {leftParamType, leftParamVal}, expr};

  ConditionClause conditionClause = {
      {}, pClause, {}, ConditionClauseType::PATTERN};

  clauses.push_back(conditionClause);
}

void TestQueryUtil::AddWithClause(vector<ConditionClause>& clauses,
                                  ParamType leftParamType, string leftParamVal,
                                  ParamType rightParamType,
                                  string rightParamVal) {
  WithClause wClause = {{leftParamType, leftParamVal},
                        {rightParamType, rightParamVal}};

  ConditionClause conditionClause = {
      {}, {}, wClause, ConditionClauseType::WITH};

  clauses.push_back(conditionClause);
}

set<int> TestQueryUtil::getUniqueSelectSingleQEResults(
    vector<vector<int>> results) {
  // used for QE testing to prevent macOS tests from failing
  // due to reversed order of vectors
  set<int> uniqueValues = {};
  for (auto result : results) {
    for (auto val : result) {
      uniqueValues.insert(val);
    }
  }
  return uniqueValues;
}
