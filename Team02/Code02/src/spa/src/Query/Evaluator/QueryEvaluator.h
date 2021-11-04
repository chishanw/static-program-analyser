#pragma once

#include <Common/Common.h>
#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/AffectsEvaluator.h>
#include <Query/Evaluator/CallsEvaluator.h>
#include <Query/Evaluator/FollowsEvaluator.h>
#include <Query/Evaluator/ModifiesEvaluator.h>
#include <Query/Evaluator/NextEvaluator.h>
#include <Query/Evaluator/ParentEvaluator.h>
#include <Query/Evaluator/PatternEvaluator.h>
#include <Query/Evaluator/UsesEvaluator.h>
#include <Query/Evaluator/WithEvaluator.h>
#include <Query/Optimizer/QueryOptimizer.h>

#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class QueryEvaluator {
 public:
  explicit QueryEvaluator(PKB* pkb, QueryOptimizer* optimizer);
  query::FinalQueryResults evaluateQuery(
      std::unordered_map<std::string, DesignEntity> synonymMap,
      query::SelectClause select);
  query::SynonymCountsTable getSynonymCounts();

 private:
  PKB* pkb;
  QueryOptimizer* optimizer;
  std::unordered_map<std::string, DesignEntity> synonymMap;
  FollowsEvaluator followsEvaluator;
  ParentEvaluator parentEvaluator;
  UsesEvaluator usesEvaluator;
  ModifiesEvaluator modifiesEvaluator;
  CallsEvaluator callsEvaluator;
  NextEvaluator nextEvaluator;
  AffectsEvaluator affectsEvaluator;
  PatternEvaluator patternEvaluator;
  WithEvaluator withEvaluator;

  bool areAllClausesTrue;
  std::vector<query::IntermediateQueryResult> finalQueryResults;
  std::vector<query::IntermediateQueryResult> groupQueryResults;
  std::unordered_set<std::string> queryResultsSynonyms;
  query::SynonymValuesTable clauseSynonymValuesTable;

  // methods to build queryResults
  void filterAndAddIncomingResults(query::ClauseIncomingResults incomingResults,
                                   const query::Param& left,
                                   const query::Param& right);
  query::ClauseIncomingResults filterIncomingResults(
      query::ClauseIncomingResults incomingResults, const query::Param& left,
      const query::Param& right);
  void initializeQueryResults(query::ClauseIncomingResults incomingResults,
                              const query::Param& left,
                              const query::Param& right);
  void addIncomingResults(query::ClauseIncomingResults incomingResults,
                          const query::Param& left, const query::Param& right);

  // main algos to merge results
  void filter(query::ClauseIncomingResults incomingResults,
              std::vector<std::string> incomingResultsSynonyms);
  void innerJoin(query::ClauseIncomingResults incomingResults,
                 std::vector<std::string> incomingResultsSynonyms);
  void crossProduct(query::ClauseIncomingResults incomingResults,
                    std::vector<std::string> incomingResultsSynonyms);
  // helpers for above main algos
  void filterValidQueryResults(
      std::vector<query::IntermediateQueryResult>* newQueryResults,
      query::IntermediateQueryResult queryResult,
      std::vector<int> incomingResult,
      std::vector<std::string> incomingResultsSynonyms);
  void innerJoinValidQueryResults(
      std::vector<query::IntermediateQueryResult>* newQueryResults,
      query::IntermediateQueryResult queryResult,
      std::vector<int> incomingResult,
      std::vector<std::string> incomingResultsSynonyms);

  // methods to call the relevant sub evaluator
  void evaluateSuchThatClause(query::SuchThatClause);
  bool callSubEvaluatorBool(RelationshipType relationshipType,
                            const query::Param& left,
                            const query::Param& right);
  query::ClauseIncomingResults callSubEvaluatorRef(
      RelationshipType relationshipType, const query::Param& left,
      const query::Param& right);
  query::ClauseIncomingResults callSubEvaluatorPair(
      RelationshipType relationshipType, const query::Param& left,
      const query::Param& right);

  void evaluatePatternClause(query::PatternClause);
  query::ClauseIncomingResults callPatternSubEvaluatorRef(
      DesignEntity designEntity, const query::Param& varParam,
      const query::PatternExpr& patternExpr);
  query::ClauseIncomingResults callPatternSubEvaluatorPair(
      DesignEntity designEntity, const query::Param& varParam,
      const query::PatternExpr& patternExpr);

  void evaluateWithClause(query::WithClause);

  // helpers for query optimization
  void insertClauseSynonymValue(query::IntermediateQueryResult queryResult);
  void updateQuerySynonymCounts();
  void filterGroupResultsBySelectSynonyms(
      const std::vector<query::Synonym>& selectedSynonyms);
  void filterQuerySynonymsBySelectSynonyms(
      const std::vector<query::Synonym>& selectedSynonyms);
  void mergeGroupResultsIntoFinalResults();

  // helpers for evaluating based on prev clauses
  std::vector<std::tuple<query::Param, query::Param, ParamPosition>>
  getParamsWithPrevResults(const query::Param& left, const query::Param& right);
  void addLeftParamFromPrevResults(
      const query::Param& left, const query::Param& right,
      std::vector<std::tuple<query::Param, query::Param, ParamPosition>>&
          newParams);
  void addRightParamFromPrevResults(
      const query::Param& left, const query::Param& right,
      std::vector<std::tuple<query::Param, query::Param, ParamPosition>>&
          newParams);
  bool getIsBoolClause(RelationshipType relationshipType,
                       std::pair<query::ParamType, query::ParamType>);
  std::pair<query::ParamType, std::string> getParamTypeAndLiteralFromIndex(
      std::string synonymName, int index);
  int getIndexFromLiteral(std::string synonymName, std::string literal);

  // miscellaneous helpers
  query::ClauseIncomingResults formatRefResults(
      std::unordered_set<int> results);
  std::unordered_set<int> getAllValuesOfSynonym(std::string synonymName);
  query::FinalQueryResults getSelectSynonymFinalResults(
      query::SelectClause selectClause);
  bool checkIsCorrectDesignEntity(int stmtNum, DesignEntity designEntity);
};
