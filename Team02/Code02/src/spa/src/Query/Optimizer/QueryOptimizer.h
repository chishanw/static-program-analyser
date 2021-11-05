#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct CLAUSE_HASH {
  size_t operator()(const query::ConditionClause& clause) const {
    int i = static_cast<int>(clause.conditionClauseType);
    std::string s;
    switch (clause.conditionClauseType) {
      case query::ConditionClauseType::SUCH_THAT: {
        i = i * 10 + static_cast<int>(clause.suchThatClause.relationshipType);
        i = i * 10 + static_cast<int>(clause.suchThatClause.leftParam.type);
        i = i * 10 + static_cast<int>(clause.suchThatClause.rightParam.type);
        s = std::to_string(i) + clause.suchThatClause.leftParam.value +
            clause.suchThatClause.rightParam.value;
        break;
      }
      case query::ConditionClauseType::PATTERN: {
        i = i * 10 + static_cast<int>(clause.patternClause.matchSynonym.entity);
        i = i * 10 +
            static_cast<int>(clause.patternClause.matchSynonym.attribute);
        i = i * 10 + static_cast<int>(clause.patternClause.leftParam.type);
        i = i * 10 +
            static_cast<int>(clause.patternClause.patternExpr.matchType);
        s = std::to_string(i) + clause.patternClause.matchSynonym.name +
            clause.patternClause.leftParam.value +
            clause.patternClause.patternExpr.expr;
        break;
      }
      case query::ConditionClauseType::WITH: {
        i = i * 10 + static_cast<int>(clause.withClause.leftParam.type);
        i = i * 10 + static_cast<int>(clause.withClause.rightParam.type);
        s = std::to_string(i) + clause.withClause.leftParam.value +
            clause.withClause.rightParam.value;
        break;
      }
    }
    return std::hash<std::string>{}(s);
  }
};

struct DetailedGrpInfo {
  query::GroupDetails details;
  bool isLiteralGroup;
  int totalNumClauses;
  int numEfficientClauses;
  int numExpensiveClauses;
};

namespace optimizer_unit_test {
struct OptimizerTester;
};

typedef int SUBSET_ID;
typedef std::unordered_map<std::string, DesignEntity> SynonymMap;
typedef std::vector<query::ConditionClause> Group;
typedef std::pair<Group, DetailedGrpInfo> GroupAndInfoPair;

class QueryOptimizer {
 public:
  explicit QueryOptimizer(PKB*);

  void PreprocessClauses(SynonymMap, const query::SelectClause&);

  std::optional<query::GroupDetails> GetNextGroupDetails();
  std::optional<query::ConditionClause> GetNextClause(
      query::SynonymCountsTable&);

 private:
  friend struct ::optimizer_unit_test::OptimizerTester;
  PKB* pkb;

  SynonymMap synonymMap;
  query::SynonymCountsTable* synonymCountTable;
  std::vector<GroupAndInfoPair> groupAndInfoPairs;
  bool isFirstGroup = true;

  std::vector<Group> groupClauses(std::vector<query::ConditionClause>);
  std::vector<std::pair<Group, DetailedGrpInfo>> extractGroupInfo(
      const std::vector<query::Synonym>&, const std::vector<Group>&);

  void sortGroups();
  void sortClausesAtGroupIndex(int);

  static std::unordered_set<query::SYN_NAME> extractSynonymsUsed(
      const query::ConditionClause&);
  std::vector<unsigned long> getSizesOfSynonyms(
      std::unordered_set<query::SYN_NAME>*);
  unsigned long getSizeOfClause(const query::ConditionClause&);
  bool hasCommonSynonyms(const query::ConditionClause&);
};
