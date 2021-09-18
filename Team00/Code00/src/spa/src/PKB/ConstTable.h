#pragma once

#include <stdio.h>
#include <iostream>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int CONST_IDX;

class ConstTable {
 public:
  ConstTable();

  CONST_IDX insertConst(std::string constant);
  std::string getConst(CONST_IDX index);
  CONST_IDX getConstIndex(std::string constant);
  std::unordered_set<CONST_IDX> getAllConstants();
 private:
  std::unordered_map<std::string, CONST_IDX> constAsKey;
  std::vector<std::string> idxAsKey;
  int getSize();
};
