#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class WithEvaluator {
 public:
  explicit WithEvaluator(PKB*);

  std::pair<bool, std::vector<query::QueryResult>> evaluateAttributes(
      const query::Param& left, const query::Param& right,
      const std::unordered_map<std::string, query::DesignEntity>& synonymMap,
      const std::vector<query::QueryResult>& currentQueryResults);

 private:
  PKB* pkb;
  std::vector<query::QueryResult> newQueryResults;
  std::unordered_map<std::string, query::DesignEntity> synonymMap;
  std::vector<query::QueryResult> currentQueryResults;
  bool isClauseTrue;

  void evaluateNameAttributes(const query::Param& left,
                              const query::Param& right);
  void evaluateIntegerAttributes(const query::Param& left,
                                 const query::Param& right);

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

  int getIndexOfProcNameAttrOfSynonym(
      int valueOfSynonym, query::DesignEntity designEntityOfSynonym);
  int getIndexOfVarNameAttrOfSynonym(int valueOfSynonym,
                                     query::DesignEntity designEntityOfSynonym);
  std::string getProcNameAttrOfSynonym(
      int valueOfSynonym, query::DesignEntity designEntityOfSynonym);
  std::string getVarNameAttrOfSynonym(
      int valueOfSynonym, query::DesignEntity designEntityOfSynonym);
};
