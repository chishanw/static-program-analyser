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

class ModifiesKB {
 public:
  explicit ModifiesKB(VarTable* varTable);

  // API for DE
  void addModifiesS(STMT_NO s, VAR_NAME var);

  // API for QE
  bool isModifiesS(STMT_NO s, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsModifiedS(STMT_NO s);
  std::unordered_set<STMT_NO> getModifiesS(VAR_NAME v);
  std::vector<std::pair<STMT_NO, std::vector<VAR_IDX>>> getAllModifiesSPairs();
 private:
  VarTable* varTable;
  std::unordered_map<STMT_NO, std::unordered_set<VAR_IDX>> modifiesTableByStmt;
  std::unordered_map<VAR_IDX, std::unordered_set<STMT_NO>> modifiesTableByVar;
};
