#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <utility>
#include <vector>

class UsesEvaluator {
 public:
  explicit UsesEvaluator(PKB*);

  bool evaluateBoolUsesS(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateUsesS(const query::Param& left,
                                        const query::Param& right);
  std::vector<std::pair<int, std::vector<int>>> evaluatePairUsesS(
      const query::Param& left, const query::Param& right);

  bool evaluateBoolUsesP(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateUsesP(const query::Param& left,
                                        const query::Param& right);
  std::vector<std::pair<int, std::vector<int>>> evaluatePairUsesP(
      const query::Param& left, const query::Param& right);

 private:
  PKB* pkb;
};
