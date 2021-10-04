#include <stdio.h>

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "VarTable.h"

typedef int STMT_NO, VAR_IDX;
typedef std::unordered_set<STMT_NO> UNO_SET_OF_STMT_NO;

class PatternKB {
 public:
  // constructor
  explicit PatternKB(VarTable* vartable);

  // Methods for DE
  void addAssignPttFullExpr(STMT_NO s, std::string var, std::string expr);
  void addAssignPttSubExpr(STMT_NO s, std::string var, std::string expr);

  // Methods for QE

  std::unordered_set<int> getAssignForFullExpr(std::string expr);
  std::unordered_set<int> getAssignForSubExpr(std::string expr);
  std::unordered_set<int> getAssignForVarAndFullExpr(std::string varName,
                                                     std::string expr);
  std::unordered_set<int> getAssignForVarAndSubExpr(std::string varName,
                                                    std::string subExpr);

  std::vector<std::vector<int>> getAssignVarPairsForFullExpr(std::string expr);
  std::vector<std::vector<int>> getAssignVarPairsForSubExpr(
      std::string subExpr);
  std::unordered_set<int> getAssignForVar(std::string varName);
  std::vector<std::vector<int>> getAssignVarPairs();

 private:
  // Members

  std::unordered_map<std::string, UNO_SET_OF_STMT_NO> tableOfFullExpr;
  std::unordered_map<std::string, UNO_SET_OF_STMT_NO> tableOfSubExpr;
  std::unordered_map<std::string,
                     std::unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO>>
      tableOfFullExprPtt;
  std::unordered_map<std::string,
                     std::unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO>>
      tableOfSubExprPtt;
  std::unordered_map<std::string, std::vector<std::vector<int>>>
      tableOfFullExprPairs;
  std::unordered_map<std::string, std::vector<std::vector<int>>>
      tableOfSubExprPairs;
  std::unordered_map<VAR_IDX, UNO_SET_OF_STMT_NO> tableOfAssignForVar;
  std::vector<std::vector<int>> listOfAssignVarPairs;

  VarTable* varTable;

  // Helper Function
  int assignVar(std::string var);
};