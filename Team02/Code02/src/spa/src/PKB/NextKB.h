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

class NextKB {
 public:
  // Constructor
  NextKB();

  // Methods for DE
  void addNext(STMT_NO s1, STMT_NO s2);

  // Methods for QE
  bool isNext(STMT_NO s1, STMT_NO s2);
  UNO_SET_OF_STMT_NO getNextStmts(STMT_NO s1);
  UNO_SET_OF_STMT_NO getPreviousStmts(STMT_NO s2);
  std::vector<std::pair<STMT_NO, std::vector<STMT_NO>>> getAllNextStmtPairs();

 private:
  // Next Tables
  std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> table;
  std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> invTable;
};
