#include "DesignExtractor/DesignExtractor.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/Common.h"

using namespace std;

struct StmtNoNamePairHash {
  size_t operator()(const StmtNoNamePair& p) const {
    string s = to_string(p.first) + '_' + p.second;
    return hash<string>{}(s);
  }
};

DesignExtractor::DesignExtractor(PKB* pkb) { this->pkb = pkb; }

void DesignExtractor::Extract(const ProgramAST* programAST) {
  unordered_set<ProcName> allProcs = ExtractProcAndStmt(programAST);

  ExtractExprPatterns(programAST);

  ExtractConst(programAST);

  ExtractParent(programAST);
  ExtractParentTrans(programAST);

  ExtractFollows(programAST);
  ExtractFollowsTrans(programAST);

  // ExtractCalls & ExtractCallsTrans should be called before ExtractUses,
  // ExtractModifies, ExtractNext & ExtractNextBip to prevent infinite recursion
  // if there exists Recursive/Cyclic Calls
  pair<CallGraph, CallGraph> callGraphPair = ExtractCalls(programAST, allProcs);
  CallGraph callGraph = callGraphPair.first;
  CallGraph reverseCallGraph = callGraphPair.second;

  vector<ProcName> topoProcs =
      GetTopoSortedProcs(callGraph, reverseCallGraph, allProcs);

  ExtractCallsTrans(reverseCallGraph, topoProcs, allProcs);

  ExtractUses(programAST);
  ExtractModifies(programAST);

  ExtractNext(programAST);
  ExtractNextBip(programAST, topoProcs);
}

unordered_set<NAME> DesignExtractor::ExtractProcAndStmt(
    const ProgramAST* programAST) {
  unordered_set<NAME> allProcs;
  for (auto procedure : programAST->ProcedureList) {
    if (allProcs.count(procedure->ProcName) > 0) {
      throw runtime_error("2 procedures with same name detected.");
    }
    allProcs.insert(procedure->ProcName);
    pkb->insertAt(TableType::PROC_TABLE, procedure->ProcName);

    ExtractProcAndStmtHelper(procedure->StmtList);
  }
  return allProcs;
}

void DesignExtractor::ExtractProcAndStmtHelper(
    const vector<StmtAST*> stmtList) {
  for (auto stmt : stmtList) {
    if (dynamic_cast<const ReadStmtAST*>(stmt)) {
      pkb->addStmt(DesignEntity::READ, stmt->StmtNo);
    } else if (dynamic_cast<const PrintStmtAST*>(stmt)) {
      pkb->addStmt(DesignEntity::PRINT, stmt->StmtNo);
    } else if (dynamic_cast<const CallStmtAST*>(stmt)) {
      pkb->addStmt(DesignEntity::CALL, stmt->StmtNo);
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      pkb->addStmt(DesignEntity::WHILE, stmt->StmtNo);

      ExtractProcAndStmtHelper(whileStmt->StmtList);
    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      pkb->addStmt(DesignEntity::IF, stmt->StmtNo);

      ExtractProcAndStmtHelper(ifStmt->ThenBlock);
      ExtractProcAndStmtHelper(ifStmt->ElseBlock);
    } else if (dynamic_cast<const AssignStmtAST*>(stmt)) {
      pkb->addStmt(DesignEntity::ASSIGN, stmt->StmtNo);
    } else {
      DMOprintErrMsgAndExit(
          "[DE][ExtractProcAndStmtHelper] shouldn't reach here.");
      return;
    }
  }
}

void mergeResultHelper(
    unordered_set<StmtNoNamePair, StmtNoNamePairHash>* resPtr,
    unordered_set<StmtNoNamePair, StmtNoNamePairHash>* resNextPtr,
    StmtNo stmtNo) {
  for (auto res : *resNextPtr) {
    resPtr->insert(make_pair(stmtNo, res.second));
  }
  resPtr->merge(*resNextPtr);
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

  unordered_set<StmtNoNamePair, StmtNoNamePairHash> result;

  unordered_map<NAME, ProcedureAST*> procNameToAST;
  for (auto procedure : programAST->ProcedureList) {
    procNameToAST.insert({procedure->ProcName, procedure});
  }

  for (auto procedure : programAST->ProcedureList) {
    result = ExtractUsesHelper(procedure->StmtList, procNameToAST);

    for (auto p : result) {
      pkb->addRs(RelationshipType::USES_S, p.first, TableType::VAR_TABLE,
                 p.second);
      pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE,
                 procedure->ProcName, TableType::VAR_TABLE, p.second);
    }
  }
}

