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
      const std::unordered_map<std::string, DesignEntity>& synonymMap,
      const std::vector<query::IntermediateQueryResult>& currentQueryResults);

 private:
  PKB* pkb;
  std::vector<query::IntermediateQueryResult> newQueryResults;
  std::unordered_map<std::string, DesignEntity> synonymMap;
  std::vector<query::IntermediateQueryResult> currentQueryResults;
  bool isClauseTrue;
  query::SynonymValuesTable clauseSynonymValuesTable;

  void evaluateNameAttributes(const query::Param& left,
                              const query::Param& right);
  void evaluateIntegerAttributes(const query::Param& left,
                                 const query::Param& right);
  void addClauseResultAndUpdateCount(
      const query::IntermediateQueryResult& queryResult);

  void evaluateProcNameAndVarName(std::string synWithProcName,
                                  std::string synWithVarName);
  void evaluateProcNameAndNameLiteral(std::string synWithProcName,
                                      std::string nameLiteral);
  void evaluateVarNameAndNameLiteral(std::string synWithVarName,
                                     std::string nameLiteral);
  void evaluateIndexes(std::string firstSyn, std::string secondSyn);
  void evaluateValueAndStmt(std::string constSyn, std::string stmtSyn);
  void evaluateValueAndIntegerLiteral(std::string constSyn,
                                      std::string integerLiteral);
  void evaluateStmtAndIntegerLiteral(std::string stmtSyn,
                                     std::string integerLiteral);

  int getIndexOfProcNameAttrOfSynonym(int valueOfSynonym,
                                      DesignEntity designEntityOfSynonym);
  int getIndexOfVarNameAttrOfSynonym(int valueOfSynonym,
                                     DesignEntity designEntityOfSynonym);
  std::string getProcNameAttrOfSynonym(int valueOfSynonym,
                                       DesignEntity designEntityOfSynonym);
  std::string getVarNameAttrOfSynonym(int valueOfSynonym,
                                      DesignEntity designEntityOfSynonym);
};
