#include "PKB.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void PKB::addStmt(DesignEntity de, StmtNo s) {
  if (tableOfStmts.count(DesignEntity::STATEMENT) == 0) {
    tableOfStmts[DesignEntity::STATEMENT] = SetOfStmts();
  }

  if (tableOfStmts.count(de) == 0) {
    tableOfStmts[de] = SetOfStmts();
  }

  tableOfStmts.at(DesignEntity::STATEMENT).insert(s);
  tableOfStmts.at(de).insert(s);
}

SetOfStmts PKB::getAllStmts(DesignEntity de) {
  if (tableOfStmts.count(de) == 0) {
    return SetOfStmts();
  }

  return tableOfStmts.at(de);
}

bool PKB::isStmt(DesignEntity de, StmtNo s) {
  if (tableOfStmts.count(de) == 0) {
    return false;
  }

  return tableOfStmts.at(de).count(s) > 0;
}

int PKB::getNumStmts(DesignEntity de) {
  if (tableOfStmts.count(de) == 0) {
    return 0;
  }

  return tableOfStmts.at(de).size();
}

void insertToTableRs(TablesRs* tablesRs, RelationshipType rs, int left,
                     int right) {
  if ((*tablesRs).count(rs) == 0) {
    (*tablesRs)[rs] = unordered_map<int, unordered_set<int>>{};
  }

  if ((*tablesRs)[rs].count(left) == 0) {
    (*tablesRs)[rs][left] = unordered_set<int>{};
  }

  (*tablesRs)[rs][left].insert(right);
}

void insertToMappings(MappingsRs* mappingsRs, RelationshipType rs, int left,
                      int right) {
  if ((*mappingsRs).count(rs) == 0) {
    unordered_map<ParamPosition, SetOfStmtLists> newMap(
        {{ParamPosition::LEFT, SetOfStmtLists()},
         {ParamPosition::RIGHT, SetOfStmtLists()},
         {ParamPosition::BOTH, SetOfStmtLists()}});
    (*mappingsRs)[rs] = newMap;
  }

  (*mappingsRs)[rs][ParamPosition::LEFT].insert(vector<int>({left}));
  (*mappingsRs)[rs][ParamPosition::RIGHT].insert(vector<int>({right}));
  (*mappingsRs)[rs][ParamPosition::BOTH].insert(vector<int>({left, right}));
}

void PKB::addRs(RelationshipType rs, int left, int right) {
  insertToTableRs(&tablesRs, rs, left, right);
  insertToTableRs(&invTablesRs, rs, right, left);
  insertToMappings(&mappingsRs, rs, left, right);
}

void PKB::addRs(RelationshipType rs, int left, TableType rightType,
                string right) {
  int rightIndex = insertAt(rightType, right);
  addRs(rs, left, rightIndex);
}

void PKB::addRs(RelationshipType rs, TableType leftType, string left,
                TableType rightType, string right) {
  int leftIndex = insertAt(leftType, left);
  int rightIndex = insertAt(rightType, right);
  addRs(rs, leftIndex, rightIndex);
}

bool PKB::isRs(RelationshipType rs, int left, int right) {
  return getRight(rs, left).count(right) > 0;
}

bool PKB::isRs(RelationshipType rs, int left, TableType rightType,
               string right) {
  int rightIndex = getIndexOf(rightType, right);
  return isRs(rs, left, rightIndex);
}

bool PKB::isRs(RelationshipType rs, TableType leftType, string left,
               TableType rightType, string right) {
  int leftIndex = getIndexOf(leftType, left);
  int rightIndex = getIndexOf(rightType, right);
  return isRs(rs, leftIndex, rightIndex);
}

unordered_set<int> PKB::getRight(RelationshipType rs, int left) {
  if (tablesRs.count(rs) == 0) {
    return SetOfStmts();
  }

  if (tablesRs.at(rs).count(left) == 0) {
    return SetOfStmts();
  }

  return tablesRs.at(rs).at(left);
}

unordered_set<int> PKB::getRight(RelationshipType rs, TableType leftType,
                                 string left) {
  int leftIndex = getIndexOf(leftType, left);
  return getRight(rs, leftIndex);
}

unordered_set<int> PKB::getLeft(RelationshipType rs, int right) {
  if (invTablesRs.count(rs) == 0) {
    return SetOfStmts();
  }

  if (invTablesRs.at(rs).count(right) == 0) {
    return SetOfStmts();
  }
  return invTablesRs.at(rs).at(right);
}

unordered_set<int> PKB::getLeft(RelationshipType rs, TableType rightType,
                                string right) {
  int rightIndex = getIndexOf(rightType, right);
  return getLeft(rs, rightIndex);
}

SetOfStmtLists PKB::getMappings(RelationshipType rs, ParamPosition param) {
  if (mappingsRs.count(rs) == 0) {
    return SetOfStmtLists();
  }
  return mappingsRs.at(rs).at(param);
}

