#include "VarTable.h"

#include "Common/Global.h"

using namespace std;

VarTable::VarTable() {
  varNameAsKey = unordered_map<string, VarIdx>({});
  idxAsKey = vector<string>({});
}

VarIdx VarTable::insertVar(string varName) {
  try {  // check if variable already exists in table
    VarIdx index = varNameAsKey.at(varName);
    return index;
  }
  catch (const out_of_range& e) {  // new variable
    VarIdx newIndex = getSize();
    varNameAsKey.insert({varName, newIndex});
    idxAsKey.push_back(varName);
    return newIndex;
  }
}

string VarTable::getVarName(VarIdx index) {
  try {
    return idxAsKey.at(index);
  }
  catch (const out_of_range& e) {
    return "";
  }
}

VarIdx VarTable::getVarIndex(string varName) {
  try {
    return varNameAsKey.at(varName);
  }
  catch (const out_of_range& e) {
    return -1;
  }
}

int VarTable::getSize() {
  return idxAsKey.size();
}
