#include "ModifiesEvaluator.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "Common/Global.h"

using namespace std;
using namespace query;

ModifiesEvaluator::ModifiesEvaluator(PKB* pkb) { this->pkb = pkb; }

bool ModifiesEvaluator::evaluateBoolModifiesS(const Param& left,
                                              const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::WILDCARD) {
    DMOprintErrMsgAndExit(
        "[ModifiesEvaluator][evaluateBoolModifiesS] wildcard in left param");
  }

  // if both literal - ModifiesS(1, "x")
  // if one literal + underscore - ModifiesS(2, _)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    return pkb->isRs(RelationshipType::MODIFIES_S, stoi(left.value),
                     TableType::VAR_TABLE, right.value);
  }

  // literal + wildcard - ModifiesS(1, _)
  return !pkb->getRight(RelationshipType::MODIFIES_S, stoi(left.value)).empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO ModifiesEvaluator::evaluateModifiesS(const Param& left,
                                                        const Param& right) {
  // if one literal + synonym - ModifiesS(s, "x"), ModifiesS(1, v)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::SYNONYM) {
    return pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                        right.value);
  }

  // rightType == ParamType::SYNONYM
  return pkb->getRight(RelationshipType::MODIFIES_S, stoi(left.value));
}

// synonym & wildcard - ModifiesS(s, _) -> getAllModifiesSStmtPair()
// synonym & synonym - ModifiesS(s, v) -> getAllModifiesSStmtPair()
vector<vector<int>> ModifiesEvaluator::evaluatePairModifiesS(
    const Param& left, const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    results =
        pkb->getMappings(RelationshipType::MODIFIES_S, ParamPosition::BOTH);
  } else if (left.type == ParamType::SYNONYM) {
    results =
        pkb->getMappings(RelationshipType::MODIFIES_S, ParamPosition::LEFT);
  } else {
    results =
        pkb->getMappings(RelationshipType::MODIFIES_S, ParamPosition::RIGHT);
  }
  return vector<vector<int>>(results.begin(), results.end());
}

bool ModifiesEvaluator::evaluateBoolModifiesP(const Param& left,
                                              const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::WILDCARD) {
    DMOprintErrMsgAndExit(
        "[ModifiesEvaluator][evaluateBoolModifiesP] wildcard in left param");
  }

  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    return pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                     left.value, TableType::VAR_TABLE, right.value);
  }

  return !pkb->getRight(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                        left.value)
              .empty();
}

UNO_SET_OF_STMT_NO ModifiesEvaluator::evaluateModifiesP(const Param& left,
                                                        const Param& right) {
  ParamType leftType = left.type;

  if (leftType == ParamType::SYNONYM) {
    return pkb->getLeft(RelationshipType::MODIFIES_P, TableType::VAR_TABLE,
                        right.value);
  }

  return pkb->getRight(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                       left.value);
}

vector<vector<int>> ModifiesEvaluator::evaluatePairModifiesP(
    const Param& left, const Param& right) {
  unordered_set<vector<int>, VectorHash> results;
  if (left.type == ParamType::SYNONYM && right.type == ParamType::SYNONYM) {
    results =
        pkb->getMappings(RelationshipType::MODIFIES_P, ParamPosition::BOTH);
  } else if (left.type == ParamType::SYNONYM) {
    results =
        pkb->getMappings(RelationshipType::MODIFIES_P, ParamPosition::LEFT);
  } else {
    results =
        pkb->getMappings(RelationshipType::MODIFIES_P, ParamPosition::RIGHT);
  }
  return vector<vector<int>>(results.begin(), results.end());
}
