
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
                                       "x", "[2]") == SetOfStmts{1});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "z", "[3]") == SetOfStmts{2});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "i", "[5]") == SetOfStmts{3});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "x", "[[x]-[1]]") == SetOfStmts{5});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "z", "[[x]+[1]]") == SetOfStmts{7, 23});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "y", "[[z]+[x]]") == SetOfStmts{8});
    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "z",
                                       "[[[z]+[x]]+[i]]") == SetOfStmts{9, 21});

    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 "[2]") == SetOfStmts{1});
    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 "[3]") == SetOfStmts{2});
    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 "[5]") == SetOfStmts{3});
    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 "[[x]-[1]]") == SetOfStmts{5});
    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 "[[x]+[1]]") == SetOfStmts{7, 18, 23});
    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 "[[z]+[x]]") == SetOfStmts{8, 24});
    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                 "[[[z]+[x]]+[i]]") == SetOfStmts{9, 21});

    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[2]")
                .count(vector<int>{1, xIdx}) != 0);
    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[3]")
                .count(vector<int>{2, zIdx}) != 0);
    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[5]")
                .count(vector<int>{3, iIdx}) != 0);
    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[x]-[1]]")
                .count(vector<int>{5, xIdx}) != 0);

    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[x]+[1]]")
                .count(vector<int>{7, zIdx}) != 0);
    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[x]+[1]]")
                .count(vector<int>{18, xIdx}) != 0);
    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[x]+[1]]")
                .count(vector<int>{23, zIdx}) != 0);

    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[z]+[x]]")
                .count(vector<int>{8, yIdx}) != 0);
    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[z]+[x]]")
                .count(vector<int>{24, xIdx}) != 0);

    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[[z]+[x]]+[i]]")
                .count(vector<int>{9, zIdx}) != 0);
    REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_FULL_EXPR,
                                       "[[[z]+[x]]+[i]]")
                .count(vector<int>{21, zIdx}) != 0);

    // ===============
    // sub expr
    // ===============

    REQUIRE(pkb->getStmtsForVarAndExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR,
                                       "x", "[2]") == SetOfStmts{1, 15});

    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR,
                                 "[2]") == SetOfStmts{1, 15});

    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[2]")
            .count(vector<int>{1, xIdx}) != 0);
    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[2]")
            .count(vector<int>{15, xIdx}) != 0);

    REQUIRE(
        pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[x]") ==
        SetOfStmts{5, 7, 8, 9, 18, 19, 21, 23, 24});
    REQUIRE(
        pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[1]") ==
        SetOfStmts{5, 7, 11, 17, 18, 20, 23});
    REQUIRE(pkb->getStmtsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR,
                                 "[z]") == SetOfStmts{8, 9, 15, 19, 21, 24});

    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[z]")
            .count(vector<int>{8, yIdx}) != 0);
    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[z]")
            .count(vector<int>{9, zIdx}) != 0);
    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[z]")
            .count(vector<int>{15, xIdx}) != 0);
    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[z]")
            .count(vector<int>{19, zIdx}) != 0);
    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[z]")
            .count(vector<int>{21, zIdx}) != 0);
    REQUIRE(
        pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[z]")
            .count(vector<int>{24, xIdx}) != 0);
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

  string s23 = "[[2]+[3]]";
  string s234 = string("[") + s23 + "+" + "[4]" + "]";
  string s1234 = string("[") + "[1]" + "+" + s234 + "]";
  string s12345 = string("[") + s1234 + "+" + "[5]" + "]";

  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[1]")
          .size() != 0);
  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[2]")
          .size() != 0);
  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[3]")
          .size() != 0);
  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[4]")
          .size() != 0);
  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, "[5]")
          .size() != 0);
  REQUIRE(pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s23)
              .size() != 0);
  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s234)
          .size() != 0);
  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s1234)
          .size() != 0);
  REQUIRE(
      pkb->getVarMappingsForExpr(RelationshipType::PTT_ASSIGN_SUB_EXPR, s12345)
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
