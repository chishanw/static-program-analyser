#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/FollowsEvaluator.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
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

  bool areAllClausesTrue;
  std::vector<std::unordered_map<std::string, int>> queryResults;
  std::vector<std::unordered_map<std::string, int>> currentQueryResults;
  std::unordered_set<std::string> queryResultsSynonyms;

  // helper methods
  bool isBoolClause(const query::Param& left, const query::Param& right);
  bool isRefClause(const query::Param& left, const query::Param& right);
  std::unordered_set<int> getAllValuesOfSynonym(
      std::unordered_map<std::string, query::DesignEntity> allQuerySynonyms,
      std::string synonymName);
  std::unordered_set<int> getSelectSynonymFinalResults(
      std::unordered_map<std::string, query::DesignEntity> allQuerySynonyms,
      string synonymName);

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

  std::vector<std::vector<int>> evaluateRefSuchThat(
      query::RelationshipType relationshipType, const query::Param& left,
      const query::Param& right);
  std::vector<std::vector<int>> evaluateRefPairSuchThat(
      query::RelationshipType relationshipType, const query::Param& left,
      const query::Param& right);
};
