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
#include "FollowKB.h"
#include "ModifiesKB.h"
#include "NextKB.h"
#include "ParentKB.h"
#include "PatternKB.h"
#include "UsesKB.h"
#include "Table.h"

typedef int CONST_IDX;

enum class TABLE_ENUM {
    VAR_TABLE,
    CONST_TABLE,
    PROC_TABLE
};

struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

class PKB {
 public:
  // Constructor
  PKB();

  // Methods
  void addStmt(STMT_NO s);
  void addReadStmt(STMT_NO s);
  void addPrintStmt(STMT_NO s);
  void addWhileStmt(STMT_NO s);
  void addIfStmt(STMT_NO s);
  void addAssignStmt(STMT_NO s);

  UNO_SET_OF_STMT_NO getAllStmts();
  UNO_SET_OF_STMT_NO getAllReadStmts();
  UNO_SET_OF_STMT_NO getAllPrintStmts();
  UNO_SET_OF_STMT_NO getAllCallStmts();
  UNO_SET_OF_STMT_NO getAllWhileStmts();
  UNO_SET_OF_STMT_NO getAllIfStmts();
  UNO_SET_OF_STMT_NO getAllAssignStmts();

  bool isReadStmt(int s);
  bool isPrintStmt(int s);
  bool isWhileStmt(int s);
  bool isIfStmt(int s);
  bool isAssignStmt(int s);

  UNO_SET_OF_STMT_NO allStmtNo;

  // Follows API
  void setFollows(STMT_NO s1, STMT_NO s2);
  void addFollowsT(STMT_NO s1, STMT_NO s2);
  bool isFollowsT(STMT_NO s1, STMT_NO s2);
  STMT_NO getFollows(STMT_NO s1);
  STMT_NO getFollowedBy(STMT_NO s2);
  UNO_SET_OF_STMT_NO getFollowsT(STMT_NO s1);
  UNO_SET_OF_STMT_NO getFollowedTBy(STMT_NO s2);
  std::vector<LIST_STMT_NO> getAllFollowsStmtPairs();
  std::vector<std::pair<STMT_NO, LIST_STMT_NO>> getAllFollowsTStmtPairs();

  // Parent API
  void setParent(STMT_NO s1, STMT_NO s2);
  void addParentT(STMT_NO s1, STMT_NO s2);
  bool isParent(STMT_NO s1, STMT_NO s2);
  bool isParentT(STMT_NO s1, STMT_NO s2);
  UNO_SET_OF_STMT_NO getChildren(STMT_NO s1);
  STMT_NO getParent(STMT_NO s2);
  UNO_SET_OF_STMT_NO getChildrenT(STMT_NO s1);
  UNO_SET_OF_STMT_NO getParentsT(STMT_NO s2);
  std::vector<std::pair<STMT_NO, LIST_STMT_NO>> getAllParentsStmtPairs();
  std::vector<std::pair<STMT_NO, LIST_STMT_NO>> getAllParentsTStmtPairs();

