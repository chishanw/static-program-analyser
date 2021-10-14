#include "ModifiesEvaluator.h"

#include <stdio.h>

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
    return pkb->isModifiesS(stoi(left.value), right.value);
  }

  // literal + wildcard - ModifiesS(1, _)
  return !pkb->getVarsModifiedS(stoi(left.value)).empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO ModifiesEvaluator::evaluateModifiesS(const Param& left,
                                                        const Param& right) {
  // if one literal + synonym - ModifiesS(s, "x"), ModifiesS(1, v)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::SYNONYM) {
    return pkb->getModifiesS(right.value);
  }

  // rightType == ParamType::SYNONYM
  return pkb->getVarsModifiedS(stoi(left.value));
}

// synonym & wildcard - ModifiesS(s, _) -> getAllModifiesSStmtPair()
// synonym & synonym - ModifiesS(s, v) -> getAllModifiesSStmtPair()
vector<pair<int, vector<int>>> ModifiesEvaluator::evaluatePairModifiesS(
    const Param& left, const Param& right) {
  return pkb->getAllModifiesSPairs();
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
    return pkb->isModifiesP(left.value, right.value);
  }

  return !pkb->getVarsModifiedP(left.value).empty();
}

UNO_SET_OF_STMT_NO ModifiesEvaluator::evaluateModifiesP(const Param& left,
                                                        const Param& right) {
  ParamType leftType = left.type;

  if (leftType == ParamType::SYNONYM) {
    return pkb->getModifiesP(right.value);
  }

  return pkb->getVarsModifiedP(left.value);
}

vector<pair<int, vector<int>>> ModifiesEvaluator::evaluatePairModifiesP(
    const Param& left, const Param& right) {
  return pkb->getAllModifiesPPairs();
}
