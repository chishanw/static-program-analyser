#pragma once

#include <PKB/VarTable.h>

#include <stdio.h>
#include <iostream>

#include <string>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>

typedef int STMT_NO;
typedef std::string VAR_NAME;
typedef int VAR_IDX;

class UsesKB {
 public:
  explicit UsesKB(VarTable* varTable);

  // API for DE
  void addUsesS(STMT_NO s, VAR_NAME var);

  // API for QE
  bool isUsesS(STMT_NO s, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsUsedS(STMT_NO s);
  std::unordered_set<STMT_NO> getUsesS(VAR_NAME v);
  std::vector<std::pair<STMT_NO, std::vector<VAR_IDX>>> getAllUsesSPairs();
 private:
  VarTable* varTable;
  std::unordered_map<STMT_NO, std::unordered_set<VAR_IDX>> usesTableByStmt;
  std::unordered_map<VAR_IDX, std::unordered_set<STMT_NO>> usesTableByVar;
};
