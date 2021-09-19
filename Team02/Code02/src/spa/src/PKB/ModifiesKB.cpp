#include "ModifiesKB.h"
#include "Common/Global.h"

using namespace std;

ModifiesKB::ModifiesKB(VarTable* varTable) {
  this->varTable = varTable;
  modifiesTableByStmt = unordered_map<STMT_NO, unordered_set<VAR_IDX>>({});
  modifiesTableByVar = unordered_map<VAR_IDX, unordered_set<STMT_NO>>({});
}

void ModifiesKB::addModifiesS(STMT_NO s, VAR_NAME var) {
  VAR_IDX varIdx = varTable->insertVar(var);
  try {  // stmt s already exists in table
    modifiesTableByStmt.at(s).insert(varIdx);
  }
  catch (const out_of_range& e) {
    unordered_set<VAR_IDX> newSet({});
    newSet.insert(varIdx);
    modifiesTableByStmt.insert({ s, newSet });
  }
  try {  // variable var already exists in table
    modifiesTableByVar.at(varIdx).insert(s);
  }
  catch (const out_of_range& e) {
    unordered_set<STMT_NO> newSet({});
    newSet.insert(s);
    modifiesTableByVar.insert({ varIdx, newSet });
  }
}

bool ModifiesKB::isModifiesS(STMT_NO s, VAR_NAME v) {
  try {
    unordered_set<VAR_IDX> setAtS = modifiesTableByStmt.at(s);
    return setAtS.count(varTable->getVarIndex(v));
  }
  catch (const out_of_range& e) {
    return false;
  }
}

unordered_set<VAR_IDX> ModifiesKB::getVarsModifiedS(STMT_NO s) {
  try {
    return modifiesTableByStmt.at(s);
  }
  catch (const out_of_range& e) {
    unordered_set<int> emptySet({});
    return emptySet;
  }
}

unordered_set<STMT_NO> ModifiesKB::getModifiesS(VAR_NAME v) {
  VAR_IDX varIdx = varTable->getVarIndex(v);
  try {
    return modifiesTableByVar.at(varIdx);
  }
  catch (const out_of_range& e) {
    unordered_set<int> emptySet({});
    return emptySet;
  }
}

vector<pair<int, vector<int>>> ModifiesKB::getAllModifiesSPairs() {
  vector<pair<int, vector<int>>> result;
  for (auto it = modifiesTableByStmt.begin(); it != modifiesTableByStmt.end();
      it++) {
    STMT_NO s = it->first;
    unordered_set<VAR_IDX> allVars = it->second;
    vector<int> allVarsVector = vector(allVars.begin(), allVars.end());
    result.push_back(pair(s, allVarsVector));
  }
  return result;
}
