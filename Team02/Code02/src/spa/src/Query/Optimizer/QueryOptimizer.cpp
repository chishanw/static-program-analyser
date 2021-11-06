#include "QueryOptimizer.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;
using namespace query;
using namespace optimizer;

const unordered_set<RelationshipType> expensiveRelationships = {
    RelationshipType::NEXT_T,      RelationshipType::NEXT_BIP_T,
    RelationshipType::AFFECTS,     RelationshipType::AFFECTS_T,
    RelationshipType::AFFECTS_BIP, RelationshipType::AFFECTS_BIP_T};

QueryOptimizer::QueryOptimizer(PKB* pkb) {
  this->pkb = pkb;
  this->synonymCountTable = {};
}

optional<query::ConditionClause> QueryOptimizer::GetNextClause(
    query::SynonymCountsTable& countTable) {
  synonymCountTable = &countTable;
  if (groupAndInfoPairs.empty() || groupAndInfoPairs[0].first.empty()) {
    return nullopt;
  }
  Group& group = groupAndInfoPairs[0].first;
  sortClausesAtGroupIndex(0);

  ConditionClause clause = group[0];
  group.erase(group.begin());
  return {clause};
}

optional<query::GroupDetails> QueryOptimizer::GetNextGroupDetails() {
  if (groupAndInfoPairs.empty()) {
    return nullopt;
  }

  if (isFirstGroup) {
    isFirstGroup = false;
    return {groupAndInfoPairs[0].second.details};
  }

  // remove used group and detail pair
  groupAndInfoPairs.erase(groupAndInfoPairs.begin());

  if (groupAndInfoPairs.empty()) {
    return nullopt;
  }

  return {groupAndInfoPairs[0].second.details};
}

void QueryOptimizer::PreprocessClauses(SynonymMap map,
                                       const SelectClause& selectClause) {
  synonymMap = std::move(map);
  vector<vector<ConditionClause>> groupsOfClauses =
      groupClauses(selectClause.conditionClauses);
  groupAndInfoPairs =
      extractGroupInfo(selectClause.selectSynonyms, groupsOfClauses);
  sortGroups();
}

vector<Group> QueryOptimizer::groupClauses(vector<ConditionClause> clauses) {
  unordered_set<ConditionClause, CLAUSE_HASH> clausesSet(clauses.begin(),
                                                         clauses.end());

  unordered_map<SYN_NAME, SUBSET_ID> synNameToSubsetId;
  unordered_map<SUBSET_ID, unordered_set<ConditionClause, CLAUSE_HASH>>
      subsetIdToClauses;
  unordered_map<SUBSET_ID, unordered_set<SYN_NAME>> subsetIdToSynNames;

  int nextOpenGrpIdx = 0;

  for (const ConditionClause& clause : clausesSet) {
    vector<string> synonymNames = extractSynonymsUsed(clause);
    // create new subset if clause has no synonyms
    if (synonymNames.empty()) {
      subsetIdToClauses[nextOpenGrpIdx].insert(clause);
      nextOpenGrpIdx += 1;
      continue;
    }

    // -1 represents unassigned subsetId
    int clauseSubsetId = -1;

    // link synonyms to a subset
    for (const string& name : synonymNames) {
      // if current synonym name is new and has no id yet
      if (synNameToSubsetId.find(name) == synNameToSubsetId.end()) {
        if (clauseSubsetId == -1) {
          clauseSubsetId = nextOpenGrpIdx;
          nextOpenGrpIdx += 1;
        }
        synNameToSubsetId[name] = clauseSubsetId;
        subsetIdToSynNames[clauseSubsetId].insert(name);
        subsetIdToClauses[clauseSubsetId].insert(clause);
        continue;
      }
      // current synonym name is not new and has an id
      int otherSubsetId = synNameToSubsetId[name];

      // if current clause has no id yet, use the same id as otherSubsetId
      if (clauseSubsetId == -1) {
        clauseSubsetId = otherSubsetId;
        subsetIdToClauses[clauseSubsetId].insert(clause);
        continue;
      }

      // current clause has a different id, merge both sets
      if (clauseSubsetId != otherSubsetId) {
        SUBSET_ID goalId = subsetIdToSynNames[clauseSubsetId].size() >
                                   subsetIdToSynNames[otherSubsetId].size()
                               ? clauseSubsetId
                               : otherSubsetId;

        SUBSET_ID transferId = subsetIdToSynNames[clauseSubsetId].size() >
                                       subsetIdToSynNames[otherSubsetId].size()
                                   ? otherSubsetId
                                   : clauseSubsetId;

        for (const SYN_NAME& transferName : subsetIdToSynNames[transferId]) {
          synNameToSubsetId[transferName] = goalId;
        }

        subsetIdToSynNames[goalId].merge(subsetIdToSynNames[transferId]);
        subsetIdToSynNames.erase(transferId);
        subsetIdToClauses[goalId].merge(subsetIdToClauses[transferId]);
        subsetIdToClauses.erase(transferId);
      }
    }
  }

  vector<vector<ConditionClause>> groupsOfClauses;
  for (const auto& pair : subsetIdToClauses) {
    vector<ConditionClause> group;
    for (auto& clause : pair.second) {
      group.push_back(clause);
    }
    groupsOfClauses.push_back(group);
  }
  return groupsOfClauses;
}

