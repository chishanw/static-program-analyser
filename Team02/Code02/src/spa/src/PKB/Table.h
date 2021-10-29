#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int TABLE_ELEM_IDX;

class Table {
 public:
  Table();

  TABLE_ELEM_IDX insert(std::string element);
  std::string getElement(int index);
  TABLE_ELEM_IDX getIndex(std::string element);
  std::unordered_set<TABLE_ELEM_IDX> getAllElements();

 protected:
  std::unordered_map<std::string, TABLE_ELEM_IDX> nameAsKey;
  std::vector<std::string> idxAsKey;
  int getSize();
};
