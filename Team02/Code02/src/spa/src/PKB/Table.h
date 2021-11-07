#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int TableElemIdx;

class Table {
 public:
  Table();

  TableElemIdx insert(std::string element);
  std::string getElement(int index);
  TableElemIdx getIndex(std::string element);
  std::unordered_set<TableElemIdx> getAllElements();
  int getSize();

 protected:
  std::unordered_map<std::string, TableElemIdx> nameAsKey;
  std::vector<std::string> idxAsKey;
  std::unordered_set<TableElemIdx> allElemIdx;
};
