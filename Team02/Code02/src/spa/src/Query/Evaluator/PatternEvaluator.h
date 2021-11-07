#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <vector>

class PatternEvaluator {
 public:
  explicit PatternEvaluator(PKB*);

  std::unordered_set<int> evaluateAssignPattern(
      const query::Param& varParam, const query::PatternExpr& patternExpr);
  query::ClauseIncomingResults evaluateAssignPairPattern(
      const query::Param& varParam, const query::PatternExpr& patternExpr);

  std::unordered_set<int> evaluateIfPattern(const query::Param& varParam);
  query::ClauseIncomingResults evaluateIfPairPattern(
      const query::Param& varParam);

  std::unordered_set<int> evaluateWhilePattern(const query::Param& varParam);
  query::ClauseIncomingResults evaluateWhilePairPattern(
      const query::Param& varParam);

 private:
  PKB* pkb;
};
