#include "QueryOptimizer.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;
using namespace query;

QueryOptimizer::QueryOptimizer(PKB* pkb) { this->pkb = pkb; }

void QueryOptimizer::PreprocessClauses(SynonymMap map,
                                       SelectClause selectClause) {
  Groups groups = groupClauses(selectClause.conditionClauses);
  GroupDetailAndGroupPairs pairsOfGroupDetailAndGroup = extractGroupDetails(
      selectClause.selectType, selectClause.selectSynonyms, groups);
}

Groups QueryOptimizer::groupClauses(vector<CLAUSE> clauses) {
  unordered_set<CLAUSE, CLAUSE_HASH> clausesSet(clauses.begin(), clauses.end());

  unordered_map<SYN_NAME, SUBSET_ID> synNameToSubsetId;
  unordered_map<SUBSET_ID, unordered_set<CLAUSE, CLAUSE_HASH>>
      subsetIdToClauses;
  unordered_map<SUBSET_ID, unordered_set<SYN_NAME>> subsetIdToSynNames;

  int nextOpenGrpIdx = 0;

  for (CLAUSE clause : clausesSet) {
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

  vector<unordered_set<CLAUSE, CLAUSE_HASH>> groupsOfClauses;
  groupsOfClauses.reserve(subsetIdToClauses.size());
  for (const auto& p : subsetIdToClauses) {
    groupsOfClauses.push_back(p.second);
  }
  return groupsOfClauses;
}

GroupDetailAndGroupPairs QueryOptimizer::extractGroupDetails(
    query::SelectType selectType, std::vector<query::Synonym> selectSynonyms,
    Groups groupsOfClauses) {
  GroupDetailAndGroupPairs detailAndGroupPairs = {};

  if (selectType == SelectType::BOOLEAN) {
    for (Group group : groupsOfClauses) {
      GroupDetails detail = {true, {}};
      detailAndGroupPairs.push_back({detail, group});
    }
    return detailAndGroupPairs;
  }

  for (const Group& group : groupsOfClauses) {
    unordered_set<string> groupSynonyms = {};
    for (CLAUSE clause : group) {
      groupSynonyms.merge(extractSynonymsUsed(clause));
    }

    vector<Synonym> selectedGroupSynonyms = {};
    for (Synonym& synonym : selectSynonyms) {
      if (groupSynonyms.find(synonym.name) != groupSynonyms.end()) {
        selectedGroupSynonyms.push_back(synonym);
      }
    }

    bool isBoolGroup = selectedGroupSynonyms.empty();
    GroupDetails detail = {isBoolGroup, selectedGroupSynonyms};
    detailAndGroupPairs.push_back({detail, group});
  }
  return detailAndGroupPairs;
}

unordered_set<SYN_NAME> QueryOptimizer::extractSynonymsUsed(
    ConditionClause& clause) {
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
