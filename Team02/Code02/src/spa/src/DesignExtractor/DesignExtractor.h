#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "PKB/PKB.h"
#include "Parser/AST.h"

typedef std::unordered_map<PROC_NAME, std::unordered_set<PROC_NAME>> CALL_GRAPH;

class DesignExtractor {
 public:
  explicit DesignExtractor(PKB*);

  void Extract(const ProgramAST*);

 private:
  PKB* pkb;

  void ExtractProcAndStmt(const ProgramAST*);
  void ExtractProcAndStmtHelper(const vector<StmtAST*>);

  void ExtractUsesRS(const ProgramAST*);
  std::vector<std::pair<STMT_NO, NAME>> ExtractUsesRSHelper(
      const vector<StmtAST*>, unordered_map<NAME, ProcedureAST*>);

  void ExtractModifies(const ProgramAST*);
  std::vector<std::pair<STMT_NO, NAME>> ExtractModifiesHelper(
      const vector<StmtAST*>, unordered_map<NAME, ProcedureAST*>);

  void ExtractParent(const ProgramAST*);
  std::vector<std::pair<STMT_NO, STMT_NO>> ExtractParentHelper(
      const STMT_NO, const std::vector<StmtAST*>);
  void ExtractParentTrans(const ProgramAST*);
  std::vector<std::pair<STMT_NO, STMT_NO>> ExtractParentTransHelper(
      const STMT_NO, const std::vector<StmtAST*>);

  void ExtractFollows(const ProgramAST*);
  std::vector<std::pair<STMT_NO, STMT_NO>> ExtractFollowsHelper(
      const std::vector<StmtAST*>);

  void ExtractFollowsTrans(const ProgramAST*);
  std::vector<std::pair<STMT_NO, STMT_NO>> ExtractFollowsTransHelper(
      const std::vector<StmtAST*>);

  void ExtractExprPatterns(const ProgramAST*);
  void ExtractExprPatternsHelper(const std::vector<StmtAST*>);

  void ExtractConst(const ProgramAST*);
  std::unordered_set<std::string> ExtractConstHelper(
      const std::vector<StmtAST*>);

  CALL_GRAPH ExtractCalls(const ProgramAST*);
  std::unordered_map<STMT_NO, PROC_NAME> ExtractCallsHelper(
      const std::vector<StmtAST*>);
  void ExtractCallsTrans(CALL_GRAPH);
  void ExtractCallsTransHelper(CALL_GRAPH, PROC_NAME, PROC_NAME);
};
