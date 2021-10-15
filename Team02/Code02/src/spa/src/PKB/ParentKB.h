#pragma once

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

typedef int STMT_NO;
typedef std::unordered_set<STMT_NO> UNO_SET_OF_STMT_NO;
typedef std::set<STMT_NO> SET_OF_STMT_NO;
typedef std::vector<STMT_NO> LIST_STMT_NO;

class ParentKB {
 public:
  // Consturctor
  ParentKB();

  // Methods for DE
  void setParent(STMT_NO s1, STMT_NO s2);
  void addParentT(STMT_NO s1, STMT_NO s2);

  // Methods for QE
  bool isParent(STMT_NO s1, STMT_NO s2);
  bool isParentT(STMT_NO s1, STMT_NO s2);
  UNO_SET_OF_STMT_NO getChildren(STMT_NO s1);
  STMT_NO getParent(STMT_NO s2);
  UNO_SET_OF_STMT_NO getChildrenT(STMT_NO s1);
  UNO_SET_OF_STMT_NO getParentsT(STMT_NO s2);
  std::vector<std::pair<STMT_NO, LIST_STMT_NO>> getAllParentsStmtPairs();
  std::vector<std::pair<STMT_NO, LIST_STMT_NO>> getAllParentsTStmtPairs();

 private:
  // Parent Tables
  // Each children can only have 1 immediate parent
  std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> table;
  std::unordered_map<STMT_NO, STMT_NO> invTable;
  std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> tableT;
  std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> invTableT;
};
