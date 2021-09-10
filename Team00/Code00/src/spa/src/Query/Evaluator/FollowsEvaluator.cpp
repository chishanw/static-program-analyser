#include "FollowsEvaluator.h"

#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace query;

FollowsEvaluator::FollowsEvaluator(PKB* pkb) { this->pkb = pkb; }

bool FollowsEvaluator::evaluateBoolFollows(const Param& left,
                                           const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // if both literal - Follows(1,2)
  // if one literal + underscore - Follows(_, 2), Follows(2, _)
  // if both underscore - Follows(_, _)
  if (leftType == ParamType::LITERAL && rightType == ParamType::LITERAL) {
    STMT_NO s2 = pkb->getFollows(stoi(left.value));
    return s2 == stoi(right.value);
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    // todo after pkb implements necessary methods
    // return !getAllStmtPairs().empty();
    return true;
  }

  if (leftType == ParamType::WILDCARD) {
    return pkb->getFollowedBy(stoi(right.value)) != -1;
  }

  return pkb->getFollows(stoi(left.value)) != -1;
}

// synonym & literal
UNO_SET_OF_STMT_NO FollowsEvaluator::evaluateStmtFollows(const Param& left,
                                                         const Param& right) {
  // if one literal + synonym - Follows(s, 2), Follows(1, s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::LITERAL) {
    STMT_NO s2 = pkb->getFollows(stoi(left.value));
    if (s2 != -1) {
      return {s2};
    } else {
      return {};
    }
  }

  // rightType == ParamType::LITERAL
  STMT_NO s1 = pkb->getFollowedBy(stoi(right.value));
  if (s1 != -1) {
    return {s1};
  } else {
    return {};
  }
}

// synonym & wildcard - Follows(s, _) -> getAllFollowsStmtPair()
// synonym & synonym - Follows(s1, s2) -> getAllFollowsTStmtPair()
vector<vector<int>> FollowsEvaluator::evaluateStmtPairFollows(
    const Param& left, const Param& right) {
  if (left.type == ParamType::WILDCARD) {
    // return pkb->getAllFollowsStatementPairs(); and extract out right param
    return {{2}, {3}};
  }
  if (right.type == ParamType::WILDCARD) {
    // return pkb->getAllFollowsStatementPairs(); and extract out left param
    return {{1}, {2}};
  }
  // return pkb->getAllFollowsStatementPairs();
  return {{1, 2}, {2, 3}};
}

bool FollowsEvaluator::evaluateBoolFollowsT(const Param& left,
                                            const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::LITERAL && rightType == ParamType::LITERAL) {
    return pkb->isFollowsT(stoi(left.value), stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    // todo after pkb implements necessary methods
    // return !getAllStmtPairs().empty();
    return true;
  }

  if (leftType == ParamType::WILDCARD) {
    UNO_SET_OF_STMT_NO listOfStmts = pkb->getFollowedTBy(stoi(right.value));
    return !listOfStmts.empty();
  }

  UNO_SET_OF_STMT_NO listOfStmts = pkb->getFollowsT(stoi(left.value));
  return !listOfStmts.empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO FollowsEvaluator::evaluateStmtFollowsT(const Param& left,
                                                          const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::LITERAL) {
    return pkb->getFollowsT(stoi(left.value));
  }

  // rightType == ParamType::LITERAL
  return pkb->getFollowedTBy(stoi(right.value));
}

// synonym & wildcard - FollowsT(s, _) -> getAllFollowsTStmtPair()
// synonym & synonym -> getAllFollowsTStmtPair()
vector<pair<int, vector<int>>> FollowsEvaluator::evaluateStmtPairFollowsT(
    const Param& left, const Param& right) {
  // return pkb->getAllFollowsTStatementPairs();
  return {{1, {2, 3}}, {2, {3}}};
}
