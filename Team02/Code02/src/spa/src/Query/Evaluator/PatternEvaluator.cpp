#include "PatternEvaluator.h"

#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "Common/Global.h"

using namespace query;

PatternEvaluator::PatternEvaluator(PKB* pkb) { this->pkb = pkb; }

unordered_set<int> PatternEvaluator::evaluateAssignPattern(
    const Param& varParam, const query::PatternExpr& patternExpr) {
  MatchType matchType = patternExpr.matchType;
  string expr = patternExpr.expr;

  if (varParam.type == ParamType::NAME_LITERAL) {
    if (matchType == MatchType::EXACT) {
      return pkb->getAssignForVarAndFullExpr(varParam.value, expr);
    } else if (matchType == MatchType::SUB_EXPRESSION) {
      return pkb->getAssignForVarAndSubExpr(varParam.value, expr);
    } else {
      return pkb->getAssignForVar(varParam.value);
    }
  }

  // varParam.type == ParamType::WILDCARD
  if (matchType == MatchType::EXACT) {
    return pkb->getAssignForFullExpr(expr);
  } else if (matchType == MatchType::SUB_EXPRESSION) {
    return pkb->getAssignForSubExpr(expr);
  } else {
    return pkb->getAllAssignStmts();
  }
}

vector<vector<int>> PatternEvaluator::evaluateAssignPairPattern(
    const Param& varParam, const query::PatternExpr& patternExpr) {
  // varParam type is synonym here already i.e. a (v, ...)
  MatchType matchType = patternExpr.matchType;
  string expr = patternExpr.expr;

  if (matchType == MatchType::EXACT) {
    return pkb->getAssignVarPairsForFullExpr(expr);
  } else if (matchType == MatchType::SUB_EXPRESSION) {
    return pkb->getAssignVarPairsForSubExpr(expr);
  } else {
    return pkb->getAssignVarPairs();
  }
}

unordered_set<int> PatternEvaluator::evaluateIfPattern(const Param& varParam) {
  if (varParam.type == ParamType::NAME_LITERAL) {
    return pkb->getIfStmtForVar(varParam.value);
  } else if (varParam.type == ParamType::WILDCARD) {
    return pkb->getAllIfStmts();
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateIfPattern] invalid varParam type");
    return {};
  }
}

vector<vector<int>> PatternEvaluator::evaluateIfPairPattern(
    const Param& varParam) {
  if (varParam.type == ParamType::SYNONYM) {
    return pkb->getIfStmtVarPairs();
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateIfPairPattern] invalid varParam type");
    return {};
  }
}

unordered_set<int> PatternEvaluator::evaluateWhilePattern(
    const Param& varParam) {
  if (varParam.type == ParamType::NAME_LITERAL) {
    return pkb->getWhileStmtForVar(varParam.value);
  } else if (varParam.type == ParamType::WILDCARD) {
    return pkb->getAllWhileStmts();
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateWhilePattern] invalid varParam type");
    return {};
  }
}

vector<vector<int>> PatternEvaluator::evaluateWhilePairPattern(
    const Param& varParam) {
  if (varParam.type == ParamType::SYNONYM) {
    return pkb->getWhileStmtVarPairs();
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateWhilePairPattern] invalid varParam type");
    return {};
  }
}
