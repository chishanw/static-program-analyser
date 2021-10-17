#include "WithEvaluator.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/Global.h"

using namespace std;
using namespace query;

WithEvaluator::WithEvaluator(PKB* pkb) {
  this->pkb = pkb;
  this->newQueryResults = {};
}

pair<bool, vector<QueryResult>> WithEvaluator::evaluateAttributes(
    const Param& left, const Param& right,
    const unordered_map<string, DesignEntity>& synonymMap,
    const vector<QueryResult>& currentQueryResults) {
  this->isClauseTrue = false;
  this->synonymMap = synonymMap;
  this->currentQueryResults = currentQueryResults;

  unordered_set<ParamType> nameParamTypes = {ParamType::ATTRIBUTE_PROC_NAME,
                                             ParamType::ATTRIBUTE_VAR_NAME,
                                             ParamType::NAME_LITERAL};
  unordered_set<ParamType> integerParamTypes = {
      ParamType::ATTRIBUTE_VALUE, ParamType::ATTRIBUTE_STMT_NUM,
      ParamType::INTEGER_LITERAL, ParamType::SYNONYM};

  if ((nameParamTypes.find(left.type) != nameParamTypes.end()) ||
      (nameParamTypes.find(right.type) != nameParamTypes.end())) {
    evaluateNameAttributes(left, right);
  }
  if ((integerParamTypes.find(left.type) != integerParamTypes.end()) ||
      (integerParamTypes.find(right.type) != integerParamTypes.end())) {
    evaluateIntegerAttributes(left, right);
  }

  bool areBothNameLiterals = left.type == ParamType::NAME_LITERAL &&
                             right.type == ParamType::NAME_LITERAL;
  bool areBothIntegerLiterals = left.type == ParamType::INTEGER_LITERAL &&
                                right.type == ParamType::INTEGER_LITERAL;
  if (!areBothNameLiterals && !areBothIntegerLiterals) {
    isClauseTrue = isClauseTrue && !newQueryResults.empty();
  }

  return make_pair(isClauseTrue, newQueryResults);
}

// ATTRIBUTE_PROC_NAME, ATTRIBUTE_VAR_NAME, NAME_LITERAL
void WithEvaluator::evaluateNameAttributes(const Param& left,
                                           const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;
  string leftValue = left.value;
  string rightValue = right.value;

  /* Same Attribute Types ------------------------------------------- */
  if (leftType == ParamType::ATTRIBUTE_PROC_NAME &&
      rightType == ParamType::ATTRIBUTE_PROC_NAME) {
    DesignEntity leftDesignEntity = synonymMap.at(leftValue);
    DesignEntity rightDesignEntity = synonymMap.at(rightValue);
    for (auto results : currentQueryResults) {
      int leftProcIdx = getIndexOfProcNameAttrOfSynonym(results.at(leftValue),
                                                        leftDesignEntity);
      int rightProcIdx = getIndexOfProcNameAttrOfSynonym(results.at(rightValue),
                                                         rightDesignEntity);

      if (leftProcIdx == rightProcIdx) {
        isClauseTrue = true;
        newQueryResults.push_back(results);
      }
    }
  }
  if (leftType == ParamType::ATTRIBUTE_VAR_NAME &&
      rightType == ParamType::ATTRIBUTE_VAR_NAME) {
    DesignEntity leftDesignEntity = synonymMap.at(leftValue);
    DesignEntity rightDesignEntity = synonymMap.at(rightValue);
    for (auto results : currentQueryResults) {
      string leftVarName =
          getVarNameAttrOfSynonym(results.at(leftValue), leftDesignEntity);
      string rightVarName =
          getVarNameAttrOfSynonym(results.at(rightValue), rightDesignEntity);
      if (leftVarName == rightVarName) {
        isClauseTrue = true;
        newQueryResults.push_back(results);
      }
    }
  }
  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    if (leftValue == rightValue) {
      isClauseTrue = true;
      newQueryResults = currentQueryResults;
    }
  }

  /* Different Attribute Types --------------------------------------- */
  if (leftType == ParamType::ATTRIBUTE_PROC_NAME &&
      rightType == ParamType::ATTRIBUTE_VAR_NAME) {
    evaluateProcNameAndVarName(leftValue, rightValue);
  }
  if (leftType == ParamType::ATTRIBUTE_VAR_NAME &&
      rightType == ParamType::ATTRIBUTE_PROC_NAME) {
    evaluateProcNameAndVarName(rightValue, leftValue);
  }
  if (leftType == ParamType::ATTRIBUTE_PROC_NAME &&
      rightType == ParamType::NAME_LITERAL) {
    evaluateProcNameAndNameLiteral(leftValue, rightValue);
  }
  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::ATTRIBUTE_PROC_NAME) {
    evaluateProcNameAndNameLiteral(rightValue, leftValue);
  }
  if (leftType == ParamType::ATTRIBUTE_VAR_NAME &&
      rightType == ParamType::NAME_LITERAL) {
    evaluateVarNameAndNameLiteral(leftValue, rightValue);
  }
  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::ATTRIBUTE_VAR_NAME) {
    evaluateVarNameAndNameLiteral(rightValue, leftValue);
  }
}

