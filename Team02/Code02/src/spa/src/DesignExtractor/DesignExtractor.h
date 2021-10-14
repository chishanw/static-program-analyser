#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/AST.h"
#include "PKB/PKB.h"

typedef std::unordered_map<PROC_NAME, std::unordered_set<PROC_NAME>> CALL_GRAPH;
typedef std::pair<STMT_NO, NAME> STMT_NO_NAME_PAIR;

struct STMT_NO_NAME_PAIR_HASH;

class DesignExtractor {
 public:
  explicit DesignExtractor(PKB*);

  void Extract(const ProgramAST*);

 private:
  PKB* pkb;

  void ExtractProcAndStmt(const ProgramAST*);
  void ExtractProcAndStmtHelper(const vector<StmtAST*>);

  void ExtractUses(const ProgramAST*);
  std::unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH>
  ExtractUsesHelper(const vector<StmtAST*>, unordered_map<NAME, ProcedureAST*>);

  void ExtractModifies(const ProgramAST*);
  std::unordered_set<STMT_NO_NAME_PAIR, STMT_NO_NAME_PAIR_HASH>
  ExtractModifiesHelper(const vector<StmtAST*>,
                        unordered_map<NAME, ProcedureAST*>);

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
