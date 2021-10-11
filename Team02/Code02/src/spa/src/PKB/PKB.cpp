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
UNO_SET_OF_STMT_NO PKB::getAllWhileStmts() { return allWhileStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllIfStmts() { return allIfStmtNo; }
UNO_SET_OF_STMT_NO PKB::getAllAssignStmts() { return allAssignStmtNo; }

bool PKB::isReadStmt(int s) { return allReadStmtNo.count(s) > 0; }
bool PKB::isPrintStmt(int s) { return allPrintStmtNo.count(s) > 0; }
bool PKB::isWhileStmt(int s) { return allWhileStmtNo.count(s) > 0; }
bool PKB::isIfStmt(int s) { return allIfStmtNo.count(s) > 0; }
bool PKB::isAssignStmt(int s) { return allAssignStmtNo.count(s) > 0; }

/* Design Abstraction API
1. Follows/FollowsT API
2. Parent/ParentT API
3. ModifiesS API
4. UsesS API*/

// Follows API
void PKB::setFollows(STMT_NO s1, STMT_NO s2) {
  followKB.setFollows(s1, s2);
}
void PKB::addFollowsT(STMT_NO s1, STMT_NO s2) {
  followKB.addFollowsT(s1, s2);
}
bool PKB::isFollowsT(STMT_NO s1, STMT_NO s2) {
  return followKB.isFollowsT(s1, s2);
}
STMT_NO PKB::getFollows(STMT_NO s1) {
  return followKB.getFollows(s1);
}
STMT_NO PKB::getFollowedBy(STMT_NO s2) {
  return followKB.getFollowedBy(s2);
}
UNO_SET_OF_STMT_NO PKB::getFollowsT(STMT_NO s1) {
  return followKB.getFollowsT(s1);
}
UNO_SET_OF_STMT_NO PKB::getFollowedTBy(STMT_NO s2) {
  return followKB.getFollowedTBy(s2);
}
vector<LIST_STMT_NO> PKB::getAllFollowsStmtPairs() {
  return followKB.getAllFollowsStmtPairs();
}
vector<pair<STMT_NO, LIST_STMT_NO>> PKB::getAllFollowsTStmtPairs() {
  return followKB.getAllFollowsTStmtPairs();
}

// Parent API
void PKB::setParent(STMT_NO s1, STMT_NO s2) {
  parentKB.setParent(s1, s2);
}
void PKB::addParentT(STMT_NO s1, STMT_NO s2) {
  parentKB.addParentT(s1, s2);
}
bool PKB::isParent(STMT_NO s1, STMT_NO s2) {
  return parentKB.isParent(s1, s2);
}
bool PKB::isParentT(STMT_NO s1, STMT_NO s2) {
  return parentKB.isParentT(s1, s2);
}
UNO_SET_OF_STMT_NO PKB::getChildren(STMT_NO s1) {
  return parentKB.getChildren(s1);
}
STMT_NO PKB::getParent(STMT_NO s2) {
  return parentKB.getParent(s2);
}
UNO_SET_OF_STMT_NO PKB::getChildrenT(STMT_NO s1) {
  return parentKB.getChildrenT(s1);
}
UNO_SET_OF_STMT_NO PKB::getParentsT(STMT_NO s2) {
  return parentKB.getParentsT(s2);
}
vector<pair<STMT_NO, LIST_STMT_NO>> PKB::getAllParentsStmtPairs() {
  return parentKB.getAllParentsStmtPairs();
}
vector<pair<STMT_NO, LIST_STMT_NO>> PKB::getAllParentsTStmtPairs() {
  return parentKB.getAllParentsTStmtPairs();
}

// Modifies API
void PKB::addModifiesS(STMT_NO s, VAR_NAME var) {
  modifiesKB.addModifiesS(s, var);
}
bool PKB::isModifiesS(STMT_NO s, VAR_NAME v) {
  return modifiesKB.isModifiesS(s, v);
}
unordered_set<VAR_IDX> PKB::getVarsModifiedS(STMT_NO s) {
  return modifiesKB.getVarsModifiedS(s);
}
unordered_set<STMT_NO> PKB::getModifiesS(VAR_NAME v) {
  return modifiesKB.getModifiesS(v);
}
vector<pair<STMT_NO, vector<VAR_IDX>>> PKB::getAllModifiesSPairs() {
  return modifiesKB.getAllModifiesSPairs();
}

void PKB::addModifiesP(PROC_NAME proc, VAR_NAME var) {
  modifiesKB.addModifiesP(proc, var);
}
bool PKB::isModifiesP(PROC_NAME proc, VAR_NAME v) {
  return modifiesKB.isModifiesP(proc, v);
}
unordered_set<VAR_IDX> PKB::getVarsModifiedP(PROC_NAME proc) {
  return modifiesKB.getVarsModifiedP(proc);
}
unordered_set<PROC_IDX> PKB::getModifiesP(VAR_NAME v) {
  return modifiesKB.getModifiesP(v);
}
vector<pair<PROC_IDX, vector<VAR_IDX>>> PKB::getAllModifiesPPairs() {
  return modifiesKB.getAllModifiesPPairs();
}

// Uses API
void PKB::addUsesS(STMT_NO s, VAR_NAME var) {
  usesKB.addUsesS(s, var);
}
bool PKB::isUsesS(STMT_NO s, VAR_NAME v) {
  return usesKB.isUsesS(s, v);
}
unordered_set<VAR_IDX> PKB::getVarsUsedS(STMT_NO s) {
  return usesKB.getVarsUsedS(s);
}
unordered_set<STMT_NO> PKB::getUsesS(VAR_NAME v) {
  return usesKB.getUsesS(v);
}
vector<pair<STMT_NO, vector<VAR_IDX>>> PKB::getAllUsesSPairs() {
  return usesKB.getAllUsesSPairs();
}

void PKB::addUsesP(PROC_NAME proc, VAR_NAME var) {
  usesKB.addUsesP(proc, var);
}
bool PKB::isUsesP(PROC_NAME proc, VAR_NAME v) {
  return usesKB.isUsesP(proc, v);
}
unordered_set<VAR_IDX> PKB::getVarsUsedP(PROC_NAME proc) {
  return usesKB.getVarsUsedP(proc);
}
unordered_set<PROC_IDX> PKB::getUsesP(VAR_NAME v) {
  return usesKB.getUsesP(v);
}
vector<pair<PROC_IDX, vector<VAR_IDX>>> PKB::getAllUsesPPairs() {
  return usesKB.getAllUsesPPairs();
}

// Pattern API
void PKB::addAssignPttFullExpr(STMT_NO s, string var, string expr) {
  patternKB.addAssignPttFullExpr(s, var, expr);
}
void PKB::addAssignPttSubExpr(STMT_NO s, string var, string expr) {
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
void PKB::addIfPtt(STMT_NO s, string varName) {
  patternKB.addIfPtt(s, varName);
}

void PKB::addWhilePtt(STMT_NO s, string varName) {
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
void PKB::addCalls(STMT_NO s, PROC_NAME caller, PROC_NAME callee) {
  addStmt(s);
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
UNO_SET_OF_STMT_NO PKB::getAllCallStmts() {
  return callsKB.getAllCallStmts();
}
bool PKB::isCallStmt(STMT_NO s) {
  return callsKB.isCallStmt(s);
}

// Next API
void PKB::addNext(STMT_NO s1, STMT_NO s2) {
  nextKB.addNext(s1, s2);
}
bool PKB::isNext(STMT_NO s1, STMT_NO s2) {
  return nextKB.isNext(s1, s2);
}
UNO_SET_OF_STMT_NO PKB::getNextStmts(STMT_NO s1) {
  return nextKB.getNextStmts(s1);
}
UNO_SET_OF_STMT_NO PKB::getPreviousStmts(STMT_NO s2) {
  return nextKB.getPreviousStmts(s2);
}
vector<pair<STMT_NO, vector<STMT_NO>>> PKB::getAllNextStmtPairs() {
  return nextKB.getAllNextStmtPairs();
}

// Table API
CONST_IDX PKB::insertConst(string constant) {
  return constTable.insertConst(constant);
}
string PKB::getConst(CONST_IDX constIdx) {
  return constTable.getConst(constIdx);
}
CONST_IDX PKB::getConstIndex(std::string constant) {
  return constTable.getConstIndex(constant);
}
unordered_set<CONST_IDX> PKB::getAllConstants() {
  return constTable.getAllConstants();
}
PROC_IDX PKB::insertProc(string procName) {
  return procTable.insertProc(procName);
}
string PKB::getProcName(PROC_IDX procIdx) {
  return procTable.getProcName(procIdx);
}
PROC_IDX PKB::getProcIndex(std::string procName) {
  return procTable.getProcIndex(procName);
}
unordered_set<PROC_IDX> PKB::getAllProcedures() { return procTable
  .getAllProcedures(); }
VAR_IDX PKB::insertVar(string varName) {
  return varTable.insertVar(varName);
}
string PKB::getVarName(VAR_IDX varIdx) {
  return varTable.getVarName(varIdx);
}
VAR_IDX PKB::getVarIndex(std::string varName) {
  return varTable.getVarIndex(varName);
}
unordered_set<VAR_IDX> PKB::getAllVariables() { return varTable
  .getAllVariables();
}