// ATTRIBUTE_VALUE, ATTRIBUTE_STMT_NUM, INTEGER_LITERAL
void WithEvaluator::evaluateIntegerAttributes(const Param& left,
                                              const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;
  string leftValue = left.value;
  string rightValue = right.value;

  /* Same Attribute Types ------------------------------------------- */
  if (leftType == ParamType::SYNONYM && rightType == ParamType::SYNONYM) {
    // compare prog_line synonyms
    evaluateIndexes(leftValue, rightValue);
  }
  if (leftType == ParamType::ATTRIBUTE_VALUE &&
      rightType == ParamType::ATTRIBUTE_VALUE) {
    // compare constant indexes
    evaluateIndexes(leftValue, rightValue);
  }
  if (leftType == ParamType::ATTRIBUTE_STMT_NUM &&
      rightType == ParamType::ATTRIBUTE_STMT_NUM) {
    // compare stmt numbers
    evaluateIndexes(leftValue, rightValue);
  }
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    if (leftValue == rightValue) {
      isClauseTrue = true;
      newQueryResults = currentQueryResults;
    }
  }

  /* Different Attribute Types --------------------------------------- */
  if ((leftType == ParamType::SYNONYM &&
       rightType == ParamType::ATTRIBUTE_STMT_NUM) ||
      (leftType == ParamType::ATTRIBUTE_STMT_NUM &&
       rightType == ParamType::SYNONYM)) {
    evaluateIndexes(leftValue, rightValue);
  }
  if (leftType == ParamType::SYNONYM &&
      rightType == ParamType::ATTRIBUTE_VALUE) {
    evaluateValueAndStmt(rightValue, leftValue);
  }
  if (leftType == ParamType::ATTRIBUTE_VALUE &&
      rightType == ParamType::SYNONYM) {
    evaluateValueAndStmt(leftValue, rightValue);
  }
  if (leftType == ParamType::SYNONYM &&
      rightType == ParamType::INTEGER_LITERAL) {
    evaluateStmtAndIntegerLiteral(leftValue, rightValue);
  }
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::SYNONYM) {
    evaluateStmtAndIntegerLiteral(rightValue, leftValue);
  }
  if (leftType == ParamType::ATTRIBUTE_VALUE &&
      rightType == ParamType::ATTRIBUTE_STMT_NUM) {
    evaluateValueAndStmt(leftValue, rightValue);
  }
  if (leftType == ParamType::ATTRIBUTE_STMT_NUM &&
      rightType == ParamType::ATTRIBUTE_VALUE) {
    evaluateValueAndStmt(rightValue, leftValue);
  }
  if (leftType == ParamType::ATTRIBUTE_VALUE &&
      rightType == ParamType::INTEGER_LITERAL) {
    evaluateValueAndIntegerLiteral(leftValue, rightValue);
  }
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::ATTRIBUTE_VALUE) {
    evaluateValueAndIntegerLiteral(rightValue, leftValue);
  }
  if (leftType == ParamType::ATTRIBUTE_STMT_NUM &&
      rightType == ParamType::INTEGER_LITERAL) {
    evaluateStmtAndIntegerLiteral(leftValue, rightValue);
  }
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::ATTRIBUTE_STMT_NUM) {
    evaluateStmtAndIntegerLiteral(rightValue, leftValue);
  }
}

void WithEvaluator::evaluateProcNameAndVarName(string synWithProcName,
                                               string synWithVarName) {
  DesignEntity designEntOfSynWithProcName = synonymMap.at(synWithProcName);
  DesignEntity designEntOfSynWithVarName = synonymMap.at(synWithVarName);

  for (auto results : currentQueryResults) {
    int valueOfSynWithProcName = results.at(synWithProcName);
    string procName = getProcNameAttrOfSynonym(valueOfSynWithProcName,
                                               designEntOfSynWithProcName);

    int valueOfSynWithVarName = results.at(synWithVarName);
    string varName = getVarNameAttrOfSynonym(valueOfSynWithVarName,
                                             designEntOfSynWithVarName);

    if (procName == varName) {
      isClauseTrue = true;
      newQueryResults.push_back(results);
    }
  }
}

