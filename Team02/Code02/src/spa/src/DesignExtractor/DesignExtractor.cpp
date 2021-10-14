#include "DesignExtractor/DesignExtractor.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

struct STMT_NO_NAME_PAIR_HASH {
  size_t operator()(const STMT_NO_NAME_PAIR& p) const {
    string s = to_string(p.first) + '_' + p.second;
    return hash<string>{}(s);
  }
};

DesignExtractor::DesignExtractor(PKB* pkb) { this->pkb = pkb; }

void DesignExtractor::Extract(const ProgramAST* programAST) {
  ExtractProcAndStmt(programAST);

  ExtractExprPatterns(programAST);

  ExtractConst(programAST);

  ExtractParent(programAST);
  ExtractParentTrans(programAST);

  ExtractFollows(programAST);
  ExtractFollowsTrans(programAST);

  CALL_GRAPH callGraph = ExtractCalls(programAST);
  ExtractCallsTrans(callGraph);  // this should be called before ExtractUses
                                 // or ExtractModifies to prevent infinite
                                 // recursion if there exists Recursive/Cyclic
                                 // Calls

  ExtractUses(programAST);
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
      // handled by ExtractCalls
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      pkb->addWhileStmt(stmt->StmtNo);

      ExtractProcAndStmtHelper(whileStmt->StmtList);
    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
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

void DesignExtractor::ExtractUses(const ProgramAST* programAST) {
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

  unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH> result;

  unordered_map<NAME, ProcedureAST*> procNameToAST;
  for (auto procedure : programAST->ProcedureList) {
    procNameToAST.insert({procedure->ProcName, procedure});
  }

  for (auto procedure : programAST->ProcedureList) {
    result = ExtractUsesHelper(procedure->StmtList, procNameToAST);

    for (auto p : result) {
      pkb->addUsesS(p.first, p.second);
      pkb->addUsesP(procedure->ProcName, p.second);
    }
  }
}

unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH>
DesignExtractor::ExtractUsesHelper(
    const vector<StmtAST*> stmtList,
    unordered_map<NAME, ProcedureAST*> procNameToAST) {
  unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH>
      result;  // resultAtThisNestingLevel
  unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH>
      resultNext;  // resultAtNextNestingLevel

  for (auto stmt : stmtList) {
    if (auto assignStmt = dynamic_cast<const AssignStmtAST*>(stmt)) {
      for (auto varName : assignStmt->Expr->GetAllVarNames()) {
        result.insert(make_pair(assignStmt->StmtNo, varName));
      }

    } else if (auto printStmt = dynamic_cast<const PrintStmtAST*>(stmt)) {
      result.insert(make_pair(printStmt->StmtNo, printStmt->VarName));

    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      for (auto varName : ifStmt->CondExpr->GetAllVarNames()) {
        pkb->addIfPtt(ifStmt->StmtNo, varName);  // for if pattern
        result.insert(make_pair(ifStmt->StmtNo, varName));
      }

      // then
      resultNext = ExtractUsesHelper(ifStmt->ThenBlock, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(ifStmt->StmtNo, res.second));
      }
      result.merge(resultNext);

      // else
      resultNext = ExtractUsesHelper(ifStmt->ElseBlock, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(ifStmt->StmtNo, res.second));
      }
      result.merge(resultNext);

    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      for (auto varName : whileStmt->CondExpr->GetAllVarNames()) {
        pkb->addWhilePtt(whileStmt->StmtNo, varName);  // for while pattern
        result.insert(make_pair(whileStmt->StmtNo, varName));
      }

      resultNext = ExtractUsesHelper(whileStmt->StmtList, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(whileStmt->StmtNo, res.second));
      }
      result.merge(resultNext);

    } else if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
      ProcedureAST* calledProc = procNameToAST.at(callStmt->ProcName);

      resultNext = ExtractUsesHelper(calledProc->StmtList, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(callStmt->StmtNo, res.second));
      }
      result.merge(resultNext);
    }
  }

  return result;
}

void DesignExtractor::ExtractModifies(const ProgramAST* programAST) {
  unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH> result;

  unordered_map<NAME, ProcedureAST*> procNameToAST;
  for (auto procedure : programAST->ProcedureList) {
    procNameToAST.insert({procedure->ProcName, procedure});
  }

  for (auto procedure : programAST->ProcedureList) {
    result = ExtractModifiesHelper(procedure->StmtList, procNameToAST);
    for (auto p : result) {
      pkb->addModifiesS(p.first, p.second);
      pkb->addModifiesP(procedure->ProcName, p.second);
    }
  }
}

unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH>
DesignExtractor::ExtractModifiesHelper(
    const vector<StmtAST*> stmtList,
    unordered_map<NAME, ProcedureAST*> procNameToAST) {
  unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH> result;
  unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH> resultNext;

  for (auto stmt : stmtList) {
    // procedure and call not included in iteration 1
    if (auto assignStmt = dynamic_cast<const AssignStmtAST*>(stmt)) {
      result.insert(make_pair(assignStmt->StmtNo, assignStmt->VarName));
    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      // then
      resultNext = ExtractModifiesHelper(ifStmt->ThenBlock, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(ifStmt->StmtNo, res.second));
      }
      result.merge(resultNext);

      // else
      resultNext = ExtractModifiesHelper(ifStmt->ElseBlock, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(ifStmt->StmtNo, res.second));
      }
      result.merge(resultNext);

    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      resultNext = ExtractModifiesHelper(whileStmt->StmtList, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(whileStmt->StmtNo, res.second));
      }
      result.merge(resultNext);

    } else if (auto readStmt = dynamic_cast<const ReadStmtAST*>(stmt)) {
      result.insert(make_pair(readStmt->StmtNo, readStmt->VarName));
    } else if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
      ProcedureAST* calledProc = procNameToAST.at(callStmt->ProcName);

      resultNext = ExtractModifiesHelper(calledProc->StmtList, procNameToAST);
      for (auto res : resultNext) {
        result.insert(make_pair(callStmt->StmtNo, res.second));
      }
      result.merge(resultNext);
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

    if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      resultNext = ExtractParentHelper(ifStmt->StmtNo, ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));

      resultNext = ExtractParentHelper(ifStmt->StmtNo, ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
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

    if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
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

    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
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

    if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      resultNext = ExtractFollowsHelper(ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));

      resultNext = ExtractFollowsHelper(ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
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

    if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      resultNext = ExtractFollowsTransHelper(ifStmt->ThenBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));

      resultNext = ExtractFollowsTransHelper(ifStmt->ElseBlock);
      copy(resultNext.begin(), resultNext.end(), back_inserter(result));
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
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
    if (auto assignStmt = dynamic_cast<const AssignStmtAST*>(stmt)) {
      const ArithAST* expr = assignStmt->Expr;
      string varName = assignStmt->VarName;
      vector<string> strs = expr->GetSubExprPatternStrs();

      string fullExpr = strs[0];
      pkb->addAssignPttFullExpr(stmt->StmtNo, varName, fullExpr);
      for (string subExpr : strs) {
        pkb->addAssignPttSubExpr(stmt->StmtNo, varName, subExpr);
      }
    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      ExtractExprPatternsHelper(ifStmt->ThenBlock);
      ExtractExprPatternsHelper(ifStmt->ElseBlock);
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
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
    if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      res.merge(whileStmt->CondExpr->GetAllConsts());
      res.merge(ExtractConstHelper(whileStmt->StmtList));
    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      res.merge(ifStmt->CondExpr->GetAllConsts());
      res.merge(ExtractConstHelper(ifStmt->ThenBlock));
      res.merge(ExtractConstHelper(ifStmt->ElseBlock));
    } else if (auto assignStmt = dynamic_cast<const AssignStmtAST*>(stmt)) {
      res.merge(assignStmt->Expr->GetAllConsts());
    }
  }

  return res;
}

CALL_GRAPH DesignExtractor::ExtractCalls(const ProgramAST* programAST) {
  CALL_GRAPH callGraph;

  for (auto caller : programAST->ProcedureList) {
    unordered_set<PROC_NAME> allProcsCalled;

    unordered_map<STMT_NO, PROC_NAME> res =
        ExtractCallsHelper(caller->StmtList);
    for (auto p : res) {
      pkb->addCalls(p.first, caller->ProcName, p.second);
      allProcsCalled.insert(p.second);
    }

    // allProcsCalled could be empty set
    callGraph.insert({caller->ProcName, allProcsCalled});
  }

  return callGraph;
}

unordered_map<STMT_NO, PROC_NAME> DesignExtractor::ExtractCallsHelper(
    const vector<StmtAST*> stmtList) {
  unordered_map<STMT_NO, PROC_NAME> res;
  for (auto stmt : stmtList) {
    if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
      res.insert({callStmt->StmtNo, callStmt->ProcName});
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      res.merge(ExtractCallsHelper(whileStmt->StmtList));
    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      res.merge(ExtractCallsHelper(ifStmt->ThenBlock));
      res.merge(ExtractCallsHelper(ifStmt->ElseBlock));
    }
  }

  return res;
}

void DesignExtractor::ExtractCallsTrans(CALL_GRAPH callGraph) {
  for (auto p : callGraph) {
    PROC_NAME caller = p.first;
    unordered_set<PROC_NAME> callees = p.second;
    for (auto callee : callees) {
      ExtractCallsTransHelper(callGraph, caller, callee);
    }
  }
}

void DesignExtractor::ExtractCallsTransHelper(CALL_GRAPH callGraph,
                                              PROC_NAME caller,
                                              PROC_NAME callee) {
  if (caller == callee) {
    throw runtime_error("Cyclic/Recursive loop detected.");
    return;
  }

  pkb->addCallsT(caller, callee);

  unordered_set<PROC_NAME> nextLayerCallees = callGraph.at(callee);
  for (auto nextLayerCallee : nextLayerCallees) {
    ExtractCallsTransHelper(callGraph, caller, nextLayerCallee);
  }
}
