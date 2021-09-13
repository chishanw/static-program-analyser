#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/FollowsEvaluator.h>
#include <Query/Evaluator/ParentEvaluator.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class QueryEvaluator {
 public:
  explicit QueryEvaluator(PKB*);
  std::unordered_set<int> evaluateQuery(
      std::unordered_map<string, query::DesignEntity> synonyms,
      query::SelectClause select);

 private:
  PKB* pkb;
  FollowsEvaluator followsEvaluator;
  ParentEvaluator parentEvaluator;

  bool areAllClausesTrue;
  std::vector<std::unordered_map<std::string, int>> queryResults;
  std::vector<std::unordered_map<std::string, int>> currentQueryResults;
  std::unordered_set<std::string> queryResultsSynonyms;

  // methods to build queryResults
  void initializeQueryResults(std::vector<std::vector<int>> incomingResults,
                              const query::Param& left,
                              const query::Param& right);
  void addIncomingResult(std::vector<std::vector<int>> incomingResults,
                         const query::Param& left, const query::Param& right);
  void filter(std::vector<std::vector<int>> incomingResults,
              std::vector<string> incomingResultsSynonyms);
  void innerJoin(std::vector<std::vector<int>> incomingResults,
                 std::vector<string> incomingResultsSynonyms);
  void crossProduct(std::vector<std::vector<int>> incomingResults,
                    std::vector<string> incomingResultsSynonyms);

  // methods to call the relevant sub evaluator
  void evaluateSuchThatClause(query::SuchThatClause);
  void evaluateFollowsClause(query::SuchThatClause);
  void evaluateFollowsTClause(query::SuchThatClause);
  void evaluateParentClause(query::SuchThatClause);
  void evaluateParentTClause(query::SuchThatClause);

  // helper methods
  bool isBoolClause(const query::Param& left, const query::Param& right);
  bool isRefClause(const query::Param& left, const query::Param& right);

  std::vector<std::vector<int>> formatRefResults(
      std::unordered_set<int> results);
  std::vector<std::vector<int>> formatRefPairResults(
      std::vector<std::pair<int, std::vector<int>>> results);

  std::unordered_set<int> getAllValuesOfSynonym(
      std::unordered_map<std::string, query::DesignEntity> allQuerySynonyms,
      std::string synonymName);
  std::unordered_set<int> getSelectSynonymFinalResults(
      std::unordered_map<std::string, query::DesignEntity> allQuerySynonyms,
      string synonymName);
};
