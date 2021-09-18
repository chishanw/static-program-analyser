#include "DesignExtractor/DesignExtractor.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

DesignExtractor::DesignExtractor(PKB* pkb) { this->pkb = pkb; }

void DesignExtractor::Extract(const ProgramAST* programAST) {
  ExtractProcAndStmt(programAST);

  ExtractExprPatterns(programAST);

  ExtractConst(programAST);

  ExtractParent(programAST);
  ExtractParentTrans(programAST);

  ExtractFollows(programAST);
  ExtractFollowsTrans(programAST);

  ExtractUsesRS(programAST);
  ExtractModifies(programAST);
  // ... other subroutines to extract other r/s go here too
}

void DesignExtractor::ExtractProcAndStmt(const ProgramAST* programAST) {
  for (auto procedure : programAST->ProcedureList) {
    pkb->insertProc(procedure->ProcName);

    ExtractProcAndStmtHelper(procedure->StmtList);
  }
}

void DesignExtractor::ExtractProcAndStmtHelper(
    const vector<StmtAST*> stmtList) {
  for (auto stmt : stmtList) {
    if (dynamic_cast<const ReadStmtAST*>(stmt)) {
      pkb->addReadStmt(stmt->StmtNo);
    } else if (dynamic_cast<const PrintStmtAST*>(stmt)) {
      pkb->addPrintStmt(stmt->StmtNo);
    } else if (dynamic_cast<const CallStmtAST*>(stmt)) {
      pkb->addCallStmt(stmt->StmtNo);
    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      pkb->addWhileStmt(stmt->StmtNo);

      ExtractProcAndStmtHelper(whileStmt->StmtList);
    } else if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      pkb->addIfStmt(stmt->StmtNo);

      ExtractProcAndStmtHelper(ifStmt->ThenBlock);
      ExtractProcAndStmtHelper(ifStmt->ElseBlock);
    } else if (dynamic_cast<const AssignStmtAST*>(stmt)) {
      pkb->addAssignStmt(stmt->StmtNo);
    } else {
      DMOprintErrMsgAndExit(
          "[DE][ExtractProcAndStmtHelper] shouldn't reach here.");
      return;
    }
  }
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

  vector<pair<STMT_NO, NAME>> result;

  for (auto procedure : programAST->ProcedureList) {
    result = ExtractUsesRSHelper(procedure->StmtList);

    for (auto p : result) pkb->addUsesS(p.first, p.second);
  }
}

vector<pair<STMT_NO, NAME>> DesignExtractor::ExtractUsesRSHelper(
    const vector<StmtAST*> stmtList) {
  vector<pair<STMT_NO, NAME>> result;      // resultAtThisNestingLevel
  vector<pair<STMT_NO, NAME>> resultNext;  // resultAtNextNestingLevel

  for (auto stmt : stmtList) {
    if (const AssignStmtAST* assignStmt =
            dynamic_cast<const AssignStmtAST*>(stmt)) {
      for (auto varName : assignStmt->Expr->GetAllVarNames()) {
        result.push_back(make_pair(assignStmt->StmtNo, varName));
      }

    } else if (const PrintStmtAST* printStmt =
                   dynamic_cast<const PrintStmtAST*>(stmt)) {
      result.push_back(make_pair(printStmt->StmtNo, printStmt->VarName));

    } else if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      unordered_set<NAME> uniqueDescendantVarNames;

      for (auto varName : ifStmt->CondExpr->GetAllVarNames()) {
        result.push_back(make_pair(ifStmt->StmtNo, varName));
      }

      // then
      resultNext = ExtractUsesRSHelper(ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantVarNames.insert(res.second);
      }

      // else
      resultNext = ExtractUsesRSHelper(ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantVarNames.insert(res.second);
      }

      // insert Descendant VarNames
      for (auto descendantVarName : uniqueDescendantVarNames) {
        result.push_back(make_pair(ifStmt->StmtNo, descendantVarName));
      }

    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      unordered_set<NAME> uniqueDescendantVarNames;

      for (auto varName : whileStmt->CondExpr->GetAllVarNames()) {
        result.push_back(make_pair(whileStmt->StmtNo, varName));
      }

      resultNext = ExtractUsesRSHelper(whileStmt->StmtList);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantVarNames.insert(res.second);
      }
      // insert Descendant VarNames
      for (auto descendantVarName : uniqueDescendantVarNames) {
        result.push_back(make_pair(whileStmt->StmtNo, descendantVarName));
      }
    }
  }

  return result;
}

