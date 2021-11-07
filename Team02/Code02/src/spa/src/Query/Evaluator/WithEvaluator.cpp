#include "WithEvaluator.h"

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/Global.h"

using namespace std;
using namespace query;

WithEvaluator::WithEvaluator(PKB* pkb) { this->pkb = pkb; }

tuple<bool, vector<IntermediateQueryResult>, SynonymValuesTable>
WithEvaluator::evaluateAttributes(
    const Param& left, const Param& right, const SynonymMap& synonymMap,
    const vector<IntermediateQueryResult>& currentQueryResults) {
  this->newQueryResults = {};
  this->isClauseTrue = false;
  this->synonymMap = synonymMap;
  this->currentQueryResults = currentQueryResults;
  this->clauseSynonymValuesTable = {};

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

  return make_tuple(isClauseTrue, newQueryResults, clauseSynonymValuesTable);
}

// ATTRIBUTE_ProcName, ATTRIBUTE_VAR_NAME, NAME_LITERAL
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
      int leftProcIdx =
          getIndexOfNameAttrOfSynonym(results.at(leftValue), leftDesignEntity);
      int rightProcIdx = getIndexOfNameAttrOfSynonym(results.at(rightValue),
                                                     rightDesignEntity);

      if (leftProcIdx == rightProcIdx) {
        isClauseTrue = true;
        addClauseResultAndUpdateCount(results);
      }
    }
    return;
  }

  if (leftType == ParamType::ATTRIBUTE_VAR_NAME &&
      rightType == ParamType::ATTRIBUTE_VAR_NAME) {
    DesignEntity leftDesignEntity = synonymMap.at(leftValue);
    DesignEntity rightDesignEntity = synonymMap.at(rightValue);
    for (auto results : currentQueryResults) {
      string leftVarName = getNameAttrOfSynonym(results.at(leftValue),
                                                leftDesignEntity, leftType);
      string rightVarName = getNameAttrOfSynonym(results.at(rightValue),
                                                 rightDesignEntity, rightType);
      if (leftVarName == rightVarName) {
        isClauseTrue = true;
        addClauseResultAndUpdateCount(results);
      }
    }
    return;
  }

  if (leftType == ParamType::NAME_LITERAL &&
      rightType == ParamType::NAME_LITERAL) {
    if (leftValue == rightValue) {
      isClauseTrue = true;
      newQueryResults = currentQueryResults;
    }
    return;
  }

  /* Different Attribute Types --------------------------------------- */
  if ((leftType == ParamType::ATTRIBUTE_PROC_NAME &&
       rightType == ParamType::ATTRIBUTE_VAR_NAME) ||
      (leftType == ParamType::ATTRIBUTE_VAR_NAME &&
       rightType == ParamType::ATTRIBUTE_PROC_NAME)) {
    return evaluateProcNameAndVarName(leftValue, rightValue, leftType,
                                      rightType);
  }
  if ((leftType == ParamType::ATTRIBUTE_PROC_NAME &&
       rightType == ParamType::NAME_LITERAL) ||
      (leftType == ParamType::ATTRIBUTE_VAR_NAME &&
       rightType == ParamType::NAME_LITERAL)) {
    return evaluateNameAttrAndNameLiteral(leftValue, rightValue, leftType);
  }
  if ((leftType == ParamType::NAME_LITERAL &&
       rightType == ParamType::ATTRIBUTE_PROC_NAME) ||
      (leftType == ParamType::NAME_LITERAL &&
       rightType == ParamType::ATTRIBUTE_VAR_NAME)) {
    return evaluateNameAttrAndNameLiteral(rightValue, leftValue, rightType);
  }
}

// ATTRIBUTE_VALUE, ATTRIBUTE_STMT_NUM, INTEGER_LITERAL
void WithEvaluator::evaluateIntegerAttributes(const Param& left,
                                              const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;
  string leftValue = left.value;
  string rightValue = right.value;

  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    if (leftValue == rightValue) {
      isClauseTrue = true;
      newQueryResults = currentQueryResults;
    }
    return;
  }

  // same types - (syn, syn), (.value, .value). (.stmt#, .stmt#)
  // diff types - (syn, .stmt#), (.stmt#, syn)
  if (leftType == rightType ||
      (leftType == ParamType::SYNONYM &&
       rightType == ParamType::ATTRIBUTE_STMT_NUM) ||
      (leftType == ParamType::ATTRIBUTE_STMT_NUM &&
       rightType == ParamType::SYNONYM)) {
    return evaluateIndexes(leftValue, rightValue);
  }

  // any other combo of .value, .stmt#, int literal
  return evaluateNumbers(leftValue, rightValue, leftType, rightType);
}

