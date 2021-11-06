#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Common/AST.h"
#include "PKB/PKB.h"

typedef std::unordered_map<ProcName, std::unordered_set<ProcName>> CallGraph;
typedef std::pair<StmtNo, NAME> StmtNoNamePair;

struct StmtNoNamePairHash;

class DesignExtractor {
 public:
  explicit DesignExtractor(PKB*);

  void Extract(const ProgramAST*);

 private:
  PKB* pkb;

  std::unordered_set<NAME> ExtractProcAndStmt(const ProgramAST*);
  void ExtractProcAndStmtHelper(const std::vector<StmtAST*>);

  void ExtractUses(const ProgramAST*);
  std::unordered_set<StmtNoNamePair, StmtNoNamePairHash> ExtractUsesHelper(
      const std::vector<StmtAST*>, std::unordered_map<NAME, ProcedureAST*>);

  void ExtractModifies(const ProgramAST*);
  std::unordered_set<StmtNoNamePair, StmtNoNamePairHash> ExtractModifiesHelper(
      const std::vector<StmtAST*>, std::unordered_map<NAME, ProcedureAST*>);

  void ExtractParent(const ProgramAST*);
  std::vector<std::pair<StmtNo, StmtNo>> ExtractParentHelper(
      const StmtNo, const std::vector<StmtAST*>);
  void ExtractParentTrans(const ProgramAST*);
  std::vector<std::pair<StmtNo, StmtNo>> ExtractParentTransHelper(
      const StmtNo, const std::vector<StmtAST*>);

  void ExtractFollows(const ProgramAST*);
  std::vector<std::pair<StmtNo, StmtNo>> ExtractFollowsHelper(
      const std::vector<StmtAST*>);

  void ExtractFollowsTrans(const ProgramAST*);
  std::vector<std::pair<StmtNo, StmtNo>> ExtractFollowsTransHelper(
      const std::vector<StmtAST*>);

  void ExtractExprPatterns(const ProgramAST*);
  void ExtractExprPatternsHelper(const std::vector<StmtAST*>);

  void ExtractConst(const ProgramAST*);
  std::unordered_set<std::string> ExtractConstHelper(
      const std::vector<StmtAST*>);

  std::pair<CallGraph, CallGraph> ExtractCalls(const ProgramAST*,
                                               std::unordered_set<NAME>);
  std::unordered_map<StmtNo, ProcName> ExtractCallsHelper(
      const std::vector<StmtAST*>, std::unordered_set<NAME>);
  void ExtractCallsTrans(CallGraph, std::vector<ProcName>,
                         std::unordered_set<ProcName>);
  std::vector<ProcName> GetTopoSortedProcs(CallGraph, CallGraph,
                                           std::unordered_set<ProcName>);

  void ExtractNext(const ProgramAST*);
  void ExtractNextHelper(const std::vector<StmtAST*>, StmtNo, StmtNo);

  void ExtractNextBip(const ProgramAST*, std::vector<ProcName>);
  void ExtractNextBipHelper(
      const std::vector<StmtAST*>, StmtNo, StmtNo, ProcName,
      const std::unordered_map<NAME, StmtNo>&,
      std::unordered_map<ProcName, std::unordered_set<StmtNo>>*, bool);
};
