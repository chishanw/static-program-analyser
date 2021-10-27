#pragma once

#include <PKB/ProcTable.h>

#include <string>
#include <unordered_map>
#include <vector>

typedef int STMT_NO, PROC_IDX;

class AffectsInfoKB {
 public:
  // Constructor
  explicit AffectsInfoKB(ProcTable* procTable);

  // Methods for DE
  void addNextStmtForIfStmt(STMT_NO ifStmt, STMT_NO nextStmtForIfStmt);
  void addFirstStmtOfProc(std::string procName, STMT_NO firstStmtOfProc);

  // Methods for QE
  STMT_NO getNextStmtForIfStmt(STMT_NO ifStmt);
  std::vector<STMT_NO> getFirstStmtOfAllProcs();

 private:
  ProcTable* procTable;
  std::unordered_map<PROC_IDX, STMT_NO> tableOfProcFirstStmts;
  std::unordered_map<STMT_NO, STMT_NO> tableOfNextStmtForIfStmts;
};
