
#include <Parser/Parser.h>
#include <Common/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[ExprParser] exactly 1 expr and 1 factor") {
  string program =
      "procedure Example {\n"
      "  x = 1; }\n"
      "\n";
  vector<string> tokens = Tokenizer::TokenizeProgramString(program);
  ProgramAST* r = Parser().Parse(tokens);

  // ================================

  vector<string> expectedTokens = {"procedure", "Example", "{", "x",
                                   "=",         "1",       ";", "}"};
  REQUIRE(tokens == expectedTokens);

  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo == 1);
  REQUIRE(ra->VarName == "x");
  REQUIRE(ra->Expr->GetFullExprPatternStr() == "[1]");
}

TEST_CASE("[ExprParser] doulbe layer expr") {
  string program =
      "procedure Example {\n"
      "  x12y = 1 -2+ 3; }\n"
      "\n";
  vector<string> tokens = Tokenizer::TokenizeProgramString(program);
  ProgramAST* r = Parser().Parse(tokens);

  // ================================

  vector<string> expectedTokens = {"procedure", "Example", "{", "x12y",
                                   "=",         "1",       "-", "2",
                                   "+",         "3",       ";", "}"};
  REQUIRE(tokens == expectedTokens);

  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo == 1);
  REQUIRE(ra->VarName == "x12y");
  REQUIRE(ra->Expr->GetFullExprPatternStr() == "[[[1]-[2]]+[3]]");
}

TEST_CASE("[ExprParser] all math operator") {
  string program =
      "procedure Example {\n"
      "  x12y = 1 -(2- 3 *4) / 5%6+7; }\n"
      "\n";
  vector<string> tokens = Tokenizer::TokenizeProgramString(program);
  ProgramAST* r = Parser().Parse(tokens);

  // ================================

  vector<string> expectedTokens = {
      "procedure", "Example", "{", "x12y", "=", "1", "-", "(", "2", "-", "3",
      "*",         "4",       ")", "/",    "5", "%", "6", "+", "7", ";", "}"};
  REQUIRE(tokens == expectedTokens);

  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo == 1);
  REQUIRE(ra->VarName == "x12y");
  REQUIRE(ra->Expr->GetFullExprPatternStr() ==
          "[[[1]-[[[[2]-[[3]*[4]]]/[5]]%[6]]]+[7]]");
}

TEST_CASE("[ExprParser] constants and varnames") {
  string program =
      "procedure Example {\n"
      "  x12y = (c%1 -(2/b - 3 *4) / 5+a%6+7); }\n"
      "\n";
  vector<string> tokens = Tokenizer::TokenizeProgramString(program);
  ProgramAST* r = Parser().Parse(tokens);

  // ================================

  vector<string> expectedTokens = {
      "procedure", "Example", "{", "x12y", "=", "(", "c", "%", "1", "-",
      "(",         "2",       "/", "b",    "-", "3", "*", "4", ")", "/",
      "5",         "+",       "a", "%",    "6", "+", "7", ")", ";", "}"};
  REQUIRE(tokens == expectedTokens);

  AssignStmtAST* ra =
      dynamic_cast<AssignStmtAST*>(r->ProcedureList[0]->StmtList[0]);
  REQUIRE(r->ProcedureList[0]->StmtList[0]->StmtNo == 1);
  REQUIRE(ra->VarName == "x12y");
  REQUIRE(ra->Expr->GetFullExprPatternStr() ==
          "[[[[[c]%[1]]-[[[[2]/[b]]-[[3]*[4]]]/[5]]]+[[a]%[6]]]+[7]]");
}
