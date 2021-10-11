#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "NextKB.h"
#include "Common/Global.h"

using namespace std;

NextKB::NextKB() { }

void NextKB::addNext(STMT_NO s1, STMT_NO s2) {
  UNO_SET_OF_STMT_NO newSet({ s2 });
  UNO_SET_OF_STMT_NO newInvSet({ s1 });
  if (table.count(s1) == 0) {
    table.insert({{ s1, newSet }});
  } else {
    table.at(s1).merge(newSet);
  }
  if (invTable.count(s2) == 0) {
    invTable.insert({ s2, newInvSet });
  } else {
    invTable.at(s2).merge(newInvSet);
  }
}

bool NextKB::isNext(STMT_NO s1, STMT_NO s2) {
  if (table.count(s1) == 0) {
    // Key does not exist
    return false;
  } else {
    return table.at(s1).count(s2);
  }
}

UNO_SET_OF_STMT_NO NextKB::getNextStmts(STMT_NO s1) {
  if (table.count(s1) == 0) {
    // Return empty set
    return UNO_SET_OF_STMT_NO({});
  } else {
    return table.at(s1);
  }
}

UNO_SET_OF_STMT_NO NextKB::getPreviousStmts(STMT_NO s2) {
  if (invTable.count(s2) == 0) {
    return UNO_SET_OF_STMT_NO({});
  } else {
    // Return empty set
    return invTable.at(s2);
  }
}

vector<pair<STMT_NO, vector<STMT_NO>>> NextKB::getAllNextStmtPairs() {
  vector<pair<STMT_NO, vector<STMT_NO>>> result;
  for (auto it = table.begin(); it != table.end(); it++) {
    STMT_NO s = it->first;
    unordered_set<STMT_NO> allStmts = it->second;
    vector<STMT_NO> stmtVec(allStmts.begin(), allStmts.end());
    result.push_back(pair(s, stmtVec));
  }
  return result;
}
