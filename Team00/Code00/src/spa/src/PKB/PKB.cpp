#include "PKB.h"

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

PKB::PKB() {
  allStmtNo = UNO_SET_OF_STMT_NO();
  allReadStmtNo = UNO_SET_OF_STMT_NO();
  allPrintStmtNo = UNO_SET_OF_STMT_NO();
  allCallStmtNo = UNO_SET_OF_STMT_NO();
  allWhileStmtNo = UNO_SET_OF_STMT_NO();
  allIfStmtNo = UNO_SET_OF_STMT_NO();
  allAssignStmtNo = UNO_SET_OF_STMT_NO();
}

void PKB::addStmt(STMT_NO s) {
  if (allStmtNo.count(s) > 0) {
    return;
  }

  allStmtNo.insert(s);
}

void PKB::addReadStmt(STMT_NO s) {
  if (allReadStmtNo.count(s) > 0) {
    return;
  }

  addStmt(s);
  allReadStmtNo.insert(s);
}

void PKB::addPrintStmt(STMT_NO s) {
  if (allPrintStmtNo.count(s) > 0) {
    return;
  }

  addStmt(s);
  allPrintStmtNo.insert(s);
}

void PKB::addCallStmt(STMT_NO s) {
  if (allCallStmtNo.count(s) > 0) {
    return;
  }

  addStmt(s);
  allCallStmtNo.insert(s);
}

void PKB::addWhileStmt(STMT_NO s) {
  if (allWhileStmtNo.count(s) > 0) {
    return;
  }

  addStmt(s);
  allWhileStmtNo.insert(s);
}

void PKB::addIfStmt(STMT_NO s) {
  if (allIfStmtNo.count(s) > 0) {
    return;
  }

  addStmt(s);
  allIfStmtNo.insert(s);
}

void PKB::addAssignStmt(STMT_NO s) {
  if (allAssignStmtNo.count(s) > 0) {
    return;
  }

  addStmt(s);
  allAssignStmtNo.insert(s);
}

UNO_SET_OF_STMT_NO PKB::getAllStmts() { return allStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllReadStmts() { return allReadStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllPrintStmts() { return allPrintStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllCallStmts() { return allCallStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllWhileStmts() { return allWhileStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllIfStmts() { return allIfStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllAssignStmts() { return allAssignStmtNo; }

bool PKB::isReadStmt(int s) { return allReadStmtNo.count(s) > 0; }
bool PKB::isPrintStmt(int s) { return allPrintStmtNo.count(s) > 0; }
bool PKB::isCallStmt(int s) { return allCallStmtNo.count(s) > 0; }
bool PKB::isWhileStmt(int s) { return allWhileStmtNo.count(s) > 0; }
bool PKB::isIfStmt(int s) { return allIfStmtNo.count(s) > 0; }
bool PKB::isAssignStmt(int s) { return allAssignStmtNo.count(s) > 0; }
