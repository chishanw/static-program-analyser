#pragma once

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "AffectsInfoKB.h"
#include "CallsKB.h"
#include "Common/Common.h"
#include "PatternKB.h"
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

  SetOfStmts getAllCallStmts();

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

  // Pattern API
  void addAssignPttFullExpr(StmtNo s, std::string var, std::string expr);
  void addAssignPttSubExpr(StmtNo s, std::string var, std::string expr);
  std::unordered_set<int> getAssignForFullExpr(std::string expr);
  std::unordered_set<int> getAssignForSubExpr(std::string expr);
  std::unordered_set<int> getAssignForVarAndFullExpr(std::string varName,
                                                     std::string subExpr);
  std::unordered_set<int> getAssignForVarAndSubExpr(std::string varName,
                                                    std::string expr);

  std::vector<std::vector<int>> getAssignVarPairsForFullExpr(std::string expr);
  std::vector<std::vector<int>> getAssignVarPairsForSubExpr(
      std::string subExpr);
  std::unordered_set<int> getAssignForVar(std::string varName);
  std::vector<std::vector<int>> getAssignVarPairs();
  void addIfPtt(StmtNo s, std::string varName);
  std::unordered_set<int> getIfStmtForVar(std::string varName);
  std::vector<std::vector<int>> getIfStmtVarPairs();
  void addWhilePtt(StmtNo s, std::string varName);
  std::unordered_set<int> getWhileStmtForVar(std::string varName);
  std::vector<std::vector<int>> getWhileStmtVarPairs();

  // Calls API
  void addCalls(StmtNo s, PROC_NAME caller, PROC_NAME callee);
  void addCallsT(PROC_NAME caller, PROC_NAME callee);
  bool isCallStmt(StmtNo s);
  bool isCalls(PROC_NAME caller, PROC_NAME callee);
  std::unordered_set<PROC_IDX> getProcsCalledBy(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getCallerProcs(PROC_NAME proc);
  std::vector<std::pair<PROC_IDX, std::vector<PROC_IDX>>> getAllCallsPairs();
  PROC_IDX getProcCalledByCallStmt(int callStmtNum);
  bool isCallsT(PROC_NAME caller, PROC_NAME callee);
  std::unordered_set<PROC_IDX> getProcsCalledTBy(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getCallerTProcs(PROC_NAME proc);
  std::vector<std::pair<PROC_IDX, std::vector<PROC_IDX>>> getAllCallsTPairs();

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
  TablesRs tablesRs, invTablesRs;
  MappingsRs mappingsRs;
  TableOfStmts tableOfStmts;
  Tables tables = {{TableType::VAR_TABLE, Table()},
                   {TableType::CONST_TABLE, Table()},
                   {TableType::PROC_TABLE, Table()}};

  // Design Abstractions
  PatternKB patternKB = PatternKB(&tables.at(TableType::VAR_TABLE));
  CallsKB callsKB = CallsKB(&tables.at(TableType::PROC_TABLE));
  AffectsInfoKB affectsInfoKB =
      AffectsInfoKB(&tables.at(TableType::PROC_TABLE));
};
