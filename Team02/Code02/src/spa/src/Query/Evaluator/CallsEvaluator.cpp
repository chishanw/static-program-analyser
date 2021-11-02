#include "CallsEvaluator.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace query;

CallsEvaluator::CallsEvaluator(PKB* pkb) { this->pkb = pkb; }

bool CallsEvaluator::evaluateBoolCalls(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // if both literal - Calls('proc1','proc2')
  // if one literal + underscore - Calls(_, 'proc2'), Calls('proc2', _)
  // if both underscore - Calls(_, _)
  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    unordered_set<PROC_IDX> procsCalled = pkb->getProcsCalledBy(left.value);
    return procsCalled.find(pkb->getIndexOf(TableType::PROC_TABLE,
        right.value)) != procsCalled.end();
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getAllCallsPairs()).empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !(pkb->getCallerProcs(right.value).empty());
  }

  return !(pkb->getProcsCalledBy(left.value).empty());
}

// synonym & literal
unordered_set<int> CallsEvaluator::evaluateProcCalls(const Param& left,
                                                     const Param& right) {
  // if one literal + synonym - Calls(s, 'proc2'), Calls('proc1', s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::NAME_LITERAL) {
    return pkb->getProcsCalledBy(left.value);
  }

  // rightType == ParamType::LITERAL
  return pkb->getCallerProcs(right.value);
}

// synonym & wildcard - Calls(s, _) -> getAllCallsStmtPair()
// synonym & synonym - Calls(s1, s2) -> getAllCallsTStmtPair()
vector<vector<int>> CallsEvaluator::evaluateProcPairCalls(const Param& left,
                                                          const Param& right) {
  vector<pair<PROC_IDX, vector<PROC_IDX>>> results = pkb->getAllCallsPairs();
  vector<vector<int>> formattedResults = {};

  if (left.type == ParamType::WILDCARD) {
    for (auto result : results) {
      for (auto second : result.second) {
        formattedResults.push_back({second});
      }
    }
    return formattedResults;
  }
  if (right.type == ParamType::WILDCARD) {
    for (auto result : results) {
      formattedResults.push_back({result.first});
    }
    return formattedResults;
  }
  // both synonyms
  for (auto result : results) {
    for (auto second : result.second) {
      formattedResults.push_back({result.first, second});
    }
  }
  return formattedResults;
}

bool CallsEvaluator::evaluateBoolCallsT(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    unordered_set<PROC_IDX> procsCalledT = pkb->getProcsCalledTBy(left.value);
    return procsCalledT.find(pkb->getIndexOf(TableType::PROC_TABLE,
        right.value)) != procsCalledT.end();
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getAllCallsTPairs()).empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !(pkb->getCallerTProcs(right.value).empty());
  }

  return !(pkb->getProcsCalledTBy(left.value).empty());
}

unordered_set<int> CallsEvaluator::evaluateProcCallsT(const Param& left,
                                                      const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::NAME_LITERAL) {
    return pkb->getProcsCalledTBy(left.value);
  }
  return pkb->getCallerTProcs(right.value);
}

vector<vector<int>> CallsEvaluator::evaluateProcPairCallsT(const Param& left,
                                                           const Param& right) {
  vector<pair<PROC_IDX, vector<PROC_IDX>>> results = pkb->getAllCallsTPairs();
  vector<vector<int>> formattedResults = {};

  if (left.type == ParamType::WILDCARD) {
    for (auto result : results) {
      for (auto second : result.second) {
        formattedResults.push_back({second});
      }
    }
    return formattedResults;
  }
  if (right.type == ParamType::WILDCARD) {
    for (auto result : results) {
      formattedResults.push_back({result.first});
    }
    return formattedResults;
  }
  // both synonyms
  for (auto result : results) {
    for (auto second : result.second) {
      formattedResults.push_back({result.first, second});
    }
  }
  return formattedResults;
}
