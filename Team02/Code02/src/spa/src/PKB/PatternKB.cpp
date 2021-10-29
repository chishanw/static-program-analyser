#include "PatternKB.h"

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Common/Global.h"

using namespace std;

PatternKB::PatternKB(Table* varTable) {
  this->varTable = varTable;
  tableOfFullExpr = unordered_map<string, UNO_SET_OF_STMT_NO>();
  tableOfFullExprPtt =
      unordered_map<string, unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO>>();
  tableOfSubExprPtt =
      unordered_map<string, unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO>>();
  tableOfFullExprPairs = unordered_map<string, vector<vector<int>>>();
  tableOfSubExprPairs = unordered_map<string, vector<vector<int>>>();

  tableOfAssignForVar = unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO>();
  listOfAssignVarPairs = vector<vector<int>>({});
  tableOfIfPtt = unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO>();
  tableOfWhilePtt = unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO>();
  listOfIfVarPairs = vector<vector<int>>({});
  listOfWhileVarPairs = vector<vector<int>>({});
}

void PatternKB::addAssignPttFullExpr(STMT_NO s, string varName, string expr) {
  VAR_IDX varIdx = varTable->insert(varName);

  if (tableOfFullExpr.count(expr) == 0) {
    UNO_SET_OF_STMT_NO newSet({s});
    tableOfFullExpr.insert({expr, newSet});
  } else {
    tableOfFullExpr.at(expr).insert(s);
  }

  if (tableOfFullExprPtt.count(expr) == 0) {
    UNO_SET_OF_STMT_NO newSet({s});
    unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO> newMap({{varIdx, newSet}});
    tableOfFullExprPtt.insert({expr, newMap});
  }

  auto curr_map = &tableOfFullExprPtt.at(expr);

  if (curr_map->count(varIdx) == 0) {
    // Empty, insert new set
    UNO_SET_OF_STMT_NO newSet({s});
    curr_map->insert({varIdx, newSet});
  } else {
    curr_map->at(varIdx).insert(s);
  }

  if (tableOfFullExprPairs.count(expr) == 0) {
    // Insert new set
    vector<int> newPair({s, varIdx});
    vector<vector<int>> newList({newPair});
    tableOfFullExprPairs.insert({expr, newList});
  } else {
    vector<int> newPair({s, varIdx});
    tableOfFullExprPairs.at(expr).push_back(newPair);
  }

  if (tableOfAssignForVar.count(varIdx) == 0) {
    // Insert new Set
    UNO_SET_OF_STMT_NO newSet({s});
    tableOfAssignForVar.insert({varIdx, newSet});
  } else {
    tableOfAssignForVar.at(varIdx).insert(s);
  }

  // TODO(Merlin): Ensure no duplicate
  vector<int> newPair({s, varIdx});
  listOfAssignVarPairs.push_back(newPair);
}

void PatternKB::addAssignPttSubExpr(STMT_NO s, string varName, string subExpr) {
  VAR_IDX varIdx = varTable->insert(varName);

  if (tableOfSubExpr.count(subExpr) == 0) {
    UNO_SET_OF_STMT_NO newSet({s});
    tableOfSubExpr.insert({subExpr, newSet});
  } else {
    tableOfSubExpr.at(subExpr).insert(s);
  }

  if (tableOfSubExprPtt.count(subExpr) == 0) {
    UNO_SET_OF_STMT_NO newSet({s});
    unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO> newMap({{varIdx, newSet}});
    tableOfSubExprPtt.insert({subExpr, newMap});
  }

  auto curr_map = &tableOfSubExprPtt.at(subExpr);
  if (curr_map->count(varIdx) == 0) {
    // Empty, insert new set
    UNO_SET_OF_STMT_NO newSet({s});
    curr_map->insert({varIdx, newSet});
  } else {
    curr_map->at(varIdx).insert(s);
  }

  vector<int> newPair({s, varIdx});
  if (tableOfSubExprPairs.count(subExpr) == 0) {
    // Insert new set
    vector<vector<int>> newList({newPair});
    tableOfSubExprPairs.insert({subExpr, newList});
  } else {
    tableOfSubExprPairs.at(subExpr).push_back(newPair);
  }

  if (tableOfAssignForVar.count(varIdx) == 0) {
    // Insert new Set
    UNO_SET_OF_STMT_NO newSet({s});
    tableOfAssignForVar.insert({varIdx, newSet});
  } else {
    tableOfAssignForVar.at(varIdx).insert(s);
  }
}

void PatternKB::addIfPtt(STMT_NO s, string varName) {
  VAR_IDX varIdx = varTable->insert(varName);
  if (tableOfIfPtt.count(varIdx) == 0) {
    UNO_SET_OF_STMT_NO newSet({s});
    tableOfIfPtt.insert({varIdx, newSet});
  } else {
    tableOfIfPtt.at(varIdx).insert(s);
  }
  vector<int> newPair({s, varIdx});
  listOfIfVarPairs.push_back(newPair);
}

