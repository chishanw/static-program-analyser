#include "UsesKB.h"
#include "Common/Global.h"

using namespace std;

UsesKB::UsesKB(VarTable* varTable) {
  this->varTable = varTable;
  usesTableByStmt = unordered_map<STMT_NO, unordered_set<VAR_IDX>>({});
  usesTableByVar = unordered_map<VAR_IDX, unordered_set<STMT_NO>>({});
}

void UsesKB::addUsesS(STMT_NO s, VAR_NAME var) {
  VAR_IDX varIdx = varTable->insertVar(var);
  try {  // stmt s already exists in table
    usesTableByStmt.at(s).insert(varIdx);
  }
  catch (const out_of_range& e) {
    unordered_set<VAR_IDX> newSet({});
    newSet.insert(varIdx);
    usesTableByStmt.insert({s, newSet});
  }
  try {  // variable var already exists in table
    usesTableByVar.at(varIdx).insert(s);
  }
  catch (const out_of_range& e) {
    unordered_set<STMT_NO> newSet({});
    newSet.insert(s);
    usesTableByVar.insert({varIdx, newSet});
  }
}

bool UsesKB::isUsesS(STMT_NO s, VAR_NAME v) {
  try {
    unordered_set<VAR_IDX> setAtS = usesTableByStmt.at(s);
    return setAtS.count(varTable->getVarIndex(v));
  }
  catch (const out_of_range& e) {
    return false;
  }
}

unordered_set<VAR_IDX> UsesKB::getVarsUsedS(STMT_NO s) {
  try {
    return usesTableByStmt.at(s);
  }
  catch (const out_of_range& e) {
      unordered_set<int> emptySet({});
    return emptySet;
  }
}

unordered_set<STMT_NO> UsesKB::getUsesS(VAR_NAME v) {
  VAR_IDX varIdx = varTable->getVarIndex(v);
  try {
    return usesTableByVar.at(varIdx);
  }
  catch (const out_of_range& e) {
    unordered_set<int> emptySet({});
    return emptySet;
  }
}

vector<pair<STMT_NO, vector<VAR_IDX>>> UsesKB::getAllUsesSPairs() {
  vector<pair<STMT_NO, vector<VAR_IDX>>> result;
  for (auto it = usesTableByStmt.begin(); it != usesTableByStmt.end(); it++) {
    STMT_NO s = it->first;
    unordered_set<VAR_IDX> allVars = it->second;
    vector<int> allVarsVector = vector(allVars.begin(), allVars.end());
    result.push_back(pair(s, allVarsVector));
  }
  return result;
}
