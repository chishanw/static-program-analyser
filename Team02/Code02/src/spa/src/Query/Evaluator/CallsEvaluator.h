#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <utility>
#include <vector>

class CallsEvaluator {
 public:
  explicit CallsEvaluator(PKB*);

  bool evaluateBoolCalls(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateProcCalls(const query::Param& left,
                                              const query::Param& right);
  std::vector<std::vector<int>> evaluateProcPairCalls(
      const query::Param& left, const query::Param& right);

  bool evaluateBoolCallsT(const query::Param& left,
                            const query::Param& right);
  std::unordered_set<int> evaluateProcCallsT(const query::Param& left,
                                               const query::Param& right);
  std::vector<std::vector<int>> evaluateProcPairCallsT(
      const query::Param& left, const query::Param& right);

 private:
  PKB* pkb;
};