void DesignExtractor::ExtractModifies(const ProgramAST* programAST) {
  vector<pair<STMT_NO, NAME>> result;
  for (auto procedure : programAST->ProcedureList) {
    result = ExtractModifiesHelper(procedure->StmtList);
    for (auto p : result) {
      pkb->addModifiesS(p.first, p.second);
    }
  }
}

vector<pair<STMT_NO, NAME>> DesignExtractor::ExtractModifiesHelper(
    const vector<StmtAST*> stmtList) {
  vector<pair<STMT_NO, NAME>> result;
  vector<pair<STMT_NO, NAME>> resultNext;

  for (auto stmt : stmtList) {
    // procedure and call not included in iteration 1
    if (const AssignStmtAST* assignStmt =
            dynamic_cast<const AssignStmtAST*>(stmt)) {
      result.push_back(make_pair(assignStmt->StmtNo, assignStmt->VarName));
    } else if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      unordered_set<NAME> uniqueDescendantVarNames;

      // then
      resultNext = ExtractModifiesHelper(ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantVarNames.insert(res.second);
      }

      // else
      resultNext = ExtractModifiesHelper(ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantVarNames.insert(res.second);
      }

      // insert descendant var names
      for (auto descendantVarName : uniqueDescendantVarNames) {
        result.push_back(make_pair(ifStmt->StmtNo, descendantVarName));
      }
    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      unordered_set<NAME> uniqueDescendantVarNames;

      resultNext = ExtractModifiesHelper(whileStmt->StmtList);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantVarNames.insert(res.second);
      }
      // insert descendant VarNames
      for (auto descendantVarName : uniqueDescendantVarNames) {
        result.push_back(make_pair(whileStmt->StmtNo, descendantVarName));
      }

    } else if (const ReadStmtAST* readStmt =
                   dynamic_cast<const ReadStmtAST*>(stmt)) {
      result.push_back(make_pair(readStmt->StmtNo, readStmt->VarName));
    }
  }

  return result;
}

void DesignExtractor::ExtractParent(const ProgramAST* programAST) {
  // only if and while stmt are container stmt,
  // and have parent r/s with the inner stmts

  for (auto procedure : programAST->ProcedureList) {
    // -1 is a special value for stmt at procedure level as they don't have a
    // parent, did this to minize duplicate code and improve code readability
    auto result = ExtractParentHelper(-1, procedure->StmtList);

    for (auto p : result) pkb->setParent(p.first, p.second);
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

    for (auto p : result) pkb->addParentT(p.first, p.second);
  }
}

vector<pair<STMT_NO, STMT_NO>> DesignExtractor::ExtractParentTransHelper(
    const STMT_NO parentStmtNo, const vector<StmtAST*> stmtList) {
  vector<pair<STMT_NO, STMT_NO>> result;           // resultAtThisNestingLevel
  vector<pair<STMT_NO, STMT_NO>> resultNext;       // resultAtNextNestingLevel
  unordered_set<STMT_NO> uniqueDescendantStmtNos;  // collate descendant StmtNos

  for (const StmtAST* stmt : stmtList) {
    uniqueDescendantStmtNos.insert(stmt->StmtNo);

    if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      resultNext = ExtractParentTransHelper(ifStmt->StmtNo, ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        // res.first not only include the current StmtNo (ifStmt->StmtNo), but
        // also parent StmtNo from deeper level
        uniqueDescendantStmtNos.insert(res.first);
        uniqueDescendantStmtNos.insert(res.second);
      }

      resultNext = ExtractParentTransHelper(ifStmt->StmtNo, ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantStmtNos.insert(res.first);
        uniqueDescendantStmtNos.insert(res.second);
      }

    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      resultNext =
          ExtractParentTransHelper(whileStmt->StmtNo, whileStmt->StmtList);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
      for (auto res : resultNext) {
        uniqueDescendantStmtNos.insert(res.first);
        uniqueDescendantStmtNos.insert(res.second);
      }
    }
  }

  // -1 is a special value, see ExtractParent method comment
  if (parentStmtNo != -1) {
    for (auto descendantStmtNo : uniqueDescendantStmtNos) {
      result.push_back(make_pair(parentStmtNo, descendantStmtNo));
    }
  }

  return result;
}

