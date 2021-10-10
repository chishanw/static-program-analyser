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
  std::vector<std::vector<int>> evaluateAssignPairPattern(
      const query::Param& varParam, const query::PatternExpr& patternExpr);

  std::unordered_set<int> evaluateIfPattern(const query::Param& varParam);
  std::vector<std::vector<int>> evaluateIfPairPattern(
      const query::Param& varParam);

  std::unordered_set<int> evaluateWhilePattern(const query::Param& varParam);
  std::vector<std::vector<int>> evaluateWhilePairPattern(
      const query::Param& varParam);

 private:
  PKB* pkb;
};
