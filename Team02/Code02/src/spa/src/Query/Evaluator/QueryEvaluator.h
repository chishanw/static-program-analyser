#pragma once

#include <Common/Common.h>
#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/AffectsEvaluator.h>
#include <Query/Evaluator/NextEvaluator.h>
#include <Query/Evaluator/PatternEvaluator.h>
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

  // methods to process different types of clauses
  void evaluateSuchThatClause(query::SuchThatClause);
  void evaluateSuchThatClauseHelper(query::SuchThatClause);
  void evaluateSuchThatOnDemandClause(query::SuchThatClause);
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

  // helpers for evaluating based on prev clauses - non on demand rs
  std::vector<std::vector<int>> resolveBothParamsFromResultTable(
      query::SuchThatClause clause);
  std::unordered_set<int> resolveLeftParam(query::SuchThatClause clause);
  std::vector<std::vector<int>> resolveRightParamFromLeftValues(
      query::SuchThatClause clause, std::unordered_set<int> leftValues);
  int convertLeftNameLiteralToInt(RelationshipType rsType,
                                  std::string nameLiteral);
  int convertRightNameLiteralToInt(RelationshipType rsType,
                                   std::string nameLiteral);

  // helpers for evaluating based on prev clauses - on demand rs
  std::vector<std::tuple<query::Param, query::Param, ParamPosition>>
  getResolvedParamsForOnDemandRs(const query::Param& left,
                                 const query::Param& right);
  void resolveLeftParamForOnDemandRs(
      const query::Param& left, const query::Param& right,
      std::vector<std::tuple<query::Param, query::Param, ParamPosition>>&
          newParams);
  void resolveRightParamForOnDemandRs(
      const query::Param& left, const query::Param& right,
      std::vector<std::tuple<query::Param, query::Param, ParamPosition>>&
          newParams);

  // miscellaneous helpers
  query::ClauseIncomingResults formatRefResults(
      std::unordered_set<int> results);
  std::unordered_set<int> getAllValuesOfSynonym(std::string synonymName);
  query::FinalQueryResults getSelectSynonymFinalResults(
      query::SelectClause selectClause);
  bool checkIsCorrectDesignEntity(int stmtNum, DesignEntity designEntity);
};
