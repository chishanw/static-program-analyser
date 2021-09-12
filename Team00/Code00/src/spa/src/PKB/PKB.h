#pragma once

#include <stdio.h>

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "FollowKB.h"
#include "ParentKB.h"
#include "VarTable.h"

using namespace std;

class PKB : public FollowKB, public ParentKB, public VarTable {
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

 private:
  // Members
  UNO_SET_OF_STMT_NO allReadStmtNo;
  UNO_SET_OF_STMT_NO allPrintStmtNo;
  UNO_SET_OF_STMT_NO allCallStmtNo;
  UNO_SET_OF_STMT_NO allWhileStmtNo;
  UNO_SET_OF_STMT_NO allIfStmtNo;
  UNO_SET_OF_STMT_NO allAssignStmtNo;
};
