#pragma once

#include <stdio.h>

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

#include "FollowKB.h"
#include "ParentKB.h"
#include "VarTable.h"
#include "ModifiesKB.h"
#include "UsesKB.h"

using namespace std;

class PKB {
 public:
  // Constructor
  PKB();

  // Methods
  void addStmt(STMT_NO s);
  void addReadStmt(STMT_NO s);
  void addPrintStmt(STMT_NO s);
  void addCallStmt(STMT_NO s);
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
  bool isCallStmt(int s);
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

  // Uses API
  void addUsesS(STMT_NO s, VAR_NAME var);
  bool isUsesS(STMT_NO s, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsUsedS(STMT_NO s);
  std::unordered_set<STMT_NO> getUsesS(VAR_NAME v);
  std::vector<std::pair<STMT_NO, std::vector<VAR_IDX>>> getAllUsesSPairs();

 private:
  // Design Abstractions
  VarTable varTable;
  FollowKB followKB;
  ParentKB parentKB;
  ModifiesKB modifiesKB = ModifiesKB(&varTable);
  UsesKB usesKB = UsesKB(&varTable);

  // Members
  UNO_SET_OF_STMT_NO allReadStmtNo;
  UNO_SET_OF_STMT_NO allPrintStmtNo;
  UNO_SET_OF_STMT_NO allCallStmtNo;
  UNO_SET_OF_STMT_NO allWhileStmtNo;
  UNO_SET_OF_STMT_NO allIfStmtNo;
  UNO_SET_OF_STMT_NO allAssignStmtNo;
};
