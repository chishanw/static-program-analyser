#include "PatternEvaluator.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "Common/Global.h"

using namespace std;
using namespace query;

PatternEvaluator::PatternEvaluator(PKB* pkb) { this->pkb = pkb; }

unordered_set<int> PatternEvaluator::evaluateAssignPattern(
    const Param& varParam, const query::PatternExpr& patternExpr) {
  MatchType matchType = patternExpr.matchType;
  string expr = patternExpr.expr;

  if (varParam.type == ParamType::NAME_LITERAL) {
    if (matchType == MatchType::EXACT) {
      return pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                        varParam.value, expr);
    } else if (matchType == MatchType::SUB_EXPRESSION) {
      return pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR,
                                        varParam.value, expr);
    } else {
      return pkb->getStmtsForVar(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 varParam.value);
    }
  }

  // varParam.type == ParamType::WILDCARD
  if (matchType == MatchType::EXACT) {
    return pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR, expr);
  } else if (matchType == MatchType::SUB_EXPRESSION) {
    return pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, expr);
  } else {
    return pkb->getAllStmts(DesignEntity::ASSIGN);
  }
}

ClauseIncomingResults PatternEvaluator::evaluateAssignPairPattern(
    const Param& varParam, const query::PatternExpr& patternExpr) {
  // varParam type is synonym here already i.e. a (v, ...)
  MatchType matchType = patternExpr.matchType;
  string expr = patternExpr.expr;

  if (matchType == MatchType::EXACT) {
    return pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                         expr);
  } else if (matchType == MatchType::SUB_EXPRESSION) {
    return
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, expr);
  } else {
    return pkb->getVarMappings(RelationshipType::PTT_ASSIGN_FULL_EXPR);
  }
}

unordered_set<int> PatternEvaluator::evaluateIfPattern(const Param& varParam) {
  if (varParam.type == ParamType::NAME_LITERAL) {
    return pkb->getStmtsForVar(RelationshipType::PTT_IF, varParam.value);
  } else if (varParam.type == ParamType::WILDCARD) {
    return pkb->getAllStmts(DesignEntity::IF);
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateIfPattern] invalid varParam type");
    return {};
  }
}

ClauseIncomingResults PatternEvaluator::evaluateIfPairPattern(
    const Param& varParam) {
  if (varParam.type == ParamType::SYNONYM) {
    return pkb->getVarMappings(RelationshipType::PTT_IF);
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateIfPairPattern] invalid varParam type");
    return {};
  }
}

unordered_set<int> PatternEvaluator::evaluateWhilePattern(
    const Param& varParam) {
  if (varParam.type == ParamType::NAME_LITERAL) {
    return pkb->getStmtsForVar(RelationshipType::PTT_WHILE, varParam.value);
  } else if (varParam.type == ParamType::WILDCARD) {
    return pkb->getAllStmts(DesignEntity::WHILE);
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateWhilePattern] invalid varParam type");
    return {};
  }
}

ClauseIncomingResults PatternEvaluator::evaluateWhilePairPattern(
    const Param& varParam) {
  if (varParam.type == ParamType::SYNONYM) {
    return pkb->getVarMappings(RelationshipType::PTT_WHILE);
  } else {
    DMOprintErrMsgAndExit(
        "[PatternEvaluator][evaluateWhilePairPattern] invalid varParam type");
    return {};
  }
}
