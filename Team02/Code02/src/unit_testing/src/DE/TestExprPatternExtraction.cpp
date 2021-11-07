
#include <Common/Tokenizer.h>
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>

#include <vector>

#include "catch.hpp"

using namespace std;
using namespace Catch;

TEST_CASE("[DE][Expr Pattern] sample source") {
  string program =
      "procedure Example {\n"
      "  x = 2;\n"
      "  z = 3;\n"
      "  i = 5;\n"
      "  while (i!=0) {\n"
      "    x = x - 1;\n"
      "    if (x==1) then {\n"
      "      z = x + 1; }\n"
      "    else {\n"
      "      y = z + x; }\n"
      "    z = z + x + i;\n"
      "    call q;\n"
      "    i = i - 1; }\n"
      "  call p; }\n"
      "\n"
      "procedure p {\n"
      "  if (x<0) then {\n"
      "    while (i>0) {\n"
      "      x = z * 3 + 2 * y;\n"
      "      call q;\n"
      "      i = i - 1; }\n"
      "    x = x + 1;\n"
      "    z = x + z; }\n"
      "  else {\n"
      "    z = 1; }\n"
      "  z = z + x + i; }\n"
      "\n"
      "procedure q {\n"
      "  if (x==1) then {\n"
      "    z = x + 1; }\n"
      "  else {\n"
      "    x = z + x; } }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  SECTION("Iter 1 test (only var and const)") {
    int xIdx = pkb->getIndexOf(TableType::VAR_TABLE, "x");
    int yIdx = pkb->getIndexOf(TableType::VAR_TABLE, "y");
    int zIdx = pkb->getIndexOf(TableType::VAR_TABLE, "z");
    int iIdx = pkb->getIndexOf(TableType::VAR_TABLE, "i");

    // ===============
    // full expr
    // ===============
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       xIdx, "[2]") == SetOfStmts{1});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       zIdx, "[3]") == SetOfStmts{2});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       iIdx, "[5]") == SetOfStmts{3});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       xIdx, "[x][1]-") == SetOfStmts{5});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       zIdx, "[x][1]+") == SetOfStmts{7, 23});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       yIdx, "[z][x]+") == SetOfStmts{8});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       zIdx,
                                       "[z][x]+[i]+") == SetOfStmts{9, 21});

    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                "[2]") == SetOfInts{xIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                "[3]") == SetOfInts{zIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                "[5]") == SetOfInts{iIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                "[x][1]-") == SetOfInts{xIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                "[x][1]+") == SetOfInts{xIdx, zIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                "[z][x]+") == SetOfInts{xIdx, yIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                "[z][x]+[i]+") == SetOfInts{zIdx});

    // ===============
    // sub expr
    // ===============
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR,
                                       xIdx, "[2]") == SetOfStmts{1, 15});

    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[2]") ==
            SetOfInts{xIdx});

    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[x]") ==
            SetOfInts{xIdx, yIdx, zIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[1]") ==
            SetOfInts{xIdx, iIdx, zIdx});
    REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[z]") ==
            SetOfInts{xIdx, yIdx, zIdx});
  }
}

TEST_CASE("[DE][Expr Pattern] expr as a factor") {
  string program =
      "procedure Example {              "
      "  x = 1 + (2 + 3 + 4) + 5;       "
      "}                                ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  string s23 = "[2][3]+";
  string s234 = s23 + "[4]" + "+";
  string s1234 = "[1]" + s234 + "+";
  string s12345 = s1234 + "[5]" + "+";

  REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[1]")
              .size() != 0);
  REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[2]")
              .size() != 0);
  REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[3]")
              .size() != 0);
  REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[4]")
              .size() != 0);
  REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[5]")
              .size() != 0);
  REQUIRE(
      pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s23).size() !=
      0);
  REQUIRE(
      pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s234).size() !=
      0);
  REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s1234)
              .size() != 0);
  REQUIRE(pkb->getVarsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s12345)
              .size() != 0);
}

TEST_CASE("[ExprParser] compare ASTs of diff expr with same AST") {
  string program1 =
      "procedure Example {\n"
      "  x12y = (c%1 -(2/b - 3 *4) / (((5)))+((a%6))+(7)); }\n"
      "\n";

  ProgramAST* ast1 = Parser().Parse(Tokenizer::TokenizeProgramString(program1));
  StmtAST* stmt1 = ast1->ProcedureList[0]->StmtList[0];
  auto assignStmt1 = dynamic_cast<AssignStmtAST*>(stmt1);
  string patternStrRepr1 = assignStmt1->Expr->GetFullExprPatternStr();

  string program2 =
      "procedure Example {\n"
      "  x12y = ((c%1) -(2/((b)) - ((((3)))) *4) / 5+a%6+7); }\n"
      "\n";
  ProgramAST* ast2 = Parser().Parse(Tokenizer::TokenizeProgramString(program2));
  StmtAST* stmt2 = ast2->ProcedureList[0]->StmtList[0];
  auto assignStmt2 = dynamic_cast<AssignStmtAST*>(stmt2);
  string patternStrRepr2 = assignStmt2->Expr->GetFullExprPatternStr();

  CHECK(patternStrRepr1 == patternStrRepr2);
}

TEST_CASE("[ExprParser] test variable/constant boundry") {
  string program1 =
      "procedure Example {\n"
      "  b = aa + a; }\n"
      "\n";

  ProgramAST* ast1 = Parser().Parse(Tokenizer::TokenizeProgramString(program1));
  StmtAST* stmt1 = ast1->ProcedureList[0]->StmtList[0];
  auto assignStmt1 = dynamic_cast<AssignStmtAST*>(stmt1);
  string patternStrRepr1 = assignStmt1->Expr->GetFullExprPatternStr();

  string program2 =
      "procedure Example {\n"
      "  b = a + aa; }\n"
      "\n";
  ProgramAST* ast2 = Parser().Parse(Tokenizer::TokenizeProgramString(program2));
  StmtAST* stmt2 = ast2->ProcedureList[0]->StmtList[0];
  auto assignStmt2 = dynamic_cast<AssignStmtAST*>(stmt2);
  string patternStrRepr2 = assignStmt2->Expr->GetFullExprPatternStr();

  REQUIRE(patternStrRepr1 == "[aa][a]+");
  REQUIRE(patternStrRepr2 == "[a][aa]+");
  CHECK(patternStrRepr1 != patternStrRepr2);
}
