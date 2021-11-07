#pragma once

#include <Common/Common.h>
#include <PKB/PKB.h>
#include <Query/Common.h>

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

class WithEvaluator {
 public:
  explicit WithEvaluator(PKB*);

  std::tuple<bool, std::vector<query::IntermediateQueryResult>,
             query::SynonymValuesTable>
  evaluateAttributes(
      const query::Param& left, const query::Param& right,
      const query::SynonymMap& synonymMap,
      const std::vector<query::IntermediateQueryResult>& currentQueryResults);

 private:
  PKB* pkb;
  std::vector<query::IntermediateQueryResult> newQueryResults;
  query::SynonymMap synonymMap;
  std::vector<query::IntermediateQueryResult> currentQueryResults;
  bool isClauseTrue;
  query::SynonymValuesTable clauseSynonymValuesTable;

  void evaluateNameAttributes(const query::Param& left,
                              const query::Param& right);
  void evaluateIntegerAttributes(const query::Param& left,
                                 const query::Param& right);
  void addClauseResultAndUpdateCount(
      const query::IntermediateQueryResult& queryResult);

  void evaluateProcNameAndVarName(std::string firstSyn, std::string secondSyn,
                                  query::ParamType firstParamType,
                                  query::ParamType secondParamType);
  void evaluateNameAttrAndNameLiteral(std::string synWithProcName,
                                      std::string nameLiteral,
                                      query::ParamType paramTypeOfSyn);
  void evaluateIndexes(std::string firstSyn, std::string secondSyn);
  void evaluateNumbers(std::string firstValue, std::string secondValue,
                       query::ParamType firstParamType,
                       query::ParamType secondParamType);
  std::string getNumber(std::string value, query::ParamType paramType,
                        const query::IntermediateQueryResult& results);
  int getIndexOfNameAttrOfSynonym(int valueOfSynonym,
                                  DesignEntity designEntity);
  std::string getNameAttrOfSynonym(int valueOfSynonym,
                                   DesignEntity designEntityOfSynonym,
                                   query::ParamType paramType);
};
