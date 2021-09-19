#include "ProcTable.h"

#include "Common/Global.h"

using namespace std;

ProcTable::ProcTable() {
  procNameAsKey = unordered_map<string, PROC_IDX>({});
  idxAsKey = vector<string>({});
}

PROC_IDX ProcTable::insertProc(string procName) {
  try {  // check if procedure already exists in table
    PROC_IDX index = procNameAsKey.at(procName);
    return index;
  }
  catch (const out_of_range& e) {  // new procedure
    PROC_IDX newIndex = getSize();
    procNameAsKey.insert({procName, newIndex});
    idxAsKey.push_back(procName);
    return newIndex;
  }
}

string ProcTable::getProcName(PROC_IDX index) {
  try {
    return idxAsKey.at(index);
  }
  catch (const out_of_range& e) {
    return "";
  }
}

PROC_IDX ProcTable::getProcIndex(string procName) {
  try {
    return procNameAsKey.at(procName);
  }
  catch (const out_of_range& e) {
    return -1;
  }
}

unordered_set<PROC_IDX> ProcTable::getAllProcedures() {
  unordered_set<PROC_IDX> result({});
  for (int i = 0; i < getSize(); i++) {
    result.insert(i);
  }
  return result;
}

int ProcTable::getSize() {
  return idxAsKey.size();
}
