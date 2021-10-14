#include <Parser/Parser.h>
#include <Common/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;
using namespace Catch;

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
  REQUIRE(ra->Expr->GetFullExprPatternStr() == "[1]");
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
  REQUIRE(ra->Expr->GetFullExprPatternStr() == "[[[1]-[2]]+[3]]");
}

TEST_CASE("Parser Exception Test") {
  SECTION("No closing }") {
    string program =
        "procedure Example {\n"
        "  x = 2;\n"
        "  z = 3;\n";

    REQUIRE_THROWS_WITH(
        Parser().Parse(Tokenizer::TokenizeProgramString(program)),
        StartsWith("[Parser] Expected token "));
  }
}

// TODO(gf): disable this after iter1
TEST_CASE("Parser Exception Test (Iter1 ONLY)") {
  SECTION("> 1 procedures}") {
    string program =
        "procedure One {"
        "  a = 1;   "
        "}"
        "procedure Two {"
        "  b = 2;  "
        "}";

    REQUIRE_THROWS_WITH(
        Parser(true).Parse(Tokenizer::TokenizeProgramString(program)),
        Equals("[Parser] No more than 1 procedure is allowed in a SIMPLE "
               "program in iteration 1."));
  }

  SECTION("call stmt}") {
    string program =
        "procedure One {"
        "call Two;"
        "}\n";

    REQUIRE_THROWS_WITH(
        Parser(true).Parse(Tokenizer::TokenizeProgramString(program)),
        Equals("[Parser] Call Stmt is NOT allowed in a SIMPLE program in "
               "iteration 1."));
  }
}
