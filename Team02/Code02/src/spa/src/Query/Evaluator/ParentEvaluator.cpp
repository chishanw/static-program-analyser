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
    return pkb->isParent(stoi(left.value), stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getAllParentsStmtPairs()).empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return pkb->getParent(stoi(right.value)) != -1;
  }

  // rightType == ParamType::WILDCARD
  return !(pkb->getChildren(stoi(left.value))).empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO ParentEvaluator::evaluateStmtParent(const Param& left,
                                                       const Param& right) {
  // if one literal + synonym - Parent(s, 2), Parent(1, s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getChildren(stoi(left.value));
  }

  // rightType == ParamType::INTEGER_LITERAL
  int s1 = pkb->getParent(stoi(right.value));
  if (s1 != -1) {
    return {s1};
  } else {
    return {};
  }
}

// synonym & wildcard - Parent(s, _) -> getAllParentStmtPair()
// synonym & synonym - Parent(s1, s2) -> getAllParentTStmtPair()
vector<pair<int, vector<int>>> ParentEvaluator::evaluateStmtPairParent(
    const Param& left, const Param& right) {
  return pkb->getAllParentsStmtPairs();
}

bool ParentEvaluator::evaluateBoolParentT(const Param& left,
                                          const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    return pkb->isParentT(stoi(left.value), stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getAllParentsTStmtPairs()).empty();
  }

  if (leftType == ParamType::WILDCARD) {
    UNO_SET_OF_STMT_NO listOfStmts = pkb->getParentsT(stoi(right.value));
    return !listOfStmts.empty();
  }

  UNO_SET_OF_STMT_NO listOfStmts = pkb->getChildrenT(stoi(left.value));
  return !listOfStmts.empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO ParentEvaluator::evaluateStmtParentT(const Param& left,
                                                        const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getChildrenT(stoi(left.value));
  }

  // rightType == ParamType::INTEGER_LITERAL
  return pkb->getParentsT(stoi(right.value));
}

// synonym & wildcard - ParentT(s, _) -> getAllParentTStmtPair()
// synonym & synonym -> getAllParentTStmtPair()
vector<pair<int, vector<int>>> ParentEvaluator::evaluateStmtPairParentT(
    const Param& left, const Param& right) {
  return pkb->getAllParentsTStmtPairs();
}
