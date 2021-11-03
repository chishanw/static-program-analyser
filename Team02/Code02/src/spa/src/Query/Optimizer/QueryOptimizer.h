#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/QueryEvaluator.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct CLAUSE_HASH {
  size_t operator()(const query::ConditionClause& clause) const {
    int sum = static_cast<int>(clause.conditionClauseType);
    std::string s;
    switch (clause.conditionClauseType) {
      case query::ConditionClauseType::SUCH_THAT: {
        sum = sum * 10 +
              static_cast<int>(clause.suchThatClause.relationshipType);
        sum = sum * 10 + static_cast<int>(clause.suchThatClause.leftParam.type);
        sum = sum * 10 +
              static_cast<int>(clause.suchThatClause.rightParam.type);
        s = std::to_string(sum) + clause.suchThatClause.leftParam.value +
            clause.suchThatClause.rightParam.value;
        break;
      }
      case query::ConditionClauseType::PATTERN: {
        sum = sum * 10 +
              static_cast<int>(clause.patternClause.matchSynonym.entity);
        sum = sum * 10 +
              static_cast<int>(clause.patternClause.matchSynonym.attribute);
        sum = sum * 10 + static_cast<int>(clause.patternClause.leftParam.type);
        sum = sum * 10 +
              static_cast<int>(clause.patternClause.patternExpr.matchType);
        s = std::to_string(sum) + clause.patternClause.matchSynonym.name +
            clause.patternClause.leftParam.value +
            clause.patternClause.patternExpr.expr;
        break;
      }
      case query::ConditionClauseType::WITH: {
        sum = sum * 10 + static_cast<int>(clause.withClause.leftParam.type);
        sum = sum * 10 + static_cast<int>(clause.withClause.rightParam.type);
        s = std::to_string(sum) + clause.withClause.leftParam.value +
            clause.withClause.rightParam.value;
        break;
      }
    }
    return std::hash<std::string>{}(s);
  }
};

typedef int SUBSET_ID;

typedef std::unordered_map<std::string, DesignEntity> SynonymMap;
typedef std::unordered_set<query::ConditionClause, CLAUSE_HASH> Group;
typedef std::vector<Group> Groups;
typedef std::vector<std::pair<query::GroupDetails, Group>>
    GroupDetailAndGroupPairs;

namespace optimizer_unit_test {
struct OptimizerTester;
}

class QueryOptimizer {
 public:
  explicit QueryOptimizer(PKB*);

  void PreprocessClauses(SynonymMap, query::SelectClause);

  std::optional<query::GroupDetails> GetNextGroupDetails();
  std::optional<query::ConditionClause> GetNextClause(
      query::SynonymCountsTable&);

 private:
  PKB* pkb;
  friend struct ::optimizer_unit_test::OptimizerTester;

  GroupDetailAndGroupPairs tempGroupDetailsAndGroupPairs;
  // TODO(Beatrice): Convert to PQ for sorting
  bool isFirstGroup = true;
  int currentGroupIndex = 0;
  // TODO(Beatrice): remove indexes and replace with PQ pops

  Groups groupClauses(std::vector<query::ConditionClause>);
  GroupDetailAndGroupPairs extractGroupDetails(query::SelectType,
                                               std::vector<query::Synonym>,
                                               Groups);

  std::unordered_set<query::SYN_NAME> extractSynonymsUsed(
      query::ConditionClause&);
};