unordered_set<StmtNoNamePair, StmtNoNamePairHash>
DesignExtractor::ExtractUsesHelper(
    const vector<StmtAST*> stmtList,
    unordered_map<NAME, ProcedureAST*> procNameToAST) {
  unordered_set<StmtNoNamePair, StmtNoNamePairHash>
      result;  // resultAtThisNestingLevel
  unordered_set<StmtNoNamePair, StmtNoNamePairHash>
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
        pkb->addPatternRs(RelationshipType::PTT_IF, ifStmt->StmtNo,
                          varName);  // for if pattern
        result.insert(make_pair(ifStmt->StmtNo, varName));
      }

      // then
      resultNext = ExtractUsesHelper(ifStmt->ThenBlock, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);

      // else
      resultNext = ExtractUsesHelper(ifStmt->ElseBlock, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);

    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      for (auto varName : whileStmt->CondExpr->GetAllVarNames()) {
        pkb->addPatternRs(RelationshipType::PTT_WHILE, whileStmt->StmtNo,
                          varName);  // for while pattern
        result.insert(make_pair(whileStmt->StmtNo, varName));
      }

      resultNext = ExtractUsesHelper(whileStmt->StmtList, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);

    } else if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
      ProcedureAST* calledProc = procNameToAST.at(callStmt->ProcName);

      resultNext = ExtractUsesHelper(calledProc->StmtList, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);
    }
  }

  return result;
}

void DesignExtractor::ExtractModifies(const ProgramAST* programAST) {
  unordered_set<StmtNoNamePair, StmtNoNamePairHash> result;

  unordered_map<NAME, ProcedureAST*> procNameToAST;
  for (auto procedure : programAST->ProcedureList) {
    procNameToAST.insert({procedure->ProcName, procedure});
  }

  for (auto procedure : programAST->ProcedureList) {
    result = ExtractModifiesHelper(procedure->StmtList, procNameToAST);
    for (auto p : result) {
      pkb->addRs(RelationshipType::MODIFIES_S, p.first, TableType::VAR_TABLE,
                 p.second);
      pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                 procedure->ProcName, TableType::VAR_TABLE, p.second);
    }
  }
}

unordered_set<StmtNoNamePair, StmtNoNamePairHash>
DesignExtractor::ExtractModifiesHelper(
    const vector<StmtAST*> stmtList,
    unordered_map<NAME, ProcedureAST*> procNameToAST) {
  unordered_set<StmtNoNamePair, StmtNoNamePairHash> result;
  unordered_set<StmtNoNamePair, StmtNoNamePairHash> resultNext;

  for (auto stmt : stmtList) {
    // procedure and call not included in iteration 1
    if (auto assignStmt = dynamic_cast<const AssignStmtAST*>(stmt)) {
      result.insert(make_pair(assignStmt->StmtNo, assignStmt->VarName));

    } else if (auto readStmt = dynamic_cast<const ReadStmtAST*>(stmt)) {
      result.insert(make_pair(readStmt->StmtNo, readStmt->VarName));

    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      // then
      resultNext = ExtractModifiesHelper(ifStmt->ThenBlock, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);

      // else
      resultNext = ExtractModifiesHelper(ifStmt->ElseBlock, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);

    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      resultNext = ExtractModifiesHelper(whileStmt->StmtList, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);

    } else if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
      ProcedureAST* calledProc = procNameToAST.at(callStmt->ProcName);

      resultNext = ExtractModifiesHelper(calledProc->StmtList, procNameToAST);
      mergeResultHelper(&result, &resultNext, stmt->StmtNo);
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

    for (auto p : result)
      pkb->addRs(RelationshipType::PARENT, p.first, p.second);
  }
}

