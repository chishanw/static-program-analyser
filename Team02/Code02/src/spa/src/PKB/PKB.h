#pragma once

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "AffectsInfoKB.h"
#include "Common/Common.h"
#include "Table.h"

typedef std::unordered_map<RelationshipType, std::unordered_map<int, SetOfInts>>
    TablesRs;

typedef std::unordered_map<RelationshipType,
                           std::unordered_map<ParamPosition, SetOfStmtLists>>
    MappingsRs;
typedef std::unordered_map<TableType, Table, EnumClassHash> Tables;

typedef std::unordered_map<DesignEntity, SetOfStmts> TableOfStmts;

class PKB {
 public:
  void addStmt(DesignEntity de, StmtNo s);
  SetOfStmts getAllStmts(DesignEntity de);
  bool isStmt(DesignEntity de, StmtNo s);
  int getNumStmts(DesignEntity de);

  // TODO(Merlin): Refractor More.
  void addRs(RelationshipType rs, int left, int right);
  void addRs(RelationshipType rs, int left, TableType rightType,
             std::string right);
  void addRs(RelationshipType rs, TableType leftType, std::string left,
             TableType rightType, std::string right);

  bool isRs(RelationshipType rs, int left, int right);
  bool isRs(RelationshipType rs, int left, TableType rightType,
            std::string right);
  bool isRs(RelationshipType rs, TableType leftType, std::string left,
            TableType rightType, std::string right);

  std::unordered_set<int> getRight(RelationshipType rs, int left);
  std::unordered_set<int> getRight(RelationshipType rs, TableType rightType,
                                   std::string);

  std::unordered_set<int> getLeft(RelationshipType rs, int right);
  std::unordered_set<int> getLeft(RelationshipType rs, TableType rightType,
                                  std::string);

  SetOfStmtLists getMappings(RelationshipType rs, ParamPosition param);

  void addPatternRs(RelationshipType rs, StmtNo stmtNo, std::string varName,
                    std::string expr);
  void addPatternRs(RelationshipType rs, StmtNo stmtNo, std::string varName);
  SetOfStmts getStmtsForVarAndExpr(RelationshipType rs, std::string varName,
                                   std::string expr);
  SetOfStmts getStmtsForVar(RelationshipType rs, std::string varName);
  SetOfStmts getStmtsForExpr(RelationshipType type, std::string expr);
  SetOfStmtLists getVarMappingsForExpr(RelationshipType rs, std::string expr);
  SetOfStmtLists getVarMappings(RelationshipType rs);

  // Calls API
  void addCallStmtToCallee(StmtNo, ProcName);
  ProcIdx getProcCalledByCallStmt(int callStmtNum);

  // Affects Info API
  void addNextStmtForIfStmt(StmtNo ifStmt, StmtNo nextStmt);
  void addFirstStmtOfProc(PROC_NAME procName, StmtNo firstStmtOfProc);
  void addProcCallEdge(PROC_NAME callerProcName, PROC_NAME calleeProcName);
  StmtNo getNextStmtForIfStmt(StmtNo ifStmt);
  std::vector<StmtNo> getFirstStmtOfAllProcs();
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> getCallGraph();

  // Table API
  TableElemIdx insertAt(TableType type, std::string element);
  std::string getElementAt(TableType type, TableElemIdx index);
  TableElemIdx getIndexOf(TableType type, std::string element);
  std::unordered_set<TableElemIdx> getAllElementsAt(TableType type);

 private:
  // Members
  TablesRs tablesRs, invTablesRs, tablesExpr;
  std::unordered_map<RelationshipType, std::unordered_map<int, SetOfStmtLists>>
      mappingsForExpr;

  std::unordered_map<
      RelationshipType,
      std::unordered_map<std::pair<ExprIdx, VarIdx>, SetOfStmts, PairHash>>
      tablesPttRs;

  MappingsRs mappingsRs;
  TableOfStmts tableOfStmts;
  Tables tables = {{TableType::VAR_TABLE, Table()},
                   {TableType::CONST_TABLE, Table()},
                   {TableType::PROC_TABLE, Table()},
                   {TableType::EXPR_TABLE, Table()}};

  // Design Abstractions
  AffectsInfoKB affectsInfoKB =
      AffectsInfoKB(&tables.at(TableType::PROC_TABLE));

  std::unordered_map<StmtNo, ProcIdx> stmtToCallee;
};
