#pragma once

#include <PKB/Table.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int STMT_NO, PROC_IDX;
typedef std::string PROC_NAME;

class AffectsInfoKB {
 public:
  // Constructor
  explicit AffectsInfoKB(Table* procTable);

  // Methods for DE
  void addNextStmtForIfStmt(STMT_NO ifStmt, STMT_NO nextStmtForIfStmt);
  void addFirstStmtOfProc(PROC_NAME procName, STMT_NO firstStmtOfProc);
  void addProcCallEdge(PROC_NAME callerProcName, PROC_NAME calleeProcName);

  // Methods for QE
  STMT_NO getNextStmtForIfStmt(STMT_NO ifStmt);
  std::vector<STMT_NO> getFirstStmtOfAllProcs();
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> getCallGraph();

 private:
  Table* procTable;
  std::unordered_map<PROC_IDX, STMT_NO> tableOfProcFirstStmts;
  std::unordered_map<STMT_NO, STMT_NO> tableOfNextStmtForIfStmts;
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> callGraph;
};
