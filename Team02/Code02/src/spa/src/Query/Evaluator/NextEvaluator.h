#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

class NextEvaluator {
 public:
  explicit NextEvaluator(PKB*);

  bool evaluateBoolNext(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateNext(const query::Param& left,
                                       const query::Param& right);
  std::vector<std::vector<int>> evaluatePairNext(const query::Param& left,
                                                 const query::Param& right);

  bool evaluateBoolNextT(const query::Param& left, const query::Param& right);
  std::unordered_set<int> evaluateNextT(const query::Param& left,
                                        const query::Param& right);
  std::vector<std::vector<int>> evaluatePairNextT(const query::Param& left,
                                                  const query::Param& right);

 private:
  PKB* pkb;
  std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> nextTCache;
  std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> reverseNextTCache;
  bool getIsNextT(int startStmt, int endStmt);
  std::unordered_set<int> getNextTStmts(int startStmt);
  std::unordered_set<int> getPrevTStmts(int endStmt);
};
