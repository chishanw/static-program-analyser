#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int PROC_IDX;
typedef std::string PROC_NAME;

class ProcTable {
 public:
  ProcTable();

  PROC_IDX insertProc(PROC_NAME procName);
  PROC_NAME getProcName(PROC_IDX index);
  PROC_IDX getProcIndex(PROC_NAME procName);
  std::unordered_set<PROC_IDX> getAllProcedures();

 private:
  std::unordered_map<PROC_NAME, PROC_IDX> procNameAsKey;
  std::vector<PROC_NAME> idxAsKey;
  int getSize();
};
