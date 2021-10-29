#pragma once

#include <PKB/Table.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

typedef int STMT_NO;
typedef std::string VAR_NAME, PROC_NAME;
typedef int VAR_IDX, PROC_IDX;

class ModifiesKB {
 public:
  explicit ModifiesKB(Table* varTable, Table* procTable);

  // API for DE
  void addModifiesS(STMT_NO s, VAR_NAME var);
  void addModifiesP(PROC_NAME proc, VAR_NAME var);

  // API for QE
  bool isModifiesS(STMT_NO s, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsModifiedS(STMT_NO s);
  std::unordered_set<STMT_NO> getModifiesS(VAR_NAME v);
  std::vector<std::pair<STMT_NO, std::vector<VAR_IDX>>> getAllModifiesSPairs();

  bool isModifiesP(PROC_NAME proc, VAR_NAME v);
  std::unordered_set<VAR_IDX> getVarsModifiedP(PROC_NAME proc);
  std::unordered_set<PROC_IDX> getModifiesP(VAR_NAME v);
  std::vector<std::pair<PROC_IDX, std::vector<VAR_IDX>>> getAllModifiesPPairs();

 private:
  Table* varTable;
  Table* procTable;
  std::unordered_map<STMT_NO, std::unordered_set<VAR_IDX>> tableS;
  std::unordered_map<VAR_IDX, std::unordered_set<STMT_NO>> invTableS;
  std::unordered_map<PROC_IDX, std::unordered_set<VAR_IDX>> tableP;
  std::unordered_map<VAR_IDX, std::unordered_set<PROC_IDX>> invTableP;
};