void PatternKB::addWhilePtt(STMT_NO s, string varName) {
  VAR_IDX varIdx = varTable->insert(varName);
  if (tableOfWhilePtt.count(varIdx) == 0) {
    UNO_SET_OF_STMT_NO newSet({s});
    tableOfWhilePtt.insert({varIdx, newSet});
  } else {
    tableOfWhilePtt.at(varIdx).insert(s);
  }
  vector<int> newPair({s, varIdx});
  listOfWhileVarPairs.push_back(newPair);
}

unordered_set<int> PatternKB::getAssignForFullExpr(string expr) {
  // Get all assignments statements for expr

  // Check table
  if (tableOfFullExpr.count(expr) == 0) {
    // Pattern does not exist, return empty unordered_set
    return unordered_set<int>({});
  }

  return tableOfFullExpr.at(expr);
}

unordered_set<int> PatternKB::getAssignForSubExpr(string subExpr) {
  // Get all assignments statements for sub expr

  // Check table
  if (tableOfSubExpr.count(subExpr) == 0) {
    // Pattern does not exist, return empty unordered_set
    return unordered_set<int>({});
  }

  return tableOfSubExpr.at(subExpr);
}

unordered_set<int> PatternKB::getAssignForVarAndFullExpr(string varName,
                                                         string expr) {
  // Return all statement numbers given expression and varName

  // Check patterns table
  if (tableOfFullExprPtt.count(expr) == 0) {
    // Pattern does not exist, return empty unordered_set
    return unordered_set<int>({});
  }
  // Access inner table
  auto inner_table = tableOfFullExprPtt.at(expr);

  // Check inner table
  VAR_IDX varIdx = varTable->getIndex(varName);
  if (inner_table.count(varIdx) == 0) {
    // No stmt_no? Parser error DEBUG
    return unordered_set<int>({});
  }
  return inner_table.at(varIdx);
}

unordered_set<int> PatternKB::getAssignForVarAndSubExpr(string varName,
                                                        string subExpr) {
  // Return all statement numbers given expression and varName

  // Check patterns table
  if (tableOfSubExprPtt.count(subExpr) == 0) {
    // Pattern does not exist, return empty unordered_set
    return unordered_set<int>({});
  }
  // Access inner table
  auto inner_table = tableOfSubExprPtt.at(subExpr);

  // Check inner table
  VAR_IDX varIdx = varTable->getIndex(varName);
  if (inner_table.count(varIdx) == 0) {
    // No stmt_no? Parser error DEBUG
    return unordered_set<int>({});
  }
  return inner_table.at(varIdx);
}

vector<vector<int>> PatternKB::getAssignVarPairsForFullExpr(string expr) {
  // Get all (STMT_NO, VAR_IDX) given a sub expression
  if (tableOfFullExprPairs.count(expr) == 0) {
    return vector<vector<int>>();
  }

  return tableOfFullExprPairs.at(expr);
}

vector<vector<int>> PatternKB::getAssignVarPairsForSubExpr(string expr) {
  // Get all (STMT_NO, VAR_IDX) given a sub expression
  if (tableOfSubExprPairs.count(expr) == 0) {
    return vector<vector<int>>();
  }

  return tableOfSubExprPairs.at(expr);
}

unordered_set<int> PatternKB::getAssignForVar(string varName) {
  // Return all assignment stt for var, regardless of pattern
  VAR_IDX varIdx = varTable->getIndex(varName);

  // Check table
  if (tableOfAssignForVar.count(varIdx) == 0) {
    // Pattern does not exist, return empty unordered_set
    return unordered_set<int>({});
  }

  return tableOfAssignForVar.at(varIdx);
}

vector<vector<int>> PatternKB::getAssignVarPairs() {
  return listOfAssignVarPairs;
}

unordered_set<int> PatternKB::getIfStmtForVar(string varName) {
  VAR_IDX varIdx = varTable->getIndex(varName);
  if (tableOfIfPtt.count(varIdx) == 0) {
    return UNO_SET_OF_STMT_NO();
  } else {
    return tableOfIfPtt.at(varIdx);
  }
}

vector<vector<int>> PatternKB::getIfStmtVarPairs() { return listOfIfVarPairs; }

unordered_set<int> PatternKB::getWhileStmtForVar(string varName) {
  VAR_IDX varIdx = varTable->getIndex(varName);
  if (tableOfWhilePtt.count(varIdx) == 0) {
    return UNO_SET_OF_STMT_NO();
  } else {
    return tableOfWhilePtt.at(varIdx);
  }
}

vector<vector<int>> PatternKB::getWhileStmtVarPairs() {
  return listOfWhileVarPairs;
}