void WithEvaluator::evaluateProcNameAndNameLiteral(string synWithProcName,
                                                   string nameLiteral) {
  DesignEntity designEntOfSynWithProcName = synonymMap.at(synWithProcName);

  for (auto results : currentQueryResults) {
    int valueOfSynWithProcName = results.at(synWithProcName);
    string procName = getProcNameAttrOfSynonym(valueOfSynWithProcName,
                                               designEntOfSynWithProcName);

    if (procName == nameLiteral) {
      isClauseTrue = true;
      newQueryResults.push_back(results);
    }
  }
}

void WithEvaluator::evaluateVarNameAndNameLiteral(string synWithVarName,
                                                  string nameLiteral) {
  DesignEntity designEntOfSynWithVarName = synonymMap.at(synWithVarName);

  for (auto results : currentQueryResults) {
    int valueOfSynWithVarName = results.at(synWithVarName);
    string varName = getVarNameAttrOfSynonym(valueOfSynWithVarName,
                                             designEntOfSynWithVarName);

    if (varName == nameLiteral) {
      isClauseTrue = true;
      newQueryResults.push_back(results);
    }
  }
}

void WithEvaluator::evaluateIndexes(string firstSyn, string secondSyn) {
  for (auto results : currentQueryResults) {
    int firstIndex = results.at(firstSyn);
    int secondIndex = results.at(secondSyn);

    if (firstIndex == secondIndex) {
      isClauseTrue = true;
      newQueryResults.push_back(results);
    }
  }
}

void WithEvaluator::evaluateValueAndStmt(string constSyn, string stmtSyn) {
  for (auto results : currentQueryResults) {
    string constValue = pkb->getConst(results.at(constSyn));
    string stmtNum = to_string(results.at(stmtSyn));

    if (constValue == stmtNum) {
      isClauseTrue = true;
      newQueryResults.push_back(results);
    }
  }
}

void WithEvaluator::evaluateValueAndIntegerLiteral(string constSyn,
                                                   string integerLiteral) {
  for (auto results : currentQueryResults) {
    string constValue = pkb->getConst(results.at(constSyn));

    if (constValue == integerLiteral) {
      isClauseTrue = true;
      newQueryResults.push_back(results);
    }
  }
}

void WithEvaluator::evaluateStmtAndIntegerLiteral(string stmtSyn,
                                                  string integerLiteral) {
  for (auto results : currentQueryResults) {
    string stmtNum = to_string(results.at(stmtSyn));

    if (stmtNum == integerLiteral) {
      isClauseTrue = true;
      newQueryResults.push_back(results);
    }
  }
}

int WithEvaluator::getIndexOfProcNameAttrOfSynonym(
    int valueOfSynonym, DesignEntity designEntityOfSynonym) {
  int procIdx;
  switch (designEntityOfSynonym) {
    case DesignEntity::PROCEDURE:
      procIdx = valueOfSynonym;
      break;
    case DesignEntity::CALL:
      procIdx = pkb->getProcCalledByCallStmt(valueOfSynonym);
      break;
    default:
      DMOprintErrMsgAndExit(
          "[WithEvaluator][getIndexOfProcName] invalid design entity of "
          "synonym.procName");
      break;
  }
  return procIdx;
}

int WithEvaluator::getIndexOfVarNameAttrOfSynonym(
    int valueOfSynonym, DesignEntity designEntityOfSynonym) {
  int varIdx;
  switch (designEntityOfSynonym) {
    case DesignEntity::VARIABLE:
      varIdx = valueOfSynonym;
      break;
    case DesignEntity::READ:
      varIdx = *(pkb->getVarsModifiedS(valueOfSynonym)).begin();
      break;
    case DesignEntity::PRINT:
      varIdx = *(pkb->getVarsUsedS(valueOfSynonym)).begin();
      break;
    default:
      DMOprintErrMsgAndExit(
          "[WithEvaluator][getIndexOfVarName] invalid design entity of "
          "synonym.varName");
      break;
  }
  return varIdx;
}

string WithEvaluator::getProcNameAttrOfSynonym(
    int valueOfSynonym, DesignEntity designEntityOfSynonym) {
  int procIdx =
      getIndexOfProcNameAttrOfSynonym(valueOfSynonym, designEntityOfSynonym);
  return pkb->getProcName(procIdx);
}

string WithEvaluator::getVarNameAttrOfSynonym(
    int valueOfSynonym, DesignEntity designEntityOfSynonym) {
  int varIdx =
      getIndexOfVarNameAttrOfSynonym(valueOfSynonym, designEntityOfSynonym);
  return pkb->getVarName(varIdx);
}
