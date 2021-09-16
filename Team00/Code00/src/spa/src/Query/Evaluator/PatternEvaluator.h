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

 private:
  PKB* pkb;
};
