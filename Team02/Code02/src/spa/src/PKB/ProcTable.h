#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int PROC_IDX;

class ProcTable {
 public:
  ProcTable();

  PROC_IDX insertProc(std::string procName);
  std::string getProcName(PROC_IDX index);
  PROC_IDX getProcIndex(std::string procName);
  std::unordered_set<PROC_IDX> getAllProcedures();

 private:
  std::unordered_map<std::string, PROC_IDX> procNameAsKey;
  std::vector<std::string> idxAsKey;
  int getSize();
};
