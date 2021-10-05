#include "ModifiesKB.h"
#include "Common/Global.h"

using namespace std;

ModifiesKB::ModifiesKB(VarTable* varTable, ProcTable* procTable) {
  this->varTable = varTable;
  this->procTable = procTable;
  tableS = unordered_map<STMT_NO, unordered_set<VAR_IDX>>({});
  invTableS = unordered_map<VAR_IDX, unordered_set<STMT_NO>>({});
  tableP = unordered_map<STMT_NO, unordered_set<VAR_IDX>>({});
  invTableP = unordered_map<VAR_IDX, unordered_set<STMT_NO>>({});
}

void ModifiesKB::addModifiesS(STMT_NO s, VAR_NAME var) {
  VAR_IDX varIdx = varTable->insertVar(var);
  try {  // stmt s already exists in table
    tableS.at(s).insert(varIdx);
  }
  catch (const out_of_range& e) {
    unordered_set<VAR_IDX> newSet({});
    newSet.insert(varIdx);
    tableS.insert({ s, newSet });
  }
  try {  // variable var already exists in table
    invTableS.at(varIdx).insert(s);
  }
  catch (const out_of_range& e) {
    unordered_set<STMT_NO> newSet({});
    newSet.insert(s);
    invTableS.insert({ varIdx, newSet });
  }
}

void ModifiesKB::addModifiesP(PROC_NAME proc, VAR_NAME var) {
  VAR_IDX varIdx = varTable->insertVar(var);
  PROC_IDX p = procTable->insertProc(proc);
  try {
    tableP.at(p).insert(varIdx);
  } catch (const out_of_range& e) {
    unordered_set<VAR_IDX> newSet({});
    newSet.insert(varIdx);
    tableP.insert({p, newSet});
  }
  try {
    invTableP.at(varIdx).insert(p);
  } catch (const out_of_range& e) {
    unordered_set<STMT_NO> newSet({});
    newSet.insert(p);
    invTableP.insert({varIdx, newSet});
  }
}

bool ModifiesKB::isModifiesS(STMT_NO s, VAR_NAME v) {
  try {
    unordered_set<VAR_IDX> setAtS = tableS.at(s);
    return setAtS.count(varTable->getVarIndex(v));
  }
  catch (const out_of_range& e) {
    return false;
  }
}

bool ModifiesKB::isModifiesP(PROC_NAME proc, VAR_NAME v) {
  try {
    PROC_IDX p = procTable->getProcIndex(proc);
    unordered_set<VAR_IDX> setAtP = tableP.at(p);
    return setAtP.count(varTable->getVarIndex(v));
  } catch (const out_of_range& e) {
    return false;
  }
}

unordered_set<VAR_IDX> ModifiesKB::getVarsModifiedS(STMT_NO s) {
  try {
    return tableS.at(s);
  }
  catch (const out_of_range& e) {
    unordered_set<int> emptySet({});
    return emptySet;
  }
}

unordered_set<VAR_IDX> ModifiesKB::getVarsModifiedP(PROC_NAME proc) {
  try {
    PROC_IDX p = procTable->getProcIndex(proc);
    return tableP.at(p);
  } catch (const out_of_range& e) {
    unordered_set<int> emptySet({});
    return emptySet;
  }
}

unordered_set<STMT_NO> ModifiesKB::getModifiesS(VAR_NAME v) {
  VAR_IDX varIdx = varTable->getVarIndex(v);
  try {
    return invTableS.at(varIdx);
  }
  catch (const out_of_range& e) {
    unordered_set<int> emptySet({});
    return emptySet;
  }
}

unordered_set<STMT_NO> ModifiesKB::getModifiesP(VAR_NAME v) {
  VAR_IDX varIdx = varTable->getVarIndex(v);
  try {
    return invTableP.at(varIdx);
  } catch (const out_of_range& e) {
    unordered_set<int> emptySet({});
    return emptySet;
  }
}

vector<pair<int, vector<int>>> ModifiesKB::getAllModifiesSPairs() {
  vector<pair<int, vector<int>>> result;
  for (auto it = tableS.begin(); it != tableS.end();
      it++) {
    STMT_NO s = it->first;
    unordered_set<VAR_IDX> allVars = it->second;
    vector<int> allVarsVector = vector(allVars.begin(), allVars.end());
    result.push_back(pair(s, allVarsVector));
  }
  return result;
}

vector<pair<PROC_IDX, vector<VAR_IDX>>> ModifiesKB::getAllModifiesPPairs() {
  vector<pair<int, vector<int>>> result;
  for (auto it = tableP.begin(); it != tableP.end(); it++) {
    PROC_IDX s = it->first;
    unordered_set<VAR_IDX> allVars = it->second;
    vector<int> allVarsVector = vector(allVars.begin(), allVars.end());
    result.push_back(pair(s, allVarsVector));
  }
  return result;
}