void WithEvaluator::addClauseResultAndUpdateCount(
    const query::IntermediateQueryResult& queryResult) {
  newQueryResults.push_back(queryResult);
  for (auto synonymValuePair : queryResult) {
    string synonym = synonymValuePair.first;
    if (clauseSynonymValuesTable.find(synonym) ==
        clauseSynonymValuesTable.end()) {
      clauseSynonymValuesTable[synonym] = {};
    }
    clauseSynonymValuesTable[synonym].insert(synonymValuePair.second);
  }
}

// (.procName, .varName)
void WithEvaluator::evaluateProcNameAndVarName(string firstSyn,
                                               string secondSyn,
                                               ParamType firstParamType,
                                               ParamType secondParamType) {
  DesignEntity designEntOfSynWithProcName = synonymMap.at(firstSyn);
  DesignEntity designEntOfSynWithVarName = synonymMap.at(secondSyn);

  for (auto results : currentQueryResults) {
    int valueOfSynWithProcName = results.at(firstSyn);
    string procName = getNameAttrOfSynonym(
        valueOfSynWithProcName, designEntOfSynWithProcName, firstParamType);

    int valueOfSynWithVarName = results.at(secondSyn);
    string varName = getNameAttrOfSynonym(
        valueOfSynWithVarName, designEntOfSynWithVarName, secondParamType);

    if (procName == varName) {
      isClauseTrue = true;
      addClauseResultAndUpdateCount(results);
    }
  }
}

void WithEvaluator::evaluateNameAttrAndNameLiteral(string synWithNameAttr,
                                                   string nameLiteral,
                                                   ParamType paramTypeOfSyn) {
  DesignEntity designEntOfSyn = synonymMap.at(synWithNameAttr);

  for (auto results : currentQueryResults) {
    int valueOfSynWithNameAttr = results.at(synWithNameAttr);
    string nameOfSyn = getNameAttrOfSynonym(valueOfSynWithNameAttr,
                                            designEntOfSyn, paramTypeOfSyn);

    if (nameOfSyn == nameLiteral) {
      isClauseTrue = true;
      addClauseResultAndUpdateCount(results);
    }
  }
}

void WithEvaluator::evaluateIndexes(string firstSyn, string secondSyn) {
  for (auto results : currentQueryResults) {
    int firstIndex = results.at(firstSyn);
    int secondIndex = results.at(secondSyn);

    if (firstIndex == secondIndex) {
      isClauseTrue = true;
      addClauseResultAndUpdateCount(results);
    }
  }
}

// (value, syn/stmt), (value, int lit), (syn/stmt, int lit)
void WithEvaluator::evaluateNumbers(string firstValue, string secondValue,
                                    ParamType firstParamType,
                                    ParamType secondParamType) {
  for (auto results : currentQueryResults) {
    string firstNumber = getNumber(firstValue, firstParamType, results);
    string secondNumber = getNumber(secondValue, secondParamType, results);

    if (firstNumber == secondNumber) {
      isClauseTrue = true;
      addClauseResultAndUpdateCount(results);
    }
  }
}

string WithEvaluator::getNumber(string value, ParamType paramType,
                                const IntermediateQueryResult& results) {
  if (paramType == ParamType::ATTRIBUTE_VALUE) {
    return pkb->getElementAt(TableType::CONST_TABLE, results.at(value));
  } else if (paramType == ParamType::SYNONYM ||
             paramType == ParamType::ATTRIBUTE_STMT_NUM) {
    return to_string(results.at(value));
  } else {
    return value;
  }
}

int WithEvaluator::getIndexOfNameAttrOfSynonym(
    int valueOfSynonym, DesignEntity designEntityOfSynonym) {
  int idx;
  switch (designEntityOfSynonym) {
    case DesignEntity::PROCEDURE:
      idx = valueOfSynonym;
      break;
    case DesignEntity::CALL:
      idx = *(pkb->getRight(RelationshipType::CALLS_S, valueOfSynonym).begin());
      break;
    case DesignEntity::VARIABLE:
      idx = valueOfSynonym;
      break;
    case DesignEntity::READ:
      idx = *(pkb->getRight(RelationshipType::MODIFIES_S, valueOfSynonym))
                 .begin();
      break;
    case DesignEntity::PRINT:
      idx = *(pkb->getRight(RelationshipType::USES_S, valueOfSynonym)).begin();
      break;
    default:
      DMOprintErrMsgAndExit(
          "[WithEvaluator][getIndexOfProcName] invalid design entity of "
          "synonym.procName or synonym.varName");
      break;
  }
  return idx;
}

string WithEvaluator::getNameAttrOfSynonym(int valueOfSynonym,
                                           DesignEntity designEntity,
                                           ParamType paramType) {
  int idx = getIndexOfNameAttrOfSynonym(valueOfSynonym, designEntity);
  if (paramType == ParamType::ATTRIBUTE_PROC_NAME) {
    return pkb->getElementAt(TableType::PROC_TABLE, idx);
  } else {
    return pkb->getElementAt(TableType::VAR_TABLE, idx);
  }
}
