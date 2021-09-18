#pragma once

#include <stdio.h>
#include <iostream>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int VAR_IDX;

class VarTable {
 public:
  VarTable();

  VAR_IDX insertVar(std::string varName);
  std::string getVarName(VAR_IDX index);
  VAR_IDX getVarIndex(std::string varName);
  std::unordered_set<VAR_IDX> getAllVariables();
 private:
  std::unordered_map<std::string, VAR_IDX> varNameAsKey;
  std::vector<std::string> idxAsKey;
  int getSize();
};
