#include "Table.h"

#include "Common/Global.h"

using namespace std;

Table::Table() {
  nameAsKey = unordered_map<string, int>({});
  idxAsKey = vector<string>({});
}

TableElemIdx Table::insert(string element) {
  try {  // check if variable already exists in table
    int index = nameAsKey.at(element);
    return index;
  }
  catch (const out_of_range& e) {  // new variable
    int newIndex = getSize();
    nameAsKey.insert({element, newIndex});
    idxAsKey.push_back(element);
    return newIndex;
  }
}

string Table::getElement(int index) {
  try {
    return idxAsKey.at(index);
  }
  catch (const out_of_range& e) {
    return "";
  }
}

TableElemIdx Table::getIndex(string element) {
  try {
    return nameAsKey.at(element);
  }
  catch (const out_of_range& e) {
    return -1;
  }
}

unordered_set<TableElemIdx> Table::getAllElements() {
  unordered_set<int> result({});
  for (int i = 0; i < getSize(); i++) {
    result.insert(i);
  }
  return result;
}

int Table::getSize() {
  return idxAsKey.size();
}
