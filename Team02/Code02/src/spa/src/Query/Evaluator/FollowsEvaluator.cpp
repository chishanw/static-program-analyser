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
    STMT_NO s2 = pkb->getFollows(stoi(left.value));
    return s2 == stoi(right.value);
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getAllFollowsStmtPairs()).empty();
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

  if (leftType == ParamType::INTEGER_LITERAL) {
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
    vector<LIST_STMT_NO> results = pkb->getAllFollowsStmtPairs();
    vector<vector<int>> formattedResults = {};
    for (LIST_STMT_NO result : results) {
      formattedResults.push_back({result.back()});
    }
    return formattedResults;
  }
  if (right.type == ParamType::WILDCARD) {
    vector<LIST_STMT_NO> results = pkb->getAllFollowsStmtPairs();
    vector<vector<int>> formattedResults = {};
    for (LIST_STMT_NO result : results) {
      formattedResults.push_back({result.front()});
    }
    return formattedResults;
  }
  return pkb->getAllFollowsStmtPairs();
}

bool FollowsEvaluator::evaluateBoolFollowsT(const Param& left,
                                            const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    return pkb->isFollowsT(stoi(left.value), stoi(right.value));
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !(pkb->getAllFollowsTStmtPairs()).empty();
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

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getFollowsT(stoi(left.value));
  }

  // rightType == ParamType::LITERAL
  return pkb->getFollowedTBy(stoi(right.value));
}

// synonym & wildcard - FollowsT(s, _) -> getAllFollowsTStmtPair()
// synonym & synonym -> getAllFollowsTStmtPair()
vector<pair<int, vector<int>>> FollowsEvaluator::evaluateStmtPairFollowsT(
    const Param& left, const Param& right) {
  return pkb->getAllFollowsTStmtPairs();
}
