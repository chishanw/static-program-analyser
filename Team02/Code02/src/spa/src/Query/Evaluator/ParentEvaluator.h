#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <utility>
#include <vector>

class ParentEvaluator {
 public:
  explicit ParentEvaluator(PKB*);

  bool evaluateBoolParent(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateStmtParent(const query::Param& left,
                                             const query::Param& right);
  std::vector<std::vector<int>> evaluateStmtPairParent(
      const query::Param& left, const query::Param& right);

  bool evaluateBoolParentT(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateStmtParentT(const query::Param& left,
                                              const query::Param& right);
  std::vector<std::vector<int>> evaluateStmtPairParentT(
      const query::Param& left, const query::Param& right);

 private:
  PKB* pkb;
};
