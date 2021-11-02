#include "FollowsEvaluator.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace query;

FollowsEvaluator::FollowsEvaluator(PKB* pkb) { this->pkb = pkb; }

bool FollowsEvaluator::evaluateBoolFollows(const Param& left,
                                           const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // if both literal - Follows(1,2)
  // if one literal + underscore - Follows(_, 2), Follows(2, _)
  // if both underscore - Follows(_, _)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    return pkb->isRs(RelationshipType::FOLLOWS, stoi(left.value),
                     stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getMappings(RelationshipType::FOLLOWS, ParamPosition::BOTH))
                .empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !pkb->getLeft(RelationshipType::FOLLOWS, stoi(right.value)).empty();
  }

  return !pkb->getRight(RelationshipType::FOLLOWS, stoi(left.value)).empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO FollowsEvaluator::evaluateStmtFollows(const Param& left,
                                                         const Param& right) {
  // if one literal + synonym - Follows(s, 2), Follows(1, s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getRight(RelationshipType::FOLLOWS, stoi(left.value));
  }
  // rightType == ParamType::LITERAL
  return pkb->getLeft(RelationshipType::FOLLOWS, stoi(right.value));
}

// synonym & wildcard - Follows(s, _) -> getAllFollowsStmtPair()
// synonym & synonym - Follows(s1, s2) -> getAllFollowsTStmtPair()
// TODO(Anybody): Change return type to optimise
vector<vector<int>> FollowsEvaluator::evaluateStmtPairFollows(
    const Param& left, const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::WILDCARD) {
    results = pkb->getMappings(RelationshipType::FOLLOWS, ParamPosition::RIGHT);

  } else if (right.type == ParamType::WILDCARD) {
    results = pkb->getMappings(RelationshipType::FOLLOWS, ParamPosition::LEFT);

  } else {
    results = pkb->getMappings(RelationshipType::FOLLOWS, ParamPosition::BOTH);
  }

  auto formattedResults = vector<vector<int>>(results.begin(), results.end());
  return formattedResults;
}

bool FollowsEvaluator::evaluateBoolFollowsT(const Param& left,
                                            const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    return pkb->isRs(RelationshipType::FOLLOWS_T, stoi(left.value),
                     stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::BOTH))
                .empty();
  }

  if (leftType == ParamType::WILDCARD) {
    UNO_SET_OF_STMT_NO listOfStmts =
        pkb->getLeft(RelationshipType::FOLLOWS_T, stoi(right.value));
    return !listOfStmts.empty();
  }

  UNO_SET_OF_STMT_NO listOfStmts =
      pkb->getRight(RelationshipType::FOLLOWS_T, stoi(left.value));
  return !listOfStmts.empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO FollowsEvaluator::evaluateStmtFollowsT(const Param& left,
                                                          const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getRight(RelationshipType::FOLLOWS_T, stoi(left.value));
  }

  // rightType == ParamType::LITERAL
  return pkb->getLeft(RelationshipType::FOLLOWS_T, stoi(right.value));
}

// synonym & wildcard - FollowsT(s, _) -> getAllFollowsTStmtPair()
// synonym & synonym -> getAllFollowsTStmtPair()
vector<vector<int>> FollowsEvaluator::evaluateStmtPairFollowsT(
    const Param& left, const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    results =
        pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::BOTH);
  } else if (left.type == ParamType::SYNONYM) {
    results =
        pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::LEFT);
  } else {
    // rightType == ParamType::SYNONYM
    results =
        pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::RIGHT);
  }
  return vector<vector<int>>(results.begin(), results.end());
}
