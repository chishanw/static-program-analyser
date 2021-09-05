#include <string>
#include <vector>

#include "PKB/PKB.h"
#include "Parser/AST.h"

class DesignExtractor {
 public:
  explicit DesignExtractor(PKB*);

  void Extract(const ProgramAST*);

 private:
  PKB* pkb;

  void ExtractExprPatterns(const ProgramAST* programAST);
  void ExtractExprPatternsHelper(vector<StmtAST*> stmtList);
};
