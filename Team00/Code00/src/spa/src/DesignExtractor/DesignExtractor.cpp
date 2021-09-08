#include "DesignExtractor/DesignExtractor.h"

#include <stdio.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

DesignExtractor::DesignExtractor(PKB* pkb) { this->pkb = pkb; }

void DesignExtractor::Extract(const ProgramAST* programAST) {
  ExtractExprPatterns(programAST);

  ExtractParent(programAST);
  ExtractParentTrans(programAST);

  ExtractUsesRS(programAST);
  // ... other subroutines to extract other r/s go here too
}

void DesignExtractor::ExtractUsesRS(const ProgramAST* programAST) {
  // From slides ...
  // 1. Assignment a Variable v
  // Uses (a, v) holds if variable v appears on the right hand side of a

  // 2. Print statement pn Variable v
  // Uses (pn, v) holds if variable v appears in pn.

  // 3. Container statement s (i.e. "if" or "while") Variable v
  // Uses (s, v) holds if v appears in the condition of s, or there is a
  // statement s1 in the container such that Uses(s1, v) holds

  // 4. Procedure p Variable v
  // Uses (p, v) holds if there is a statement s in p or in a procedure called
  // (directly or indirectly) from p such that Uses (s, v) holds.

  // 5. Procedure call c (i.e. "call p") Variable v
  // Uses (c, v) is defined in the same way as Uses (p, v).

  // TODO(de): 4 and 5 is not included in iter1
  for (auto procedure : programAST->ProcedureList) {
    ExtractUsesRSHelper(procedure->StmtList);
  }
}

void DesignExtractor::ExtractUsesRSHelper(const vector<StmtAST*> stmtList) {
  auto callPKB = [](STMT_NO stmtNo, NAME varName) {
    // TODO(gf): rm prints and replace with pkb method calls
    // pkb.setUsesS(stmtNo, varName);
    cout << "Uses r/s result: "
         << "<" << stmtNo << ", " << varName << ">" << endl;
  };

  auto callPKBforList = [](STMT_NO stmtNo, vector<NAME> varNames) {
    // TODO(gf): rm prints and replace with pkb method calls
    // pkb.setUsesS(stmtNo, varName);
    for (auto varName : varNames) {
      cout << "Uses r/s result: "
           << "<" << stmtNo << ", " << varName << ">" << endl;
    }
  };

  for (auto stmt : stmtList) {
    if (const AssignStmtAST* assignStmt =
            dynamic_cast<const AssignStmtAST*>(stmt)) {
      callPKBforList(assignStmt->StmtNo, assignStmt->Expr->GetAllVarNames());
    } else if (const PrintStmtAST* printStmt =
                   dynamic_cast<const PrintStmtAST*>(stmt)) {
      callPKB(printStmt->StmtNo, printStmt->VarName);
    } else if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      callPKBforList(ifStmt->StmtNo, ifStmt->CondExpr->GetAllVarNames());

      ExtractUsesRSHelper(ifStmt->ThenBlock);
      ExtractUsesRSHelper(ifStmt->ElseBlock);
    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      callPKBforList(whileStmt->StmtNo, whileStmt->CondExpr->GetAllVarNames());

      ExtractUsesRSHelper(whileStmt->StmtList);
    }
  }
}

void DesignExtractor::ExtractParent(const ProgramAST* programAST) {
  // only if and while stmt are container stmt,
  // and have parent r/s with the inner stmts

  for (auto procedure : programAST->ProcedureList) {
    // -1 is a special value for stmt at procedure level as they don't have a
    // parent, did this to minize duplicate code and improve code readability
    auto result = ExtractParentHelper(-1, procedure->StmtList);

    // TODO(gf): rm prints and replace with pkb method calls
    // pkb.setParent(parentStmtNo, childStmtNo);
    for (auto p : result)
      cout << "Parent r/s result: "
           << "<" << p.first << ", " << p.second << ">" << endl;
  }
}