vector<pair<Group, DetailedGrpInfo>> QueryOptimizer::extractGroupInfo(
    const vector<query::Synonym>& selectSynonyms,
    const vector<Group>& groupsOfClauses) {
  vector<pair<Group, DetailedGrpInfo>> pairs;
  for (auto& group : groupsOfClauses) {
    unordered_set<string> groupSynonyms = {};
    vector<Synonym> groupSelectSynonyms = {};
    int numEfficientClauses = 0;
    int numExpensiveClauses = 0;
    int curClauseIndex = 0;

    for (auto& clause : group) {
      vector<string> clauseSynonyms = extractSynonymsUsed(clause);
      for (string s : clauseSynonyms) {
        groupSynonyms.insert(s);
      }
      ClauseDifficulty difficulty = ClauseDifficulty::NORMAL;
      if (clause.conditionClauseType == ConditionClauseType::WITH) {
        numEfficientClauses++;
        difficulty = ClauseDifficulty::EFFICIENT;
      }
      if (clause.conditionClauseType == ConditionClauseType::SUCH_THAT &&
          expensiveRelationships.find(clause.suchThatClause.relationshipType) !=
              expensiveRelationships.end()) {
        numExpensiveClauses++;
        difficulty = ClauseDifficulty::EXPENSIVE;
      }

      DetailedClauseInfo clauseInfo = {difficulty, clauseSynonyms,
                                       curClauseIndex};
      clauseToClauseInfo.insert({clause, clauseInfo});
      curClauseIndex++;
    }

    for (auto& synonym : selectSynonyms) {
      if (groupSynonyms.find(synonym.name) != groupSynonyms.end()) {
        groupSelectSynonyms.push_back(synonym);
      }
    }

    bool isLiteralGroup = groupSynonyms.empty();
    bool isBoolGroup = groupSelectSynonyms.empty();
    int totalNumClauses = group.size();
    DetailedGrpInfo info = {{isBoolGroup, groupSelectSynonyms},
                            isLiteralGroup,
                            totalNumClauses,
                            numEfficientClauses,
                            numExpensiveClauses};
    pairs.emplace_back(group, info);
  }
  return pairs;
}

void QueryOptimizer::sortGroups() {
  // groups comparator
  // priority: groups with less expensive clauses -> groups with more efficient
  // clauses -> groups of smaller sizes
  auto groupComparator = [](const GroupAndInfoPair& groupAndInfoA,
                            const GroupAndInfoPair& groupAndInfoB) {
    DetailedGrpInfo infoA = groupAndInfoA.second;
    DetailedGrpInfo infoB = groupAndInfoB.second;
    if (infoA.numExpensiveClauses != infoB.numExpensiveClauses) {
      return infoB.numExpensiveClauses > infoA.numExpensiveClauses;
    }
    if (infoA.isLiteralGroup != infoB.isLiteralGroup) {
      return infoA.isLiteralGroup;
    }
    if (infoB.numEfficientClauses != infoA.numEfficientClauses) {
      return infoB.numEfficientClauses <= infoA.numEfficientClauses;
    }
    return infoB.totalNumClauses > infoA.totalNumClauses;
  };

  sort(groupAndInfoPairs.begin(), groupAndInfoPairs.end(), groupComparator);
}

