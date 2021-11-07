#include "PKB.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void PKB::addStmt(DesignEntity de, StmtNo s) {
  tableOfStmts[DesignEntity::STATEMENT].insert(s);
  tableOfStmts[DesignEntity::PROG_LINE].insert(s);
  tableOfStmts[de].insert(s);
}

SetOfStmts PKB::getAllStmts(DesignEntity de) { return tableOfStmts[de]; }

bool PKB::isStmt(DesignEntity de, StmtNo s) {
  return tableOfStmts[de].find(s) != tableOfStmts[de].end();
}

int PKB::getNumEntity(DesignEntity de) {
  switch (de) {
    case DesignEntity::STATEMENT:
    case DesignEntity::READ:
    case DesignEntity::PRINT:
    case DesignEntity::CALL:
    case DesignEntity::WHILE:
    case DesignEntity::IF:
    case DesignEntity::ASSIGN:
    case DesignEntity::PROG_LINE:
      return tableOfStmts[de].size();
    case DesignEntity::VARIABLE:
      return tables.at(TableType::VAR_TABLE).getSize();
    case DesignEntity::CONSTANT:
      return tables.at(TableType::CONST_TABLE).getSize();
    case DesignEntity::PROCEDURE:
      return tables.at(TableType::PROC_TABLE).getSize();
  }
}

void insertToTableRs(TablesRs* tablesRs, RelationshipType rs, int left,
                     int right) {
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

unordered_set<int> getValue(TablesRs* tables, RelationshipType rs, int key) {
  return (*tables)[rs][key];
}

bool PKB::hasRight(RelationshipType rs, int left) {
  return !tablesRs[rs][left].empty();
}

bool PKB::hasRight(RelationshipType rs, TableType leftType, std::string left) {
  int leftIndex = getIndexOf(leftType, left);
  return hasRight(rs, leftIndex);
}

unordered_set<int> PKB::getRight(RelationshipType rs, int left) {
  return tablesRs[rs][left];
}

unordered_set<int> PKB::getRight(RelationshipType rs, TableType leftType,
                                 string left) {
  int leftIndex = getIndexOf(leftType, left);
  return getRight(rs, leftIndex);
}

unordered_set<int> PKB::getLeft(RelationshipType rs, int right) {
  return invTablesRs[rs][right];
}

unordered_set<int> PKB::getLeft(RelationshipType rs, TableType rightType,
                                string right) {
  int rightIndex = getIndexOf(rightType, right);
  return getLeft(rs, rightIndex);
}

SetOfStmtLists PKB::getMappings(RelationshipType rs, ParamPosition param) {
  return mappingsRs[rs][param];
}

// New Pattern API
void PKB::addPatternRs(RelationshipType rs, StmtNo stmtNo, string varName) {
  int varIndex = insertAt(TableType::VAR_TABLE, varName);
  insertToTableRs(&tablesRs, rs, varIndex, stmtNo);
  insertToMappings(&mappingsRs, rs, stmtNo, varIndex);
}

void PKB::addPatternRs(RelationshipType rs, StmtNo stmtNo, string varName,
                       string expr) {
  addPatternRs(rs, stmtNo, varName);

  int exprIndex = insertAt(TableType::EXPR_TABLE, expr);
  int varIndex = getIndexOf(TableType::VAR_TABLE, varName);

  insertToTableRs(&tablesExpr, rs, exprIndex, varIndex);

  // Insert to Special Mappings Table
  ListOfInts newList({stmtNo, varIndex});
  mappingsForExpr[rs][exprIndex].insert(newList);

  // Insert to Special Expr and Var Table
  pair newPair(varIndex, exprIndex);
  tablesPttRs[rs][newPair].insert(stmtNo);
}

bool PKB::isPatternRs(RelationshipType rs, StmtNo stmtno, int varIndex,
                      string expr) {
  int exprIndex = getIndexOf(TableType::EXPR_TABLE, expr);
  auto key = pair(varIndex, exprIndex);
  return tablesPttRs[rs][key].count(stmtno) != 0;
}

bool PKB::isPatternRs(RelationshipType rs, StmtNo stmtno, int varIndex) {
  return isRs(rs, varIndex, stmtno);
}

SetOfStmts PKB::getStmtsForVarAndExpr(RelationshipType rs, int varIndex,
                                      string expr) {
  int exprIndex = getIndexOf(TableType::EXPR_TABLE, expr);
  pair newPair(varIndex, exprIndex);
  return tablesPttRs[rs][newPair];
}

SetOfStmts PKB::getStmtsForVar(RelationshipType rs, int varIndex) {
  return getRight(rs, varIndex);
}

SetOfStmts PKB::getVarsForExpr(RelationshipType rs, std::string expr) {
  int exprIndex = getIndexOf(TableType::EXPR_TABLE, expr);
  return getValue(&tablesExpr, rs, exprIndex);
}

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
