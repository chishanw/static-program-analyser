#include "TestQueryUtil.h"

using namespace std;
using namespace query;

void TestQueryUtil::AddSuchThatClause(vector<ConditionClause>& clauses,
                                      RelationshipType type,
                                      ParamType leftParamType,
                                      string leftParamVal,
                                      ParamType rightParamType,
                                      string rightParamVal) {
  ConditionClause conditionClause = BuildSuchThatClause(
      type, leftParamType, leftParamVal, rightParamType, rightParamVal);
  clauses.push_back(conditionClause);
}

void TestQueryUtil::AddPatternClause(vector<ConditionClause>& clauses,
                                     Synonym patternSynonym,
                                     ParamType leftParamType,
                                     string leftParamVal, PatternExpr expr) {
  ConditionClause conditionClause =
      BuildPatternClause(patternSynonym, leftParamType, leftParamVal, expr);
  clauses.push_back(conditionClause);
}

void TestQueryUtil::AddWithClause(vector<ConditionClause>& clauses,
                                  ParamType leftParamType, string leftParamVal,
                                  ParamType rightParamType,
                                  string rightParamVal) {
  ConditionClause conditionClause = BuildWithClause(
      leftParamType, leftParamVal, rightParamType, rightParamVal);
  clauses.push_back(conditionClause);
}

query::ConditionClause TestQueryUtil::BuildSuchThatClause(
    RelationshipType type, ParamType leftParamType, string leftParamVal,
    ParamType rightParamType, string rightParamVal) {
  SuchThatClause stClause = {
      type, {leftParamType, leftParamVal}, {rightParamType, rightParamVal}};
  ConditionClause conditionClause = {
      stClause, {}, {}, ConditionClauseType::SUCH_THAT};
  return conditionClause;
}

query::ConditionClause TestQueryUtil::BuildPatternClause(
    Synonym patternSynonym, ParamType leftParamType, string leftParamVal,
    PatternExpr expr) {
  PatternClause pClause = {patternSynonym, {leftParamType, leftParamVal}, expr};
  ConditionClause conditionClause = {
      {}, pClause, {}, ConditionClauseType::PATTERN};
  return conditionClause;
}

query::ConditionClause TestQueryUtil::BuildWithClause(ParamType leftParamType,
                                                      string leftParamVal,
                                                      ParamType rightParamType,
                                                      string rightParamVal) {
  WithClause wClause = {{leftParamType, leftParamVal},
                        {rightParamType, rightParamVal}};
  ConditionClause conditionClause = {
      {}, {}, wClause, ConditionClauseType::WITH};
  return conditionClause;
}

query::FinalQueryResults TestQueryUtil::EvaluateQuery(
    PKB* pkb, vector<ConditionClause> clauses, SelectType selectType,
    unordered_map<string, DesignEntity> synonymMap,
    vector<Synonym> selectSynonyms) {
  SelectClause select = {{selectSynonyms}, selectType, clauses};

  QueryOptimizer* opt = new QueryOptimizer(pkb);
  opt->PreprocessClauses(synonymMap, select);

  QueryEvaluator qe(pkb, opt);
  return qe.evaluateQuery(synonymMap, select);
}

set<int> TestQueryUtil::GetUniqueSelectSingleQEResults(
    query::FinalQueryResults results) {
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
