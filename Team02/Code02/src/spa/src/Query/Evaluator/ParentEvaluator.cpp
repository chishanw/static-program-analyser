#include "ParentEvaluator.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace query;

ParentEvaluator::ParentEvaluator(PKB* pkb) { this->pkb = pkb; }

bool ParentEvaluator::evaluateBoolParent(const Param& left,
                                         const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // if both literal - Parent(1,2)
  // if one literal + underscore - Parent(_, 2), Parent(2, _)
  // if both underscore - Parent(_, _)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    return pkb->isRs(RelationshipType::PARENT, stoi(left.value),
                     stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getMappings(RelationshipType::PARENT, ParamPosition::BOTH))
                .empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !pkb->getLeft(RelationshipType::PARENT, stoi(right.value)).empty();
  }

  // rightType == ParamType::WILDCARD
  return !(pkb->getRight(RelationshipType::PARENT, stoi(left.value))).empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO ParentEvaluator::evaluateStmtParent(const Param& left,
                                                       const Param& right) {
  // if one literal + synonym - Parent(s, 2), Parent(1, s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getRight(RelationshipType::PARENT, stoi(left.value));
  }

  // rightType == ParamType::INTEGER_LITERAL
  return pkb->getLeft(RelationshipType::PARENT, stoi(right.value));
}

// synonym & wildcard - Parent(s, _) -> getAllParentStmtPair()
// synonym & synonym - Parent(s1, s2) -> getAllParentTStmtPair()
vector<vector<int>> ParentEvaluator::evaluateStmtPairParent(
    const Param& left, const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    results = pkb->getMappings(RelationshipType::PARENT, ParamPosition::BOTH);
  } else if (left.type == ParamType::SYNONYM) {
    results = pkb->getMappings(RelationshipType::PARENT, ParamPosition::LEFT);
  } else {
    // righttype == ParamType::SYNONYM
    results = pkb->getMappings(RelationshipType::PARENT, ParamPosition::RIGHT);
  }
  return vector<vector<int>>(results.begin(), results.end());
}

bool ParentEvaluator::evaluateBoolParentT(const Param& left,
                                          const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    return pkb->isRs(RelationshipType::PARENT_T, stoi(left.value),
                     stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getMappings(RelationshipType::PARENT_T, ParamPosition::BOTH))
                .empty();
  }

  if (leftType == ParamType::WILDCARD) {
    UNO_SET_OF_STMT_NO listOfStmts =
        pkb->getLeft(RelationshipType::PARENT_T, stoi(right.value));
    return !listOfStmts.empty();
  }

  UNO_SET_OF_STMT_NO listOfStmts =
      pkb->getRight(RelationshipType::PARENT_T, stoi(left.value));
  return !listOfStmts.empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO ParentEvaluator::evaluateStmtParentT(const Param& left,
                                                        const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getRight(RelationshipType::PARENT_T, stoi(left.value));
  }

  // rightType == ParamType::INTEGER_LITERAL
  return pkb->getLeft(RelationshipType::PARENT_T, stoi(right.value));
}

// synonym & wildcard - ParentT(s, _) -> getAllParentTStmtPair()
// synonym & synonym -> getAllParentTStmtPair()
vector<vector<int>> ParentEvaluator::evaluateStmtPairParentT(
    const Param& left, const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    results = pkb->getMappings(RelationshipType::PARENT_T, ParamPosition::BOTH);
  } else if (left.type == ParamType::SYNONYM) {
    results = pkb->getMappings(RelationshipType::PARENT_T, ParamPosition::LEFT);
  } else {
    results =
        pkb->getMappings(RelationshipType::PARENT_T, ParamPosition::RIGHT);
  }

  return vector<vector<int>>(results.begin(), results.end());
}
