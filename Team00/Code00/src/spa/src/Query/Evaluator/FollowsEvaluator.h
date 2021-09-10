#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <utility>
#include <vector>

class FollowsEvaluator {
 public:
  explicit FollowsEvaluator(PKB*);

  bool evaluateBoolFollows(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateStmtFollows(const query::Param& left,
                                              const query::Param& right);
  std::vector<std::vector<int>> evaluateStmtPairFollows(
      const query::Param& left, const query::Param& right);

  bool evaluateBoolFollowsT(const query::Param& left,
                            const query::Param& right);
  std::unordered_set<int> evaluateStmtFollowsT(const query::Param& left,
                                               const query::Param& right);
  std::vector<std::pair<int, std::vector<int>>> evaluateStmtPairFollowsT(
      const query::Param& left, const query::Param& right);

 private:
  PKB* pkb;
};
