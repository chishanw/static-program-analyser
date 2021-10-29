#include "AffectsInfoKB.h"

#include "Common/Global.h"

using namespace std;

AffectsInfoKB::AffectsInfoKB(Table* procTable) {
  this->procTable = procTable;
}

// DE Methods
void AffectsInfoKB::addNextStmtForIfStmt(STMT_NO ifStmt,
                                         STMT_NO nextStmtForIfStmt) {
  tableOfNextStmtForIfStmts[ifStmt] = nextStmtForIfStmt;
}

void AffectsInfoKB::addFirstStmtOfProc(PROC_NAME procName,
                                       STMT_NO firstStmtOfProc) {
  PROC_IDX procIdx = procTable->insert(procName);
  tableOfProcFirstStmts[procIdx] = firstStmtOfProc;
}

void AffectsInfoKB::addProcCallEdge(PROC_NAME callerProcName,
                                    PROC_NAME calleeProcName) {
  PROC_IDX callerProcIdx = procTable->insert(callerProcName);
  PROC_IDX calleeProcIdx = procTable->insert(calleeProcName);
  if (callGraph.count(callerProcIdx) == 0) {
    callGraph[callerProcIdx] = unordered_set<PROC_IDX>{};
  }
  callGraph[callerProcIdx].insert(calleeProcIdx);
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

unordered_map<PROC_IDX, unordered_set<PROC_IDX>> AffectsInfoKB::getCallGraph() {
  return callGraph;
}