vector<pair<StmtNo, StmtNo>> DesignExtractor::ExtractParentHelper(
    const StmtNo parentStmtNo, const vector<StmtAST*> stmtList) {
  vector<pair<StmtNo, StmtNo>> result;      // resultAtThisNestingLevel
  vector<pair<StmtNo, StmtNo>> resultNext;  // resultAtNextNestingLevel

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

    for (auto p : result)
      pkb->addRs(RelationshipType::PARENT_T, p.first, p.second);
  }
}

vector<pair<StmtNo, StmtNo>> DesignExtractor::ExtractParentTransHelper(
    const StmtNo parentStmtNo, const vector<StmtAST*> stmtList) {
  vector<pair<StmtNo, StmtNo>> result;            // resultAtThisNestingLevel
  vector<pair<StmtNo, StmtNo>> resultNext;        // resultAtNextNestingLevel
  unordered_set<StmtNo> uniqueDescendantStmtNos;  // collate descendant StmtNos

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
    for (auto p : result)
      pkb->addRs(RelationshipType::FOLLOWS, p.first, p.second);
  }
}

vector<pair<StmtNo, StmtNo>> DesignExtractor::ExtractFollowsHelper(
    const vector<StmtAST*> stmtList) {
  vector<pair<StmtNo, StmtNo>> result;
  vector<pair<StmtNo, StmtNo>> resultNext;
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
    for (auto p : result)
      pkb->addRs(RelationshipType::FOLLOWS_T, p.first, p.second);
  }
}

vector<pair<StmtNo, StmtNo>> DesignExtractor::ExtractFollowsTransHelper(
    const vector<StmtAST*> stmtList) {
  vector<pair<StmtNo, StmtNo>> result;
  vector<pair<StmtNo, StmtNo>> resultNext;
  unordered_set<StmtNo> uniqueAncestorStmtNos;

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

      string fullExpr = assignStmt->Expr->GetFullExprPatternStr();
      pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, stmt->StmtNo,
                        varName, fullExpr);

      for (string subExpr : expr->GetSubExprPatternStrs()) {
        pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, stmt->StmtNo,
                          varName, subExpr);
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
    pkb->insertAt(TableType::CONST_TABLE, constant);
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

pair<CallGraph, CallGraph> DesignExtractor::ExtractCalls(
    const ProgramAST* programAST, unordered_set<NAME> allProcs) {
  CallGraph callGraph;
  CallGraph reverseCallGraph;

  for (auto caller : programAST->ProcedureList) {
    unordered_set<ProcName> allProcsCalled;

    unordered_map<StmtNo, ProcName> res =
        ExtractCallsHelper(caller->StmtList, allProcs);
    for (auto p : res) {
      StmtNo callStmtNo = p.first;
      ProcName callee = p.second;

      pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE,
                 caller->ProcName, TableType::PROC_TABLE, callee);
      pkb->addRs(RelationshipType::CALLS_S, callStmtNo, TableType::PROC_TABLE,
                 callee);

      // build call graphs
      if (caller->ProcName == callee)
        throw runtime_error("Recursive call detected.");

      allProcsCalled.insert(callee);

      if (reverseCallGraph.count(callee) == 0)
        reverseCallGraph[callee] = unordered_set<ProcName>{};
      reverseCallGraph[callee].insert(caller->ProcName);
    }

    if (!allProcsCalled.empty()) callGraph[caller->ProcName] = allProcsCalled;
  }

  // for Affects r/s
  for (auto p : callGraph) {
    ProcName callerProcName = p.first;
    for (ProcName calleeProcName : p.second) {
      pkb->addProcCallEdge(callerProcName, calleeProcName);
    }
  }

  return pair<CallGraph, CallGraph>{callGraph, reverseCallGraph};
}

