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
    unordered_set<PROC_IDX> procsCalled = pkb->getRight(
        RelationshipType::CALLS, TableType::PROC_TABLE, left.value);
    return procsCalled.find(pkb->getIndexOf(TableType::PROC_TABLE,
                                            right.value)) != procsCalled.end();
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getMappings(RelationshipType::CALLS, ParamPosition::BOTH))
                .empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !(pkb->getLeft(RelationshipType::CALLS, TableType::PROC_TABLE,
                          right.value)
                 .empty());
  }

  return !(
      pkb->getRight(RelationshipType::CALLS, TableType::PROC_TABLE, left.value)
          .empty());
}

// synonym & literal
unordered_set<int> CallsEvaluator::evaluateProcCalls(const Param& left,
                                                     const Param& right) {
  // if one literal + synonym - Calls(s, 'proc2'), Calls('proc1', s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::NAME_LITERAL) {
    return pkb->getRight(RelationshipType::CALLS, TableType::PROC_TABLE,
                         left.value);
  }

  // rightType == ParamType::LITERAL
  return pkb->getLeft(RelationshipType::CALLS, TableType::PROC_TABLE,
                      right.value);
}

// synonym & wildcard - Calls(s, _) -> getAllCallsStmtPair()
// synonym & synonym - Calls(s1, s2) -> getAllCallsTStmtPair()
vector<vector<int>> CallsEvaluator::evaluateProcPairCalls(const Param& left,
                                                          const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::WILDCARD) {
    results = pkb->getMappings(RelationshipType::CALLS, ParamPosition::RIGHT);

  } else if (right.type == ParamType::WILDCARD) {
    results = pkb->getMappings(RelationshipType::CALLS, ParamPosition::LEFT);

  } else {
    results = pkb->getMappings(RelationshipType::CALLS, ParamPosition::BOTH);
  }

  auto formattedResults = vector<vector<int>>(results.begin(), results.end());
  return formattedResults;
}

bool CallsEvaluator::evaluateBoolCallsT(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    unordered_set<PROC_IDX> procsCalledT = pkb->getRight(
        RelationshipType::CALLS_T, TableType::PROC_TABLE, left.value);
    return procsCalledT.find(pkb->getIndexOf(
               TableType::PROC_TABLE, right.value)) != procsCalledT.end();
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getMappings(RelationshipType::CALLS_T, ParamPosition::BOTH))
                .empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !(pkb->getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                          right.value)
                 .empty());
  }

  return !(pkb->getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                         left.value)
               .empty());
}

unordered_set<int> CallsEvaluator::evaluateProcCallsT(const Param& left,
                                                      const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::NAME_LITERAL) {
    return pkb->getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                         left.value);
  }
  return pkb->getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                      right.value);
}

vector<vector<int>> CallsEvaluator::evaluateProcPairCallsT(const Param& left,
                                                           const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    results = pkb->getMappings(RelationshipType::CALLS_T, ParamPosition::BOTH);
  } else if (left.type == ParamType::SYNONYM) {
    results = pkb->getMappings(RelationshipType::CALLS_T, ParamPosition::LEFT);
  } else {
    // rightType == ParamType::SYNONYM
    results = pkb->getMappings(RelationshipType::CALLS_T, ParamPosition::RIGHT);
  }
  return vector<vector<int>>(results.begin(), results.end());
}
