#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("Tokenizer Parser Integration Test 1") {
  string program =
      "procedure Example {\n"
      "  x = 1; }\n"
      "\n";
  ProgramAST* r = Parser().Parse(Tokenizer::TokenizeProgramString(program));

  // procedure
  REQUIRE(r->ProcedureList.size() == 1);
  REQUIRE(r->ProcedureList[0]->ProcName == "Example");

  // assign stmt
  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo == 1);
  REQUIRE(ra->VarName == "x");

  // expr
  REQUIRE(ra->Expr->GetPatternStr() == "1");
}

TEST_CASE("Tokenizer Parser Integration Test 2") {
  string program =
      "procedure Example {\n"
      "  x = 1 - 2 + 3; }\n"
      "\n";
  ProgramAST* r = Parser().Parse(Tokenizer::TokenizeProgramString(program));

  // procedure
  REQUIRE(r->ProcedureList.size() == 1);
  REQUIRE(r->ProcedureList[0]->ProcName == "Example");

  // assign stmt
  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo == 1);
  REQUIRE(ra->VarName == "x");

  // expr
  REQUIRE(ra->Expr->GetPatternStr() == "1-2+3");
}
