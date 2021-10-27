#include "AffectsInfoKB.h"

#include "Common/Global.h"

using namespace std;

AffectsInfoKB::AffectsInfoKB(ProcTable* procTable) {
  this->procTable = procTable;
}

// DE Methods
void AffectsInfoKB::addNextStmtForIfStmt(STMT_NO ifStmt,
                                           STMT_NO nextStmtForIfStmt) {
  if (tableOfNextStmtForIfStmts.count(ifStmt) == 0) {
    tableOfNextStmtForIfStmts.insert({ifStmt, nextStmtForIfStmt});
  }
}

void AffectsInfoKB::addFirstStmtOfProc(std::string procName,
                                       STMT_NO firstStmtOfProc) {
  PROC_IDX procIdx = procTable->getProcIndex(procName);
  if (tableOfProcFirstStmts.count(procIdx) == 0) {
    tableOfProcFirstStmts.insert({procIdx, firstStmtOfProc});
  }
}

// QE Methods
STMT_NO AffectsInfoKB::getNextStmtForIfStmt(STMT_NO ifStmt) {
  if (tableOfNextStmtForIfStmts.count(ifStmt) != 0) {
    return tableOfNextStmtForIfStmts.at(ifStmt);
  }
  return -1;
}

vector<STMT_NO> AffectsInfoKB::getFirstStmtOfAllProcs() {
  // TODO(PKB): toposort for AffectsBip later
  vector<STMT_NO> results;
  for (auto procToFirstStmt : tableOfProcFirstStmts) {
    results.push_back(procToFirstStmt.second);
  }
  return results;
}