unordered_map<StmtNo, ProcName> DesignExtractor::ExtractCallsHelper(
    const vector<StmtAST*> stmtList, unordered_set<NAME> allProcs) {
  unordered_map<StmtNo, ProcName> res;
  for (auto stmt : stmtList) {
    if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
      if (allProcs.count(callStmt->ProcName) < 1) {
        throw runtime_error(
            "Found call statement calling non-existent procedure.");
      }
      res.insert({callStmt->StmtNo, callStmt->ProcName});
    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      res.merge(ExtractCallsHelper(whileStmt->StmtList, allProcs));
    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      res.merge(ExtractCallsHelper(ifStmt->ThenBlock, allProcs));
      res.merge(ExtractCallsHelper(ifStmt->ElseBlock, allProcs));
    }
  }

  return res;
}

void DesignExtractor::ExtractCallsTrans(CallGraph reverseCallGraph,
                                        vector<ProcName> topoProcs,
                                        unordered_set<NAME> allProcs) {
  if (topoProcs.size() != allProcs.size())
    throw runtime_error("Cyclic call detected.");

  CallGraph transCallGraph;

  reverse(topoProcs.begin(), topoProcs.end());

  for (auto proc : topoProcs) {
    // if no proc calls this proc
    if (reverseCallGraph.count(proc) == 0) continue;

    for (auto caller : reverseCallGraph[proc]) {
      transCallGraph[caller].insert(proc);

      // if this proc calls no proc
      if (transCallGraph.count(proc) == 0) continue;

      for (auto callee : transCallGraph[proc]) {
        transCallGraph[caller].insert(callee);
      }
    }
  }

  for (auto p : transCallGraph) {
    auto caller = p.first;
    for (auto callee : p.second)
      pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, caller,
                 TableType::PROC_TABLE, callee);
  }
}

vector<ProcName> DesignExtractor::GetTopoSortedProcs(
    CallGraph callGraph, CallGraph reverseCallGraph,
    unordered_set<ProcName> allProcs) {
  vector<ProcName> res;

  unordered_map<ProcName, int> indegree;
  for (auto proc : allProcs) indegree[proc] = 0;
  for (auto p : reverseCallGraph) {
    ProcName callee = p.first;
    indegree[callee] = p.second.size();
  }

  unordered_set<ProcName> procQueue;
  for (auto p : indegree) {
    if (p.second == 0) procQueue.insert(p.first);
  }

  while (!procQueue.empty()) {
    // pop one procedure from the bfsQueue
    ProcName procName = *(procQueue.begin());
    procQueue.erase(procName);

    res.push_back(procName);

    for (auto callee : callGraph[procName]) {
      indegree[callee]--;
      if (indegree[callee] == 0) procQueue.insert(callee);
    }
  }

  return res;
}

void DesignExtractor::ExtractNext(const ProgramAST* programAST) {
  for (auto procedure : programAST->ProcedureList) {
    pkb->addFirstStmtOfProc(procedure->ProcName,
                            procedure->StmtList.front()->StmtNo);
  }

  for (auto procedure : programAST->ProcedureList) {
    ExtractNextHelper(procedure->StmtList, -1, -1);
  }
}

