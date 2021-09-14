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
