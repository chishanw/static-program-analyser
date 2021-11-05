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
    unordered_set<string> synonymNames = extractSynonymsUsed(clause);
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

    for (auto& clause : group) {
      groupSynonyms.merge(extractSynonymsUsed(clause));
      if (clause.conditionClauseType == ConditionClauseType::WITH) {
        numEfficientClauses++;
      }
      if (clause.conditionClauseType == ConditionClauseType::SUCH_THAT &&
          expensiveRelationships.find(clause.suchThatClause.relationshipType) !=
              expensiveRelationships.end()) {
        numExpensiveClauses++;
      }
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
    bool hasCommonSynonymA = hasCommonSynonyms(clauseA);
    bool hasCommonSynonymB = hasCommonSynonyms(clauseB);
    if (hasCommonSynonymA != hasCommonSynonymB) {
      return hasCommonSynonymA;
    }

    bool isExpensiveA =
        clauseA.conditionClauseType == ConditionClauseType::SUCH_THAT &&
        expensiveRelationships.find(clauseA.suchThatClause.relationshipType) !=
            expensiveRelationships.end();
    bool isExpensiveB =
        clauseB.conditionClauseType == ConditionClauseType::SUCH_THAT &&
        expensiveRelationships.find(clauseB.suchThatClause.relationshipType) !=
            expensiveRelationships.end();
    if (isExpensiveA != isExpensiveB) {
      return isExpensiveB;
    }

    unsigned long estSizeA = getSizeOfClause(clauseA);
    unsigned long estSizeB = getSizeOfClause(clauseB);
    if (estSizeA != estSizeB) {
      return estSizeB > estSizeA;
    }

    bool isEfficientA =
        clauseA.conditionClauseType == ConditionClauseType::WITH;
    return isEfficientA;
  };

  Group& group = groupAndInfoPairs[index].first;
  sort(group.begin(), group.end(), clausesComparator);
}

std::vector<unsigned long> QueryOptimizer::getSizesOfSynonyms(
    std::unordered_set<query::SYN_NAME>* synNames) {
  vector<unsigned long> sizes = {};
  for (const string& s : *synNames) {
    if (synonymCountTable != nullptr &&
        synonymCountTable->find(s) != synonymCountTable->end()) {
      sizes.push_back(synonymCountTable->at(s));
    } else {
      sizes.push_back(pkb->getNumEntity(synonymMap[s]));
    }
  }
  return sizes;
}

unsigned long QueryOptimizer::getSizeOfClause(
    const query::ConditionClause& clause) {
  unordered_set<string> synonyms = QueryOptimizer::extractSynonymsUsed(clause);
  vector<unsigned long> sizes = getSizesOfSynonyms(&synonyms);
  if (!synonyms.empty()) {
    for (string s : synonyms) {
    }
    for (unsigned long s : sizes) {
    }
  }
  if (clause.conditionClauseType == ConditionClauseType::WITH) {
    if (synonyms.size() < 2) {
      return 1;
    }
    return *min_element(sizes.begin(), sizes.end());
  }
  return reduce(sizes.begin(), sizes.end(), 1, multiplies<>());
}

bool QueryOptimizer::hasCommonSynonyms(const ConditionClause& clause) {
  unordered_set<string> synonyms = QueryOptimizer::extractSynonymsUsed(clause);
  return any_of(synonyms.begin(), synonyms.end(), [this](const string& s){
    return synonymCountTable != nullptr &&
           synonymCountTable->find(s) != synonymCountTable->end();
  });
}

unordered_set<SYN_NAME> QueryOptimizer::extractSynonymsUsed(
    const ConditionClause& clause) {
  unordered_set<ParamType> synonymTypes = {
      ParamType::SYNONYM, ParamType::ATTRIBUTE_PROC_NAME,
      ParamType::ATTRIBUTE_VAR_NAME, ParamType::ATTRIBUTE_VALUE,
      ParamType::ATTRIBUTE_STMT_NUM};

  unordered_set<string> synonymNamesUsed;
  ConditionClauseType type = clause.conditionClauseType;
  switch (type) {
    case ConditionClauseType::SUCH_THAT: {
      SuchThatClause stClause = clause.suchThatClause;
      if (synonymTypes.find(stClause.leftParam.type) != synonymTypes.end()) {
        synonymNamesUsed.insert(stClause.leftParam.value);
      }
      if (synonymTypes.find(stClause.rightParam.type) != synonymTypes.end()) {
        synonymNamesUsed.insert(stClause.rightParam.value);
      }
      return synonymNamesUsed;
    }
    case ConditionClauseType::PATTERN: {
      PatternClause patternClause = clause.patternClause;
      synonymNamesUsed.insert(patternClause.matchSynonym.name);
      if (synonymTypes.find(patternClause.leftParam.type) !=
          synonymTypes.end()) {
        synonymNamesUsed.insert(patternClause.leftParam.value);
      }
      return synonymNamesUsed;
    }
    case ConditionClauseType::WITH: {
      WithClause withClause = clause.withClause;
      if (synonymTypes.find(withClause.leftParam.type) != synonymTypes.end()) {
        synonymNamesUsed.insert(withClause.leftParam.value);
      }
      if (synonymTypes.find(withClause.rightParam.type) != synonymTypes.end()) {
        synonymNamesUsed.insert(withClause.rightParam.value);
      }
      return synonymNamesUsed;
    }
  }
}