void QueryOptimizer::sortClausesAtGroupIndex(int index) {
  // clauses comparator
  // priority: common synonym -> non-expensive clauses -> estimated pdt size ->
  // efficient clauses
  auto clausesComparator = [this](const ConditionClause& clauseA,
                                  const ConditionClause& clauseB) {
    DetailedClauseInfo clauseInfoA = clauseToClauseInfo[clauseA];
    DetailedClauseInfo clauseInfoB = clauseToClauseInfo[clauseB];

    bool hasCommonA = hasCommonSynonyms(clauseInfoA.synonyms);
    bool hasCommonB = hasCommonSynonyms(clauseInfoB.synonyms);
    if ((hasCommonA || hasCommonB) && !(hasCommonA && hasCommonB)) {
      return hasCommonA;
    }

    bool isExpensiveA = clauseInfoA.difficulty == ClauseDifficulty::EXPENSIVE;
    bool isExpensiveB = clauseInfoB.difficulty == ClauseDifficulty::EXPENSIVE;
    if ((isExpensiveA || isExpensiveB) && !(isExpensiveA && isExpensiveB)) {
      return isExpensiveB;
    }

    unsigned long estSizeA =
        getSizeOfClause(clauseA.conditionClauseType, clauseInfoA.synonyms);
    unsigned long estSizeB =
        getSizeOfClause(clauseB.conditionClauseType, clauseInfoB.synonyms);
    if (estSizeA != estSizeB) {
      return estSizeB > estSizeA;
    }

    bool isEfficientA = clauseInfoA.difficulty == ClauseDifficulty::EFFICIENT;
    bool isEfficientB = clauseInfoB.difficulty == ClauseDifficulty::EFFICIENT;
    if ((isEfficientA || isExpensiveB) && !(isEfficientA && isEfficientB)) {
      return isEfficientA;
    }

    return clauseInfoA.clauseIndex > clauseInfoB.clauseIndex;
  };

  Group& group = groupAndInfoPairs[index].first;
  sort(group.begin(), group.end(), clausesComparator);
}

unsigned long QueryOptimizer::getSizeOfClause(
    query::ConditionClauseType type, const std::vector<std::string>& synonyms) {
  vector<int> sizes = {};
  for (const string& s : synonyms) {
    if (synonymCountTable != nullptr &&
        synonymCountTable->find(s) != synonymCountTable->end()) {
      sizes.push_back(synonymCountTable->at(s));
    } else {
      sizes.push_back(pkb->getNumEntity(synonymMap[s]));
    }
  }
  if (type == ConditionClauseType::WITH) {
    if (synonyms.size() < 2) {
      return 1;
    }
    return *min_element(sizes.begin(), sizes.end());
  }
  return reduce(sizes.begin(), sizes.end(), 1, multiplies<>());
}

bool QueryOptimizer::hasCommonSynonyms(
    const std::vector<std::string>& clauseSynonyms) {
  return any_of(clauseSynonyms.begin(), clauseSynonyms.end(),
                [this](const string& s) {
                  return synonymCountTable != nullptr &&
                         synonymCountTable->find(s) != synonymCountTable->end();
                });
}

vector<SYN_NAME> QueryOptimizer::extractSynonymsUsed(
    const ConditionClause& clause) {
  unordered_set<ParamType> synonymTypes = {
      ParamType::SYNONYM, ParamType::ATTRIBUTE_PROC_NAME,
      ParamType::ATTRIBUTE_VAR_NAME, ParamType::ATTRIBUTE_VALUE,
      ParamType::ATTRIBUTE_STMT_NUM};

  vector<string> synonymNamesUsed;
  ConditionClauseType type = clause.conditionClauseType;
  switch (type) {
    case ConditionClauseType::SUCH_THAT: {
      SuchThatClause stClause = clause.suchThatClause;
      if (synonymTypes.find(stClause.leftParam.type) != synonymTypes.end()) {
        synonymNamesUsed.push_back(stClause.leftParam.value);
      }
      if (synonymTypes.find(stClause.rightParam.type) != synonymTypes.end()) {
        synonymNamesUsed.push_back(stClause.rightParam.value);
      }
      return synonymNamesUsed;
    }
    case ConditionClauseType::PATTERN: {
      PatternClause patternClause = clause.patternClause;
      synonymNamesUsed.push_back(patternClause.matchSynonym.name);
      if (synonymTypes.find(patternClause.leftParam.type) !=
          synonymTypes.end()) {
        synonymNamesUsed.push_back(patternClause.leftParam.value);
      }
      return synonymNamesUsed;
    }
    case ConditionClauseType::WITH: {
      WithClause withClause = clause.withClause;
      if (synonymTypes.find(withClause.leftParam.type) != synonymTypes.end()) {
        synonymNamesUsed.push_back(withClause.leftParam.value);
      }
      if (synonymTypes.find(withClause.rightParam.type) != synonymTypes.end()) {
        synonymNamesUsed.push_back(withClause.rightParam.value);
      }
      return synonymNamesUsed;
    }
  }
  return synonymNamesUsed;
}
