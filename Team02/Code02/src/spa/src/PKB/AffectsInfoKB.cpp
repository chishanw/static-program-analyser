#include "AffectsInfoKB.h"

#include "Common/Global.h"

using namespace std;

AffectsInfoKB::AffectsInfoKB(Table* procTable) {
  this->procTable = procTable;
}

// DE Methods
void AffectsInfoKB::addNextStmtForIfStmt(StmtNo ifStmt,
                                         StmtNo nextStmtForIfStmt) {
  tableOfNextStmtForIfStmts[ifStmt] = nextStmtForIfStmt;
}

void AffectsInfoKB::addFirstStmtOfProc(ProcName procName,
                                       StmtNo firstStmtOfProc) {
  ProcIdx procIdx = procTable->insert(procName);
  tableOfProcFirstStmts[procIdx] = firstStmtOfProc;
}

void AffectsInfoKB::addProcCallEdge(ProcName callerProcName,
                                    ProcName calleeProcName) {
  ProcIdx callerProcIdx = procTable->insert(callerProcName);
  ProcIdx calleeProcIdx = procTable->insert(calleeProcName);
  if (callGraph.count(callerProcIdx) == 0) {
    callGraph[callerProcIdx] = unordered_set<ProcIdx>{};
  }
  callGraph[callerProcIdx].insert(calleeProcIdx);
}

// QE Methods
StmtNo AffectsInfoKB::getNextStmtForIfStmt(StmtNo ifStmt) {
  if (tableOfNextStmtForIfStmts.count(ifStmt) != 0) {
    return tableOfNextStmtForIfStmts.at(ifStmt);
  }
  return -1;
}

vector<StmtNo> AffectsInfoKB::getFirstStmtOfAllProcs() {
  vector<StmtNo> results;
  for (auto procToFirstStmt : tableOfProcFirstStmts) {
    results.push_back(procToFirstStmt.second);
  }
  return results;
}

unordered_map<ProcIdx, unordered_set<ProcIdx>> AffectsInfoKB::getCallGraph() {
  return callGraph;
}
