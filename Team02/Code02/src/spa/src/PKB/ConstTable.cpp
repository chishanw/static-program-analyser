#include "ConstTable.h"

#include "Common/Global.h"

using namespace std;

ConstTable::ConstTable() {
  constAsKey = unordered_map<string, CONST_IDX>({});
  idxAsKey = vector<string>({});
}

CONST_IDX ConstTable::insertConst(string constant) {
  try {  // check if constant already exists in table
    CONST_IDX index = constAsKey.at(constant);
    return index;
  }
  catch (const out_of_range& e) {  // new constant
    CONST_IDX newIndex = getSize();
    constAsKey.insert({constant, newIndex});
    idxAsKey.push_back(constant);
    return newIndex;
  }
}

string ConstTable::getConst(CONST_IDX index) {
  try {
    return idxAsKey.at(index);
  }
  catch (const out_of_range& e) {
    return "";
  }
}

CONST_IDX ConstTable::getConstIndex(string constant) {
  try {
    return constAsKey.at(constant);
  }
  catch (const out_of_range& e) {
    return -1;
  }
}

unordered_set<CONST_IDX> ConstTable::getAllConstants() {
  unordered_set<CONST_IDX> result({});
  for (int i = 0; i < getSize(); i++) {
    result.insert(i);
  }
  return result;
}

int ConstTable::getSize() {
  return idxAsKey.size();
}
