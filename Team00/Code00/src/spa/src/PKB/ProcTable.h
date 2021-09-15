#pragma once

#include <stdio.h>
#include <iostream>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int PROC_IDX;

class ProcTable {
 public:
  ProcTable();

  PROC_IDX insertProc(std::string ProcName);
  std::string getProcName(PROC_IDX index);
  PROC_IDX getProcIndex(std::string ProcName);
  std::unordered_set<PROC_IDX> getAllProcedures();
 private:
  std::unordered_map<std::string, PROC_IDX> procNameAsKey;
  std::vector<std::string> idxAsKey;
  int getSize();  // tbh no clue why this was included in the API...
};