// Pattern API
void PKB::addAssignPttFullExpr(StmtNo s, string var, string expr) {
  patternKB.addAssignPttFullExpr(s, var, expr);
}
void PKB::addAssignPttSubExpr(StmtNo s, string var, string expr) {
  patternKB.addAssignPttSubExpr(s, var, expr);
}
unordered_set<int> PKB::getAssignForFullExpr(string expr) {
  return patternKB.getAssignForFullExpr(expr);
}
unordered_set<int> PKB::getAssignForSubExpr(string expr) {
  return patternKB.getAssignForSubExpr(expr);
}
unordered_set<int> PKB::getAssignForVarAndFullExpr(string varName,
                                                   string expr) {
  return patternKB.getAssignForVarAndFullExpr(varName, expr);
}
unordered_set<int> PKB::getAssignForVarAndSubExpr(string varName,
                                                  string subExpr) {
  return patternKB.getAssignForVarAndSubExpr(varName, subExpr);
}
vector<vector<int>> PKB::getAssignVarPairsForFullExpr(string expr) {
  return patternKB.getAssignVarPairsForFullExpr(expr);
}
vector<vector<int>> PKB::getAssignVarPairsForSubExpr(string subExpr) {
  return patternKB.getAssignVarPairsForSubExpr(subExpr);
}
unordered_set<int> PKB::getAssignForVar(string varName) {
  return patternKB.getAssignForVar(varName);
}
vector<vector<int>> PKB::getAssignVarPairs() {
  return patternKB.getAssignVarPairs();
}
void PKB::addIfPtt(StmtNo s, string varName) { patternKB.addIfPtt(s, varName); }

void PKB::addWhilePtt(StmtNo s, string varName) {
  patternKB.addWhilePtt(s, varName);
}

unordered_set<int> PKB::getIfStmtForVar(string varName) {
  return patternKB.getIfStmtForVar(varName);
}

vector<vector<int>> PKB::getIfStmtVarPairs() {
  return patternKB.getIfStmtVarPairs();
}

unordered_set<int> PKB::getWhileStmtForVar(string varName) {
  return patternKB.getWhileStmtForVar(varName);
}

vector<vector<int>> PKB::getWhileStmtVarPairs() {
  return patternKB.getWhileStmtVarPairs();
}

// Calls API
void PKB::addCalls(StmtNo s, PROC_NAME caller, PROC_NAME callee) {
  addStmt(DesignEntity::STATEMENT, s);
  callsKB.addCalls(s, caller, callee);
}
void PKB::addCallsT(PROC_NAME caller, PROC_NAME callee) {
  callsKB.addCallsT(caller, callee);
}
bool PKB::isCalls(PROC_NAME caller, PROC_NAME callee) {
  return callsKB.isCalls(caller, callee);
}
unordered_set<PROC_IDX> PKB::getProcsCalledBy(PROC_NAME proc) {
  return callsKB.getProcsCalledBy(proc);
}
unordered_set<PROC_IDX> PKB::getCallerProcs(PROC_NAME proc) {
  return callsKB.getCallerProcs(proc);
}
vector<pair<PROC_IDX, vector<PROC_IDX>>> PKB::getAllCallsPairs() {
  return callsKB.getAllCallsPairs();
}
PROC_IDX PKB::getProcCalledByCallStmt(int callStmtNum) {
  return callsKB.getProcCalledByCallStmt(callStmtNum);
}
bool PKB::isCallsT(PROC_NAME caller, PROC_NAME callee) {
  return callsKB.isCallsT(caller, callee);
}
unordered_set<PROC_IDX> PKB::getProcsCalledTBy(PROC_NAME proc) {
  return callsKB.getProcsCalledTBy(proc);
}
unordered_set<PROC_IDX> PKB::getCallerTProcs(PROC_NAME proc) {
  return callsKB.getCallerTProcs(proc);
}
vector<pair<PROC_IDX, vector<PROC_IDX>>> PKB::getAllCallsTPairs() {
  return callsKB.getAllCallsTPairs();
}
SetOfStmts PKB::getAllCallStmts() { return callsKB.getAllCallStmts(); }
bool PKB::isCallStmt(StmtNo s) { return callsKB.isCallStmt(s); }

// Affects Info API
void PKB::addNextStmtForIfStmt(StmtNo ifStmt, StmtNo nextStmtForIfStmt) {
  affectsInfoKB.addNextStmtForIfStmt(ifStmt, nextStmtForIfStmt);
}
void PKB::addFirstStmtOfProc(string procName, StmtNo firstStmtOfProc) {
  affectsInfoKB.addFirstStmtOfProc(procName, firstStmtOfProc);
}
void PKB::addProcCallEdge(PROC_NAME callerProcName, PROC_NAME calleeProcName) {
  affectsInfoKB.addProcCallEdge(callerProcName, calleeProcName);
}

StmtNo PKB::getNextStmtForIfStmt(StmtNo ifStmt) {
  return affectsInfoKB.getNextStmtForIfStmt(ifStmt);
}
vector<StmtNo> PKB::getFirstStmtOfAllProcs() {
  return affectsInfoKB.getFirstStmtOfAllProcs();
}
unordered_map<PROC_IDX, unordered_set<PROC_IDX>> PKB::getCallGraph() {
  return affectsInfoKB.getCallGraph();
}

// Table API
TableElemIdx PKB::insertAt(TableType type, string element) {
  return tables.at(type).insert(element);
}
string PKB::getElementAt(TableType type, TableElemIdx index) {
  return tables.at(type).getElement(index);
}
TableElemIdx PKB::getIndexOf(TableType type, string element) {
  return tables.at(type).getIndex(element);
}
unordered_set<TableElemIdx> PKB::getAllElementsAt(TableType type) {
  return tables.at(type).getAllElements();
}
