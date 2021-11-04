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

unordered_set<int> getValue(TablesRs* tables, RelationshipType rs, int key) {
  if ((*tables).count(rs) == 0) {
    return unordered_set<int>({});
  }

  if ((*tables)[rs].count(key) == 0) {
    return unordered_set<int>({});
  }

  return (*tables)[rs][key];
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

  insertToTableRs(&tablesExpr, rs, exprIndex, stmtNo);

  // Insert to Special Mappings Table
  if (mappingsForExpr.count(rs) == 0) {
    // insert new map
    unordered_map<int, SetOfIntLists> newMap({});
    mappingsForExpr.insert({rs, newMap});
  }

  if (mappingsForExpr[rs].count(exprIndex) == 0) {
    SetOfIntLists newSet({});
    mappingsForExpr[rs].insert({exprIndex, newSet});
  }

  ListOfInts newList({stmtNo, varIndex});
  mappingsForExpr[rs][exprIndex].insert(newList);

  // Insert to Special Expr and Var Table
  if (tablesPttRs.count(rs) == 0) {
    // insert new map
    unordered_map<pair<ExprIdx, VarIdx>, SetOfStmts, PairHash> newMap({});
    tablesPttRs.insert({rs, newMap});
  }
  pair newPair(varIndex, exprIndex);
  if (tablesPttRs[rs].count(newPair) == 0) {
    SetOfStmts newSet({});
    tablesPttRs[rs].insert({newPair, newSet});
  }
  tablesPttRs[rs][newPair].insert(stmtNo);
}

SetOfStmts PKB::getStmtsForVarAndExpr(RelationshipType rs, string varName,
                                      string expr) {
  int varIndex = getIndexOf(TableType::VAR_TABLE, varName);
  int exprIndex = getIndexOf(TableType::EXPR_TABLE, expr);
  if (tablesPttRs.count(rs) == 0) {
    return SetOfStmts({});
  }

  pair newPair(varIndex, exprIndex);
  if (tablesPttRs[rs].count(newPair) == 0) {
    return SetOfStmts({});
  }

  return tablesPttRs[rs][newPair];
}

SetOfStmts PKB::getStmtsForVar(RelationshipType rs, string varName) {
  return getRight(rs, TableType::VAR_TABLE, varName);
}

SetOfStmts PKB::getStmtsForExpr(RelationshipType rs, std::string expr) {
  int exprIndex = getIndexOf(TableType::EXPR_TABLE, expr);
  return getValue(&tablesExpr, rs, exprIndex);
}

SetOfStmtLists PKB::getVarMappings(RelationshipType rs) {
  return getMappings(rs, ParamPosition::BOTH);
}

SetOfStmtLists PKB::getVarMappingsForExpr(RelationshipType rs, string expr) {
  int exprIndex = getIndexOf(TableType::EXPR_TABLE, expr);
  if (mappingsForExpr.count(rs) == 0) {
    return SetOfStmtLists({});
  }

  if (mappingsForExpr[rs].count(exprIndex) == 0) {
    return SetOfStmtLists({});
  }

  return mappingsForExpr[rs][exprIndex];
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
