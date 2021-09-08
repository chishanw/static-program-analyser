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
  void ExtractUsesRSHelper(const vector<StmtAST*>);

  void ExtractParent(const ProgramAST*);
  std::vector<std::pair<STMT_NO, STMT_NO>> ExtractParentHelper(
      const STMT_NO, const std::vector<StmtAST*>);
  void ExtractParentTrans(const ProgramAST*);
  std::vector<std::pair<STMT_NO, STMT_NO>> ExtractParentTransHelper(
      const STMT_NO, const std::vector<StmtAST*>);

  void ExtractExprPatterns(const ProgramAST*);
  void ExtractExprPatternsHelper(const std::vector<StmtAST*>);
};
