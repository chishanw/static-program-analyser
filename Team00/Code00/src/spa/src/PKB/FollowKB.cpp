#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FollowKB.h"
#include "Common/Global.h"

using namespace std;

FollowKB::FollowKB() {
  table = unordered_map<STMT_NO, STMT_NO>({});
  invTable = unordered_map<STMT_NO, STMT_NO>({});
  tableT = unordered_map<STMT_NO, UNO_SET_OF_STMT_NO>({});
  invTableT = unordered_map<STMT_NO, UNO_SET_OF_STMT_NO>({});
}

void FollowKB::setFollows(STMT_NO s1, STMT_NO s2) {
  if (table.count(s1) == 0) {
    // True if successful
    table.insert({{s1, s2}});
    invTable.insert({{s2, s1}});
    return;
  }

  // check if value for key s1 is s2
  if (getFollows(s1) != s2) {
    ostringstream s;
    s << "[debug][setFollows] value for key "
      << "{" << s1 << "}"
      << " already exists"
         " and it is not "
      << "{" << s2 << "}";
    DMOprintErrMsgAndExit(s.str());
  }
}

void FollowKB::addFollowsT(STMT_NO s1, STMT_NO s2) {
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

bool FollowKB::isFollowsT(STMT_NO s1, STMT_NO s2) {
  if (tableT.count(s1) == 0) {
    // Key does not exist
    return false;
  } else {
    unordered_set<STMT_NO> memberSet = tableT.at(s1);
    int result = memberSet.count(s2);
    return result > 0;
  }
}

STMT_NO FollowKB::getFollows(STMT_NO s1) {
  // Check if s1 is a valid key
  if (table.count(s1) == 0) {
    return -1;
  } else {
    return table.at(s1);
  }
}

STMT_NO FollowKB::getFollowedBy(STMT_NO s2) {
  // Check if s2 is a valid key
  if (invTable.count(s2) == 0) {
    return -1;
  } else {
    return invTable.at(s2);
  }
}

UNO_SET_OF_STMT_NO FollowKB::getFollowsT(STMT_NO s1) {
  if (tableT.count(s1) == 0) {
    // Return empty set
    return UNO_SET_OF_STMT_NO({});
  } else {
    return tableT.at(s1);
  }
}

UNO_SET_OF_STMT_NO FollowKB::getFollowedTBy(STMT_NO s2) {
  if (invTableT.count(s2) == 0) {
    return UNO_SET_OF_STMT_NO({});
  } else {
    // Return empty set
    return invTableT.at(s2);
  }
}

vector<vector<STMT_NO>> FollowKB::getAllFollowsStmtPairs() {
  // Iterate through all pairs
  vector<vector<STMT_NO>> outputList({});

  for (auto it = table.begin(); it != table.end(); ++it) {
    outputList.push_back(vector({it->first, it->second}));
  }

  return outputList;
}

vector<pair<STMT_NO, vector<STMT_NO>>> FollowKB::getAllFollowsTStmtPairs() {
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
