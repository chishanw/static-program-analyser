#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/FollowsEvaluator.h>
#include <Query/Evaluator/ModifiesEvaluator.h>
#include <Query/Evaluator/ParentEvaluator.h>
#include <Query/Evaluator/PatternEvaluator.h>
#include <Query/Evaluator/UsesEvaluator.h>
#include <Query/Evaluator/WithEvaluator.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class QueryEvaluator {
 public:
  explicit QueryEvaluator(PKB*);
  std::vector<std::vector<int>> evaluateQuery(
      std::unordered_map<std::string, query::DesignEntity> synonymMap,
      query::SelectClause select);

 private:
  PKB* pkb;
  std::unordered_map<std::string, query::DesignEntity> synonymMap;
  FollowsEvaluator followsEvaluator;
  ParentEvaluator parentEvaluator;
  UsesEvaluator usesEvaluator;
  ModifiesEvaluator modifiesEvaluator;
  PatternEvaluator patternEvaluator;
  WithEvaluator withEvaluator;

  bool areAllClausesTrue;
  query::QueryResults queryResults;
  query::QueryResults currentQueryResults;
  std::unordered_set<std::string> queryResultsSynonyms;

  // methods to build queryResults
  void filterAndAddIncomingResults(
      std::vector<std::vector<int>> incomingResults, const query::Param& left,
      const query::Param& right);
  std::vector<std::vector<int>> filterIncomingResults(
      std::vector<std::vector<int>> incomingResults, const query::Param& left,
      const query::Param& right);
  void initializeQueryResults(std::vector<std::vector<int>> incomingResults,
                              const query::Param& left,
                              const query::Param& right);
  void addIncomingResults(std::vector<std::vector<int>> incomingResults,
                          const query::Param& left, const query::Param& right);

  // main algos to merge results
  void filter(std::vector<std::vector<int>> incomingResults,
              std::vector<std::string> incomingResultsSynonyms);
  void innerJoin(std::vector<std::vector<int>> incomingResults,
                 std::vector<std::string> incomingResultsSynonyms);
  void crossProduct(std::vector<std::vector<int>> incomingResults,
                    std::vector<std::string> incomingResultsSynonyms);

  // methods to call the relevant sub evaluator
  void evaluateSuchThatClause(query::SuchThatClause);
  void evaluateFollowsClause(query::SuchThatClause);
  void evaluateFollowsTClause(query::SuchThatClause);
  void evaluateParentClause(query::SuchThatClause);
  void evaluateParentTClause(query::SuchThatClause);
  void evaluateUsesSClause(query::SuchThatClause);
  void evaluateUsesPClause(query::SuchThatClause);
  void evaluateModifiesSClause(query::SuchThatClause);
  void evaluateModifiesPClause(query::SuchThatClause);

  void evaluatePatternClause(query::PatternClause);
  void evaluateAssignPatternClause(query::PatternClause);
  void evaluateIfPatternClause(query::PatternClause);
  void evaluateWhilePatternClause(query::PatternClause);

  void evaluateWithClause(query::WithClause);

  // helper methods
  std::vector<std::vector<int>> formatRefResults(
      std::unordered_set<int> results);
  std::vector<std::vector<int>> formatRefPairResults(
      std::vector<std::pair<int, std::vector<int>>> results);

  std::unordered_set<int> getAllValuesOfSynonym(std::string synonymName);
  std::vector<std::vector<int>> getSelectSynonymFinalResults(
      query::SelectClause selectClause);

  bool checkIsCorrectDesignEntity(int stmtNum,
                                  query::DesignEntity designEntity);
};