void DesignExtractor::ExtractFollows(const ProgramAST* programAST) {
  // must be at the same nesting level and in the same stmtLst
  for (auto procedure : programAST->ProcedureList) {
    auto result = ExtractFollowsHelper(procedure->StmtList);
    for (auto p : result) pkb->setFollows(p.first, p.second);
  }
}

vector<pair<STMT_NO, STMT_NO>> DesignExtractor::ExtractFollowsHelper(
    const vector<StmtAST*> stmtList) {
  vector<pair<STMT_NO, STMT_NO>> result;
  vector<pair<STMT_NO, STMT_NO>> resultNext;
  // at the start of each stmtList, the first stmt cannot follow anything,
  // so prevStmt is set to null
  StmtAST* prevStmt = NULL;
  StmtAST* currStmt;

  for (StmtAST* stmt : stmtList) {
    currStmt = stmt;
    // must be at the same nesting level!
    if (prevStmt != NULL) {
      result.push_back(make_pair(prevStmt->StmtNo, currStmt->StmtNo));
    }

    if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      resultNext = ExtractFollowsHelper(ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));

      resultNext = ExtractFollowsHelper(ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      resultNext = ExtractFollowsHelper(whileStmt->StmtList);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    }

    prevStmt = stmt;
  }

  return result;
}

void DesignExtractor::ExtractFollowsTrans(const ProgramAST* programAST) {
  for (auto procedure : programAST->ProcedureList) {
    auto result = ExtractFollowsTransHelper(procedure->StmtList);
    for (auto p : result) pkb->addFollowsT(p.first, p.second);
  }
}

vector<pair<STMT_NO, STMT_NO>> DesignExtractor::ExtractFollowsTransHelper(
    const vector<StmtAST*> stmtList) {
  vector<pair<STMT_NO, STMT_NO>> result;
  vector<pair<STMT_NO, STMT_NO>> resultNext;
  unordered_set<STMT_NO> uniqueAncestorStmtNos;

  for (StmtAST* stmt : stmtList) {
    for (auto ancestorStmtNo : uniqueAncestorStmtNos) {
      result.push_back(make_pair(ancestorStmtNo, stmt->StmtNo));
    }

    uniqueAncestorStmtNos.insert(stmt->StmtNo);

    if (IfStmtAST* ifStmt = dynamic_cast<IfStmtAST*>(stmt)) {
      resultNext = ExtractFollowsTransHelper(ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));

      resultNext = ExtractFollowsTransHelper(ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    } else if (const WhileStmtAST* whileStmt =
                   dynamic_cast<const WhileStmtAST*>(stmt)) {
      resultNext = ExtractFollowsTransHelper(whileStmt->StmtList);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
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
      vector<string> strs = expr->GetSubExprPatternStrs();

      string fullExpr = strs[0];
      pkb->addAssignPttFullExpr(stmt->StmtNo, varName, fullExpr);
      for (auto it = strs.begin(); it != strs.end(); ++it) {
        pkb->addAssignPttSubExpr(stmt->StmtNo, varName, *it);
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

void DesignExtractor::ExtractConst(const ProgramAST* programAST) {
  unordered_set<string> res;

  for (auto procedure : programAST->ProcedureList) {
    res.merge(ExtractConstHelper(procedure->StmtList));
  }

  for (auto constant : res) {
    pkb->insertConst(constant);
  }
}

unordered_set<string> DesignExtractor::ExtractConstHelper(
    vector<StmtAST*> stmtList) {
  unordered_set<string> res;

  for (auto stmt : stmtList) {
    if (const WhileStmtAST* whileStmt =
            dynamic_cast<const WhileStmtAST*>(stmt)) {
      res.merge(whileStmt->CondExpr->GetAllConsts());
      res.merge(ExtractConstHelper(whileStmt->StmtList));
    } else if (const IfStmtAST* ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      res.merge(ifStmt->CondExpr->GetAllConsts());
      res.merge(ExtractConstHelper(ifStmt->ThenBlock));
      res.merge(ExtractConstHelper(ifStmt->ElseBlock));
    } else if (const AssignStmtAST* assignStmt =
                   dynamic_cast<const AssignStmtAST*>(stmt)) {
      res.merge(assignStmt->Expr->GetAllConsts());
    }
  }

  return res;
}