void DesignExtractor::ExtractNextHelper(const vector<StmtAST*> stmtList,
                                        StmtNo prevStmt,
                                        StmtNo nextStmtForLastStmt) {
  auto addNext = [this](StmtNo s1, StmtNo s2) {
    if (s1 == -1 || s2 == -1) return;
    pkb->addRs(RelationshipType::NEXT, s1, s2);
  };
  auto getNextStmtNoAfterCurrIdx = [stmtList](size_t i) {
    if (i + 1 < stmtList.size()) {
      return stmtList[i + 1]->StmtNo;
    }
    return -1;
  };

  for (size_t i = 0; i < stmtList.size(); ++i) {
    auto stmt = stmtList[i];

    addNext(prevStmt, stmt->StmtNo);

    if (dynamic_cast<const ReadStmtAST*>(stmt) ||
        dynamic_cast<const PrintStmtAST*>(stmt) ||
        dynamic_cast<const CallStmtAST*>(stmt) ||
        dynamic_cast<const AssignStmtAST*>(stmt)) {
      prevStmt = stmt->StmtNo;

    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      ExtractNextHelper(whileStmt->StmtList, stmt->StmtNo, stmt->StmtNo);
      prevStmt = stmt->StmtNo;

    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      StmtNo nextStmtAfterIf = getNextStmtNoAfterCurrIdx(i);

      if (nextStmtAfterIf != -1) {
        pkb->addNextStmtForIfStmt(stmt->StmtNo, nextStmtAfterIf);
        // skip extraction if this stmt is the last stmt in current stmtList,
        // let the next block of code handles it, for better performance
        ExtractNextHelper(ifStmt->ThenBlock, ifStmt->StmtNo, nextStmtAfterIf);
        ExtractNextHelper(ifStmt->ElseBlock, ifStmt->StmtNo, nextStmtAfterIf);
      }

      prevStmt = -1;  // because there's no Next r/s between the current if stmt
                      // and the next stmt in the current stmtList

    } else {
      DMOprintErrMsgAndExit(
          "[DE][ExtractNextHelper][for loop] shouldn't reach "
          "here.");
      return;
    }
  }

  // settle last stmt in curr stmtList
  auto stmt = stmtList[stmtList.size() - 1];
  if (dynamic_cast<const ReadStmtAST*>(stmt) ||
      dynamic_cast<const PrintStmtAST*>(stmt) ||
      dynamic_cast<const CallStmtAST*>(stmt) ||
      dynamic_cast<const AssignStmtAST*>(stmt) ||
      dynamic_cast<const WhileStmtAST*>(stmt)) {
    addNext(stmt->StmtNo, nextStmtForLastStmt);
  } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
    if (nextStmtForLastStmt != -1) {
      pkb->addNextStmtForIfStmt(ifStmt->StmtNo, nextStmtForLastStmt);
    }
    ExtractNextHelper(ifStmt->ThenBlock, ifStmt->StmtNo, nextStmtForLastStmt);
    ExtractNextHelper(ifStmt->ElseBlock, ifStmt->StmtNo, nextStmtForLastStmt);
  } else {
    DMOprintErrMsgAndExit(
        "[DE][ExtractNextHelper][last stmt] shouldn't reach here.");
    return;
  }
}

void DesignExtractor::ExtractNextBip(const ProgramAST* programAST,
                                     vector<ProcName> topoProcs) {
  unordered_map<NAME, StmtNo> procNameToItsFirstStmt;
  for (auto procedure : programAST->ProcedureList) {
    procNameToItsFirstStmt[procedure->ProcName] =
        procedure->StmtList[0]->StmtNo;
  }

  unordered_map<ProcName, const ProcedureAST*> procNameToProc;
  for (auto procedure : programAST->ProcedureList) {
    procNameToProc[procedure->ProcName] = procedure;
  }

  // for storing callee's procName to the last stmts in this call path
  unordered_map<ProcName, unordered_set<StmtNo>> lastStmtsOfCallPath;

  reverse(topoProcs.begin(), topoProcs.end());

  for (auto procName : topoProcs) {
    ExtractNextBipHelper(procNameToProc[procName]->StmtList, -1, -1, procName,
                         procNameToItsFirstStmt, &lastStmtsOfCallPath, true);
  }
}

