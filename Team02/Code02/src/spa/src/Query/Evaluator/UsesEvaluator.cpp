#include "UsesEvaluator.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "Common/Global.h"

using namespace std;
using namespace query;

UsesEvaluator::UsesEvaluator(PKB* pkb) { this->pkb = pkb; }

bool UsesEvaluator::evaluateBoolUsesS(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::WILDCARD || leftType == ParamType::NAME_LITERAL) {
    DMOprintErrMsgAndExit(
        "[UsesEvaluator][evaluateBoolUsesS] wildcard or string in left param");
  }

  // if both literal - UsesS(1, "x")
  // if one literal + underscore - UsesS(2, _)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    return pkb->isUsesS(stoi(left.value), right.value);
  }

  // literal + wildcard - UsesS(1, _)
  return !pkb->getVarsUsedS(stoi(left.value)).empty();
}

// synonym & literal
UNO_SET_OF_STMT_NO UsesEvaluator::evaluateUsesS(const Param& left,
                                                const Param& right) {
  // if one literal + synonym - UsesS(s, "x"), UsesS(1, v)
  ParamType leftType = left.type;

  if (leftType == ParamType::SYNONYM) {
    return pkb->getUsesS(right.value);
  }

  // rightType == ParamType::SYNONYM
  return pkb->getVarsUsedS(stoi(left.value));
}

// synonym & wildcard - UsesS(s, _) -> getAllUsesSStmtPair()
// synonym & synonym - UsesS(s, v) -> getAllUsesSStmtPair()
vector<pair<int, vector<int>>> UsesEvaluator::evaluatePairUsesS(
    const Param& left, const Param& right) {
  return pkb->getAllUsesSPairs();
}

bool UsesEvaluator::evaluateBoolUsesP(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::WILDCARD ||
      leftType == ParamType::INTEGER_LITERAL) {
    DMOprintErrMsgAndExit(
        "[UsesEvaluator][evaluateBoolUsesP] wildcard or integer in left param");
  }

  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    return pkb->isUsesP(left.value, right.value);
  }

  return !pkb->getVarsUsedP(left.value).empty();
}

UNO_SET_OF_STMT_NO UsesEvaluator::evaluateUsesP(const Param& left,
                                                const Param& right) {
  ParamType leftType = left.type;

  if (leftType == ParamType::SYNONYM) {
    return pkb->getUsesP(right.value);
  }

  return pkb->getVarsUsedP(left.value);
}

vector<pair<int, vector<int>>> UsesEvaluator::evaluatePairUsesP(
    const Param& left, const Param& right) {
  return pkb->getAllUsesPPairs();
}
