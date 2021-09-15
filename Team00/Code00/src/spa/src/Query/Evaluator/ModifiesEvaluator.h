#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <utility>
#include <vector>

class ModifiesEvaluator {
 public:
  explicit ModifiesEvaluator(PKB*);

  bool evaluateBoolModifiesS(const query::Param& left,
                             const query::Param& right);
  std::unordered_set<int> evaluateModifiesS(const query::Param& left,
                                            const query::Param& right);
  std::vector<std::pair<int, std::vector<int>>> evaluatePairModifiesS(
      const query::Param& left, const query::Param& right);

 private:
  PKB* pkb;
};
