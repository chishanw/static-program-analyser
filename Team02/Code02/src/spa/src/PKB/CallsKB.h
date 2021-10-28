#pragma once

#include <PKB/ProcTable.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

typedef int STMT_NO;

class CallsKB {
 public:
  explicit CallsKB(ProcTable* procTable);

  // API for DE
  void addCalls(STMT_NO s, PROC_NAME caller, PROC_NAME callee);
  void addCallsT(PROC_NAME caller, PROC_NAME callee);

  // API for QE
  bool isCallStmt(STMT_NO s);
  PROC_IDX getProcCalledByCallStmt(int callStmtNum);
  std::unordered_set<STMT_NO> getAllCallStmts();
  bool isCalls(PROC_NAME caller, PROC_NAME callee);
  std::unordered_set<PROC_IDX> getProcsCalledBy(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getCallerProcs(PROC_NAME proc);
  std::vector<std::pair<PROC_IDX, std::vector<PROC_IDX>>> getAllCallsPairs();

  bool isCallsT(PROC_NAME caller, PROC_NAME callee);
  std::unordered_set<PROC_IDX> getProcsCalledTBy(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getCallerTProcs(PROC_NAME proc);
  std::vector<std::pair<PROC_IDX, std::vector<PROC_IDX>>> getAllCallsTPairs();

 private:
  ProcTable* procTable;
  std::unordered_map<STMT_NO, PROC_IDX> stmtToCallee;
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> table;
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> invTable;
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> tableT;
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> invTableT;
};
