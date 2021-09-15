#include <string>
#include <utility>
#include <vector>

#include "PKB/PKB.h"
#include "Parser/AST.h"

class DesignExtractor {
 public:
  explicit DesignExtractor(PKB*);

  void Extract(const ProgramAST*);

 private:
  PKB* pkb;

  void ExtractUsesRS(const ProgramAST*);
  std::vector<std::pair<STMT_NO, NAME>> ExtractUsesRSHelper(
      const vector<StmtAST*>);

  void ExtractModifies(const ProgramAST*);
  std::vector<std::pair<STMT_NO, NAME>> ExtractModifiesHelper(
      const vector<StmtAST*>);

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
};
