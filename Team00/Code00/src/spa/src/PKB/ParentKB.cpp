#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Common/Global.h"
#include "ParentKB.h"

using namespace std;

ParentKB::ParentKB() {
  table = unordered_map<STMT_NO, UNO_SET_OF_STMT_NO>({});
  invTable = unordered_map<STMT_NO, STMT_NO>({});
  tableT = unordered_map<STMT_NO, UNO_SET_OF_STMT_NO>({});
  invTableT = unordered_map<STMT_NO, UNO_SET_OF_STMT_NO>({});
}

void ParentKB::setParent(STMT_NO s1, STMT_NO s2) {
  if (table.count(s1) == 0) {
    // Empty, insert new set
    UNO_SET_OF_STMT_NO newSet({s2});
    table.insert({s1, newSet});
  } else {
    // Not empty, add s2
    table.at(s1).insert(s2);
  }

  if (invTable.count(s2) == 0) {
    invTable.insert({s2, s1});
  }

  // check if parent exists already
  if (getParent(s2) != s1) {
    ostringstream s;
    s << "[debug][setParent] parent for child "
      << "{" << s2 << "}"
      << " already exists"
         " and it is not "
      << "{" << s1 << "}";
    DMOprintErrMsgAndExit(s.str());
  }
}

void ParentKB::addParentT(STMT_NO s1, STMT_NO s2) {
  if (tableT.count(s1) == 0) {
    // If no key, insert Set
    UNO_SET_OF_STMT_NO newSet({s2});
    tableT.insert({{s1, newSet}});
  }
  if (invTableT.count(s2) == 0) {
    // If no key, insert Set
    UNO_SET_OF_STMT_NO newInvSet({s1});
    invTableT.insert({{s2, newInvSet}});
  }

  if (tableT.at(s1).count(s2) == 0) {
    // Add key to exisiting Set
    tableT.at(s1).insert(s2);
  }
  if (invTableT.at(s2).count(s1) == 0) {
    // Add key to exisiting Set
    invTableT.at(s2).insert(s1);
  }
}

// Added, since table has more than one child for each parent
bool ParentKB::isParent(STMT_NO s1, STMT_NO s2) {
  if (table.count(s1) == 0) {
    // Key does not exist
    return false;
  } else {
    unordered_set<STMT_NO> memberSet = table.at(s1);
    int result = memberSet.count(s2);
    return result > 0;
  }
}

bool ParentKB::isParentT(STMT_NO s1, STMT_NO s2) {
  if (tableT.count(s1) == 0) {
    // Key does not exist
    return false;
  } else {
    unordered_set<STMT_NO> memberSet = tableT.at(s1);
    int result = memberSet.count(s2);
    return result > 0;
  }
}

UNO_SET_OF_STMT_NO ParentKB::getChildren(STMT_NO s1) {
  // Check if s1 is a valid key
  if (table.count(s1) == 0) {
    // Return Empty Set
    return UNO_SET_OF_STMT_NO({});
  } else {
    return table.at(s1);
  }
}

STMT_NO ParentKB::getParent(STMT_NO s2) {
  // Check if s2 is a valid key
  if (invTable.count(s2) == 0) {
    return -1;
  } else {
    return invTable.at(s2);
  }
}

UNO_SET_OF_STMT_NO ParentKB::getChildrenT(STMT_NO s1) {
  if (tableT.count(s1) == 0) {
    // Return empty set
    return UNO_SET_OF_STMT_NO({});
  } else {
    return tableT.at(s1);
  }
}

UNO_SET_OF_STMT_NO ParentKB::getParentsT(STMT_NO s2) {
  if (invTableT.count(s2) == 0) {
    return UNO_SET_OF_STMT_NO({});
  } else {
    // Return empty set
    return invTableT.at(s2);
  }
}

vector<pair<STMT_NO, vector<STMT_NO>>> ParentKB::getAllParentsStmtPairs() {
  // Iterate through all pairs
  vector<pair<STMT_NO, vector<STMT_NO>>> outputList({});

  for (auto it = table.begin(); it != table.end(); it++) {
    auto set = it->second;
    auto innerList = vector(set.begin(), set.end());
    auto element = pair(it->first, innerList);
    outputList.push_back(element);
  }

  return outputList;
}

vector<pair<STMT_NO, vector<STMT_NO>>> ParentKB::getAllParentsTStmtPairs() {
  // Iterate through all pairs
  vector<pair<STMT_NO, vector<STMT_NO>>> outputList({});

  for (auto it = tableT.begin(); it != tableT.end(); it++) {
    auto set = it->second;
    auto innerList = vector(set.begin(), set.end());
    auto element = pair(it->first, innerList);
    outputList.push_back(element);
  }

  return outputList;
}