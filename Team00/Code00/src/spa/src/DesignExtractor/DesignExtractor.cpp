#include "DesignExtractor/DesignExtractor.h"

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

DesignExtractor::DesignExtractor(PKB* pkb) { this->pkb = pkb; }

void DesignExtractor::Extract(const ProgramAST* programAST) {
  ExtractExprPatterns(programAST);
  // ... other subroutines to extract other r/s go here too
}

void DesignExtractor::ExtractExprPatterns(const ProgramAST* programAST) {
  for (auto procedure : programAST->ProcedureList) {
    ExtractExprPatternsHelper(procedure->StmtList);
  }
}

void DesignExtractor::ExtractExprPatternsHelper(vector<StmtAST*> stmtList) {
  for (auto stmt : stmtList) {
    if (AssignStmtAST* assignStmt = dynamic_cast<AssignStmtAST*>(stmt)) {
      ArithAST* expr = assignStmt->Expr;
      string varName = assignStmt->VarName;
      vector<string> strs = expr->GetAllPatternStr();

      string wholeExpr = strs[0];
      // TODO(gf): rm prints and replace with pkb method calls
      cout << endl;
      cout << "Assign stmt: "
           << "[" << varName << "=" << wholeExpr << ";"
           << "]" << endl;
      cout << "Var name: " << varName << endl;
      cout << "Extracted " << strs.size() << " expressions:" << endl;
      cout << "Whole expr: " << wholeExpr << endl;
      // pkb.addExpr(varName, wholeExpr);
      for (auto it = strs.begin(); it != strs.end(); ++it) {
        // pkb.addSubExpr(varName, *it);
        cout << "Sub expr: " << *it << endl;
      }
    } else if (IfStmtAST* ifStmt = dynamic_cast<IfStmtAST*>(stmt)) {
      ExtractExprPatternsHelper(ifStmt->ThenBlock);
      ExtractExprPatternsHelper(ifStmt->ElseBlock);
    } else if (WhileStmtAST* whileStmt = dynamic_cast<WhileStmtAST*>(stmt)) {
      ExtractExprPatternsHelper(whileStmt->StmtList);
    }
  }
}