void DesignExtractor::ExtractNextBipHelper(
    const std::vector<StmtAST*> stmtList, StmtNo prevStmt,
    StmtNo nextStmtForLastStmt, ProcName currProcName,
    const unordered_map<NAME, StmtNo>& procNameToItsFirstStmt,
    unordered_map<ProcName, unordered_set<StmtNo>>* lastStmtsOfCallPath,
    bool isAtProcLevel) {
  auto addNextBip = [this](StmtNo s1, StmtNo s2) {
    if (s1 == -1 || s2 == -1) return;
    pkb->addRs(RelationshipType::NEXT_BIP, s1, s2);
  };
  auto getNextStmtNoAfterCurrIdx = [stmtList](size_t i) {
    if (i + 1 < stmtList.size()) {
      return stmtList[i + 1]->StmtNo;
    }
    return -1;
  };

  for (size_t i = 0; i < stmtList.size(); ++i) {
    auto stmt = stmtList[i];

    addNextBip(prevStmt, stmt->StmtNo);

    if (dynamic_cast<const ReadStmtAST*>(stmt) ||
        dynamic_cast<const PrintStmtAST*>(stmt) ||
        dynamic_cast<const AssignStmtAST*>(stmt)) {
      prevStmt = stmt->StmtNo;

    } else if (auto whileStmt = dynamic_cast<const WhileStmtAST*>(stmt)) {
      ExtractNextBipHelper(whileStmt->StmtList, stmt->StmtNo, stmt->StmtNo,
                           currProcName, procNameToItsFirstStmt,
                           lastStmtsOfCallPath, false);
      prevStmt = stmt->StmtNo;

    } else if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
      auto nextStmtAfterCall = getNextStmtNoAfterCurrIdx(i);
      auto callee = callStmt->ProcName;
      auto firstStmtInCallee = procNameToItsFirstStmt.at(callee);
      auto lastStmtsInThisCallPath = lastStmtsOfCallPath->at(callee);

      addNextBip(callStmt->StmtNo, firstStmtInCallee);

      for (auto lastStmt : lastStmtsInThisCallPath) {
        addNextBip(lastStmt, nextStmtAfterCall);
      }

      prevStmt = -1;

    } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
      StmtNo nextStmtAfterIf = getNextStmtNoAfterCurrIdx(i);

      if (nextStmtAfterIf != -1) {
        // skip extraction if this stmt is the last stmt in current stmtList,
        // let the next block of code handles it, for better performance
        ExtractNextBipHelper(ifStmt->ThenBlock, ifStmt->StmtNo, nextStmtAfterIf,
                             currProcName, procNameToItsFirstStmt,
                             lastStmtsOfCallPath, false);
        ExtractNextBipHelper(ifStmt->ElseBlock, ifStmt->StmtNo, nextStmtAfterIf,
                             currProcName, procNameToItsFirstStmt,
                             lastStmtsOfCallPath, false);
      }

      prevStmt = -1;  // because there's no Next r/s between the current if
                      // stmt and the next stmt in the current stmtList

    } else {
      DMOprintErrMsgAndExit(
          "[DE][ExtractNextBipHelper][for loop] shouldn't reach "
          "here.");
      return;
    }
  }

  // settle last stmt in curr stmtList
  auto stmt = stmtList[stmtList.size() - 1];
  if (dynamic_cast<const ReadStmtAST*>(stmt) ||
      dynamic_cast<const PrintStmtAST*>(stmt) ||
      dynamic_cast<const AssignStmtAST*>(stmt) ||
      dynamic_cast<const WhileStmtAST*>(stmt)) {
    addNextBip(stmt->StmtNo, nextStmtForLastStmt);

    if (isAtProcLevel)
      (*lastStmtsOfCallPath)[currProcName].insert(stmt->StmtNo);

  } else if (auto ifStmt = dynamic_cast<const IfStmtAST*>(stmt)) {
    ExtractNextBipHelper(ifStmt->ThenBlock, ifStmt->StmtNo, nextStmtForLastStmt,
                         currProcName, procNameToItsFirstStmt,
                         lastStmtsOfCallPath, isAtProcLevel);
    ExtractNextBipHelper(ifStmt->ElseBlock, ifStmt->StmtNo, nextStmtForLastStmt,
                         currProcName, procNameToItsFirstStmt,
                         lastStmtsOfCallPath, isAtProcLevel);
  } else if (auto callStmt = dynamic_cast<const CallStmtAST*>(stmt)) {
    ProcName callee = callStmt->ProcName;

    for (auto lastStmt : (*lastStmtsOfCallPath)[callee]) {
      addNextBip(lastStmt, nextStmtForLastStmt);
    }

    if (isAtProcLevel) {
      for (auto lastStmt : (*lastStmtsOfCallPath)[callee]) {
        (*lastStmtsOfCallPath)[currProcName].insert(lastStmt);
      }
    }

  } else {
    DMOprintErrMsgAndExit(
        "[DE][ExtractNextBipHelper][last stmt] shouldn't reach here.");
    return;
  }
}
