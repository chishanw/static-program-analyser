#include "VarTable.h"

#include "Common/Global.h"

using namespace std;

VarTable::VarTable() {
  varNameAsKey = unordered_map<string, VAR_IDX>({});
  idxAsKey = vector<string>({});
}

VAR_IDX VarTable::insertVar(string varName) {
  try {  // check if variable already exists in table
    VAR_IDX index = varNameAsKey.at(varName);
    return index;
  }
  catch (const out_of_range& e) {  // new variable
    VAR_IDX newIndex = getSize();
    varNameAsKey.insert({varName, newIndex});
    idxAsKey.push_back(varName);
    return newIndex;
  }
}

string VarTable::getVarName(VAR_IDX index) {
  try {
    return idxAsKey.at(index);
  }
  catch (const out_of_range& e) {
    return "";
  }
}

VAR_IDX VarTable::getVarIndex(string varName) {
  try {
    return varNameAsKey.at(varName);
  }
  catch (const out_of_range& e) {
    return -1;
  }
}

unordered_set<VAR_IDX> VarTable::getAllVariables() {
  unordered_set<VAR_IDX> result({});
  for (int i = 0; i < getSize(); i++) {
    result.insert(i);
  }
  return result;
}

int VarTable::getSize() {
  return idxAsKey.size();
}