vector<pair<STMT_NO, STMT_NO>> DesignExtractor::ExtractParentHelper(
    const STMT_NO parentStmtNo, const vector<StmtAST*> stmtList) {
  vector<pair<STMT_NO, STMT_NO>> result;      // resultAtThisNestingLevel
  vector<pair<STMT_NO, STMT_NO>> resultNext;  // resultAtNextNestingLevel

  for (const StmtAST* stmt : stmtList) {
    // -1 is a special value, see ExtractParent method comment
    if (parentStmtNo != -1) {
      result.push_back(make_pair(parentStmtNo, stmt->StmtNo));
    }

    if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      resultNext = ExtractParentHelper(ifStmt->StmtNo, ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));

      resultNext = ExtractParentHelper(ifStmt->StmtNo, ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      resultNext = ExtractParentHelper(whileStmt->StmtNo, whileStmt->StmtList);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    }
  }

  return result;
}

void DesignExtractor::ExtractParentTrans(const ProgramAST* programAST) {
  for (auto procedure : programAST->ProcedureList) {
    // -1 is a special value for stmt at procedure level as they don't have a
    // parent, did this to minize duplicate code and improve code readability
    auto result = ExtractParentTransHelper(-1, procedure->StmtList);

    // TODO(gf): rm prints and replace with pkb method calls
    // pkb.setParent(parentStmtNo, childStmtNo);
    for (auto p : result)
      cout << "Parent* r/s result: "
           << "<" << p.first << ", " << p.second << ">" << endl;
  }
}

vector<pair<STMT_NO, STMT_NO>> DesignExtractor::ExtractParentTransHelper(
    const STMT_NO parentStmtNo, const vector<StmtAST*> stmtList) {
  vector<pair<STMT_NO, STMT_NO>> result;         // resultAtThisNestingLevel
  vector<pair<STMT_NO, STMT_NO>> resultNext;     // resultAtNextNestingLevel
  unordered_set<STMT_NO> uniqueAncestorStmtNos;  // collate Ancestor StmtNos

  for (const StmtAST* stmt : stmtList) {
    uniqueAncestorStmtNos.insert(stmt->StmtNo);

    if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      resultNext = ExtractParentTransHelper(ifStmt->StmtNo, ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        // res.first not only include the current StmtNo (ifStmt->StmtNo), but
        // also parent StmtNo from deeper level
        uniqueAncestorStmtNos.insert(res.first);
        uniqueAncestorStmtNos.insert(res.second);
      }

      resultNext = ExtractParentTransHelper(ifStmt->StmtNo, ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueAncestorStmtNos.insert(res.first);
        uniqueAncestorStmtNos.insert(res.second);
      }

    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      resultNext =
          ExtractParentTransHelper(whileStmt->StmtNo, whileStmt->StmtList);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueAncestorStmtNos.insert(res.first);
        uniqueAncestorStmtNos.insert(res.second);
      }
    }
  }

  // -1 is a special value, see ExtractParent method comment
  if (parentStmtNo != -1) {
    for (auto ancestorStmtNo : uniqueAncestorStmtNos) {
      result.push_back(make_pair(parentStmtNo, ancestorStmtNo));
    }
  }

  return result;
}

void DesignExtractor::ExtractExprPatterns(const ProgramAST* programAST) {
  for (auto procedure : programAST->ProcedureList) {
    ExtractExprPatternsHelper(procedure->StmtList);
  }
}

void DesignExtractor::ExtractExprPatternsHelper(vector<StmtAST*> stmtList) {
  for (auto stmt : stmtList) {
    if (const AssignStmtAST* assignStmt =
            dynamic_cast<const AssignStmtAST*>(stmt)) {
      const ArithAST* expr = assignStmt->Expr;
      string varName = assignStmt->VarName;
      vector<string> strs = expr->GetAllPatternStr();

      string wholeExpr = strs[0];
      // TODO(gf): rm prints and replace with pkb method calls
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
    } else if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      ExtractExprPatternsHelper(ifStmt->ThenBlock);
      ExtractExprPatternsHelper(ifStmt->ElseBlock);
    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      ExtractExprPatternsHelper(whileStmt->StmtList);
    }
  }
}