  // Modifies API
  void addModifiesS(STMT_NO s, VAR_NAME var);
  bool isModifiesS(STMT_NO s, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsModifiedS(STMT_NO s);
  std::unordered_set<STMT_NO> getModifiesS(VAR_NAME v);
  std::vector<std::pair<STMT_NO, std::vector<VAR_IDX>>> getAllModifiesSPairs();
  void addModifiesP(PROC_NAME proc, VAR_NAME var);
  bool isModifiesP(PROC_NAME p, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsModifiedP(PROC_NAME p);
  std::unordered_set<PROC_IDX> getModifiesP(VAR_NAME v);
  std::vector<std::pair<PROC_IDX, std::vector<VAR_IDX>>> getAllModifiesPPairs();

  // Uses API
  void addUsesS(STMT_NO s, VAR_NAME var);
  bool isUsesS(STMT_NO s, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsUsedS(STMT_NO s);
  std::unordered_set<STMT_NO> getUsesS(VAR_NAME v);
  std::vector<std::pair<STMT_NO, std::vector<VAR_IDX>>> getAllUsesSPairs();
  void addUsesP(PROC_NAME proc, VAR_NAME var);
  bool isUsesP(PROC_NAME proc, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsUsedP(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getUsesP(VAR_NAME v);
  std::vector<std::pair<PROC_IDX, std::vector<VAR_IDX>>> getAllUsesPPairs();

  // Pattern API
  void addAssignPttFullExpr(STMT_NO s, std::string var, std::string expr);
  void addAssignPttSubExpr(STMT_NO s, std::string var, std::string expr);
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
  void addIfPtt(STMT_NO s, std::string varName);
  std::unordered_set<int> getIfStmtForVar(std::string varName);
  std::vector<std::vector<int>> getIfStmtVarPairs();
  void addWhilePtt(STMT_NO s, std::string varName);
  std::unordered_set<int> getWhileStmtForVar(std::string varName);
  std::vector<std::vector<int>> getWhileStmtVarPairs();

  // Calls API
  void addCalls(STMT_NO s, PROC_NAME caller, PROC_NAME callee);
  void addCallsT(PROC_NAME caller, PROC_NAME callee);
  bool isCallStmt(STMT_NO s);
  bool isCalls(PROC_NAME caller, PROC_NAME callee);
  std::unordered_set<PROC_IDX> getProcsCalledBy(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getCallerProcs(PROC_NAME proc);
  std::vector<std::pair<PROC_IDX, std::vector<PROC_IDX>>> getAllCallsPairs();
  PROC_IDX getProcCalledByCallStmt(int callStmtNum);
  bool isCallsT(PROC_NAME caller, PROC_NAME callee);
  std::unordered_set<PROC_IDX> getProcsCalledTBy(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getCallerTProcs(PROC_NAME proc);
  std::vector<std::pair<PROC_IDX, std::vector<PROC_IDX>>> getAllCallsTPairs();

  // Next API
  void addNext(STMT_NO s1, STMT_NO s2);
  bool isNext(STMT_NO s1, STMT_NO s2);
  UNO_SET_OF_STMT_NO getNextStmts(STMT_NO s1);
  UNO_SET_OF_STMT_NO getPreviousStmts(STMT_NO s2);
  std::vector<std::pair<STMT_NO, std::vector<STMT_NO>>> getAllNextStmtPairs();

  // Affects Info API
  void addNextStmtForIfStmt(STMT_NO ifStmt, STMT_NO nextStmt);
  void addFirstStmtOfProc(PROC_NAME procName, STMT_NO firstStmtOfProc);
  void addProcCallEdge(PROC_NAME callerProcName, PROC_NAME calleeProcName);
  STMT_NO getNextStmtForIfStmt(STMT_NO ifStmt);
  std::vector<STMT_NO> getFirstStmtOfAllProcs();
  std::unordered_map<PROC_IDX, std::unordered_set<PROC_IDX>> getCallGraph();

  // Table API
  TABLE_ELEM_IDX insertAt(TABLE_ENUM type, std::string element);
  std::string getElementAt(TABLE_ENUM type, TABLE_ELEM_IDX index);
  TABLE_ELEM_IDX getIndexOf(TABLE_ENUM type, std::string element);
  std::unordered_set<TABLE_ELEM_IDX> getAllElementsAt(TABLE_ENUM type);

 private:
  // Members
  UNO_SET_OF_STMT_NO allReadStmtNo;
  UNO_SET_OF_STMT_NO allPrintStmtNo;
  UNO_SET_OF_STMT_NO allWhileStmtNo;
  UNO_SET_OF_STMT_NO allIfStmtNo;
  UNO_SET_OF_STMT_NO allAssignStmtNo;
  std::unordered_map<TABLE_ENUM, Table, EnumClassHash> tables = {
    { TABLE_ENUM::VAR_TABLE, Table() },
    { TABLE_ENUM::CONST_TABLE, Table() },
    { TABLE_ENUM::PROC_TABLE, Table() }
  };

  // Design Abstractions
  FollowKB followKB;
  ParentKB parentKB;
  NextKB nextKB;
  ModifiesKB modifiesKB = ModifiesKB(&tables.at(TABLE_ENUM::VAR_TABLE),
                                     &tables.at(TABLE_ENUM::PROC_TABLE));
  UsesKB usesKB = UsesKB(&tables.at(TABLE_ENUM::VAR_TABLE),
                         &tables.at(TABLE_ENUM::PROC_TABLE));
  PatternKB patternKB = PatternKB(&tables.at(TABLE_ENUM::VAR_TABLE));
  CallsKB callsKB = CallsKB(&tables.at(TABLE_ENUM::PROC_TABLE));
  AffectsInfoKB affectsInfoKB = AffectsInfoKB(&tables.at(
      TABLE_ENUM::PROC_TABLE));
};
