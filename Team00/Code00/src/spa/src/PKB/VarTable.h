#pragma once

#include <stdio.h>
#include <iostream>

#include <string>
#include <unordered_map>
#include <vector>

typedef int VarIdx;

class VarTable {
 public:
  VarTable();

  VarIdx insertVar(std::string varName);
  std::string getVarName(VarIdx index);
  VarIdx getVarIndex(std::string varName);
 private:
  std::unordered_map<std::string, VarIdx> varNameAsKey;
  std::vector<std::string> idxAsKey;
  int getSize();  // tbh no clue why this was included in the API...
};
