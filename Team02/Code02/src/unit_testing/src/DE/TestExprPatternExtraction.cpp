
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
    int xIdx = pkb->getVarIndex("x");
    int yIdx = pkb->getVarIndex("y");
    int zIdx = pkb->getVarIndex("z");
    int iIdx = pkb->getVarIndex("i");

    // ===============
    // full expr
    // ===============
    REQUIRE(pkb->getAssignForVarAndFullExpr("x", "[2]") ==
            UNO_SET_OF_STMT_NO{1});
    REQUIRE(pkb->getAssignForVarAndFullExpr("z", "[3]") ==
            UNO_SET_OF_STMT_NO{2});
    REQUIRE(pkb->getAssignForVarAndFullExpr("i", "[5]") ==
            UNO_SET_OF_STMT_NO{3});
    REQUIRE(pkb->getAssignForVarAndFullExpr("x", "[[x]-[1]]") ==
            UNO_SET_OF_STMT_NO{5});
    REQUIRE(pkb->getAssignForVarAndFullExpr("z", "[[x]+[1]]") ==
            UNO_SET_OF_STMT_NO{7, 23});
    REQUIRE(pkb->getAssignForVarAndFullExpr("y", "[[z]+[x]]") ==
            UNO_SET_OF_STMT_NO{8});
    REQUIRE(pkb->getAssignForVarAndFullExpr("z", "[[[z]+[x]]+[i]]") ==
            UNO_SET_OF_STMT_NO{9, 21});

    REQUIRE(pkb->getAssignForFullExpr("[2]") == UNO_SET_OF_STMT_NO{1});
    REQUIRE(pkb->getAssignForFullExpr("[3]") == UNO_SET_OF_STMT_NO{2});
    REQUIRE(pkb->getAssignForFullExpr("[5]") == UNO_SET_OF_STMT_NO{3});
    REQUIRE(pkb->getAssignForFullExpr("[[x]-[1]]") == UNO_SET_OF_STMT_NO{5});
    REQUIRE(pkb->getAssignForFullExpr("[[x]+[1]]") ==
            UNO_SET_OF_STMT_NO{7, 18, 23});
    REQUIRE(pkb->getAssignForFullExpr("[[z]+[x]]") ==
            UNO_SET_OF_STMT_NO{8, 24});
    REQUIRE(pkb->getAssignForFullExpr("[[[z]+[x]]+[i]]") ==
            UNO_SET_OF_STMT_NO{9, 21});

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[2]"),
                 VectorContains(vector<int>{1, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[3]"),
                 VectorContains(vector<int>{2, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[5]"),
                 VectorContains(vector<int>{3, iIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[x]-[1]]"),
                 VectorContains(vector<int>{5, xIdx}));

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[x]+[1]]"),
                 VectorContains(vector<int>{7, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[x]+[1]]"),
                 VectorContains(vector<int>{18, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[x]+[1]]"),
                 VectorContains(vector<int>{23, zIdx}));

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[z]+[x]]"),
                 VectorContains(vector<int>{8, yIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[z]+[x]]"),
                 VectorContains(vector<int>{24, xIdx}));

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[[z]+[x]]+[i]]"),
                 VectorContains(vector<int>{9, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("[[[z]+[x]]+[i]]"),
                 VectorContains(vector<int>{21, zIdx}));

    // ===============
    // sub expr
    // ===============

    REQUIRE(pkb->getAssignForVarAndSubExpr("x", "[2]") ==
            UNO_SET_OF_STMT_NO{1, 15});

    REQUIRE(pkb->getAssignForSubExpr("[2]") == UNO_SET_OF_STMT_NO{1, 15});

    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[2]"),
                 VectorContains(vector<int>{1, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[2]"),
                 VectorContains(vector<int>{15, xIdx}));

    REQUIRE(pkb->getAssignForSubExpr("[x]") ==
            UNO_SET_OF_STMT_NO{5, 7, 8, 9, 18, 19, 21, 23, 24});
    REQUIRE(pkb->getAssignForSubExpr("[1]") ==
            UNO_SET_OF_STMT_NO{5, 7, 11, 17, 18, 20, 23});
    REQUIRE(pkb->getAssignForSubExpr("[z]") ==
            UNO_SET_OF_STMT_NO{8, 9, 15, 19, 21, 24});

    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[z]"),
                 VectorContains(vector<int>{8, yIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[z]"),
                 VectorContains(vector<int>{9, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[z]"),
                 VectorContains(vector<int>{15, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[z]"),
                 VectorContains(vector<int>{19, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[z]"),
                 VectorContains(vector<int>{21, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("[z]"),
                 VectorContains(vector<int>{24, xIdx}));
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

  REQUIRE(pkb->getAssignVarPairsForSubExpr("[1]").size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr("[2]").size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr("[3]").size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr("[4]").size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr("[5]").size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr(s23).size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr(s234).size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr(s1234).size() != 0);
  REQUIRE(pkb->getAssignVarPairsForSubExpr(s12345).size() != 0);
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
