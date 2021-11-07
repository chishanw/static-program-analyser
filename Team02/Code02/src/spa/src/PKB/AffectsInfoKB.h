#pragma once

#include <Common/Common.h>
#include <PKB/Table.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class AffectsInfoKB {
 public:
  // Constructor
  explicit AffectsInfoKB(Table* procTable);

  // Methods for DE
  void addNextStmtForIfStmt(StmtNo ifStmt, StmtNo nextStmtForIfStmt);
  void addFirstStmtOfProc(ProcName procName, StmtNo firstStmtOfProc);
  void addProcCallEdge(ProcName callerProcName, ProcName calleeProcName);

  // Methods for QE
  StmtNo getNextStmtForIfStmt(StmtNo ifStmt);
  std::vector<StmtNo> getFirstStmtOfAllProcs();
  std::unordered_map<ProcIdx, std::unordered_set<ProcIdx>> getCallGraph();

 private:
  Table* procTable;
  std::unordered_map<ProcIdx, StmtNo> tableOfProcFirstStmts;
  std::unordered_map<StmtNo, StmtNo> tableOfNextStmtForIfStmts;
  std::unordered_map<ProcIdx, std::unordered_set<ProcIdx>> callGraph;
};
