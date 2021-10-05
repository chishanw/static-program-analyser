#pragma once

#include <PKB/VarTable.h>
#include <PKB/ProcTable.h>

#include <stdio.h>
#include <iostream>

#include <string>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>

typedef int STMT_NO;
typedef std::string VAR_NAME, PROC_NAME;
typedef int VAR_IDX;

class UsesKB {
 public:
  explicit UsesKB(VarTable* varTable, ProcTable* procTable);

  // API for DE
  void addUsesS(STMT_NO s, VAR_NAME var);
  void addUsesP(PROC_NAME p, VAR_NAME var);

  // API for QE
  bool isUsesS(STMT_NO s, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsUsedS(STMT_NO s);
  std::unordered_set<STMT_NO> getUsesS(VAR_NAME v);
  std::vector<std::pair<STMT_NO, std::vector<VAR_IDX>>> getAllUsesSPairs();

  bool isUsesP(PROC_NAME p, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsUsedP(PROC_NAME p);
  std::unordered_set<PROC_IDX> getUsesP(VAR_NAME v);
  std::vector<std::pair<PROC_IDX, std::vector<VAR_IDX>>> getAllUsesPPairs();

 private:
  VarTable* varTable;
  ProcTable* procTable;
  std::unordered_map<STMT_NO, std::unordered_set<VAR_IDX>> tableS;
  std::unordered_map<VAR_IDX, std::unordered_set<STMT_NO>> invTableS;
  std::unordered_map<PROC_IDX, std::unordered_set<VAR_IDX>> tableP;
  std::unordered_map<VAR_IDX, std::unordered_set<PROC_IDX>> invTableP;
};
