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
      stClause, {}, ConditionClauseType::SUCH_THAT};

  clauses.push_back(conditionClause);
}

void TestQueryUtil::AddPatternClause(vector<ConditionClause>& clauses,
                                     Synonym patternSynonym,
                                     ParamType leftParamType,
                                     string leftParamVal,
                                     MatchType exprMatchType,
                                     string exprString) {
  PatternClause pClause = {patternSynonym,
                           {leftParamType, leftParamVal},
                           {exprMatchType, exprString}};

  ConditionClause conditionClause = {{}, pClause, ConditionClauseType::PATTERN};

  clauses.push_back(conditionClause);
}
