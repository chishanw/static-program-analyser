
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

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
    REQUIRE(pkb->getAssignForVarAndFullExpr("x", "2") == UNO_SET_OF_STMT_NO{1});
    REQUIRE(pkb->getAssignForVarAndFullExpr("z", "3") == UNO_SET_OF_STMT_NO{2});
    REQUIRE(pkb->getAssignForVarAndFullExpr("i", "5") == UNO_SET_OF_STMT_NO{3});
    REQUIRE(pkb->getAssignForVarAndFullExpr("x", "x-1") ==
            UNO_SET_OF_STMT_NO{5});
    REQUIRE(pkb->getAssignForVarAndFullExpr("z", "x+1") ==
            UNO_SET_OF_STMT_NO{7, 23});
    REQUIRE(pkb->getAssignForVarAndFullExpr("y", "z+x") ==
            UNO_SET_OF_STMT_NO{8});
    REQUIRE(pkb->getAssignForVarAndFullExpr("z", "z+x+i") ==
            UNO_SET_OF_STMT_NO{9, 21});
    // TODO(gf): expr pattern string is gonna change in
    // iter2, so not gonna write too many tests for now

    REQUIRE(pkb->getAssignForFullExpr("2") == UNO_SET_OF_STMT_NO{1});
    REQUIRE(pkb->getAssignForFullExpr("3") == UNO_SET_OF_STMT_NO{2});
    REQUIRE(pkb->getAssignForFullExpr("5") == UNO_SET_OF_STMT_NO{3});
    REQUIRE(pkb->getAssignForFullExpr("x-1") == UNO_SET_OF_STMT_NO{5});
    REQUIRE(pkb->getAssignForFullExpr("x+1") == UNO_SET_OF_STMT_NO{7, 18, 23});
    REQUIRE(pkb->getAssignForFullExpr("z+x") == UNO_SET_OF_STMT_NO{8, 24});
    REQUIRE(pkb->getAssignForFullExpr("z+x+i") == UNO_SET_OF_STMT_NO{9, 21});

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("2"),
                 VectorContains(vector<int>{1, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("3"),
                 VectorContains(vector<int>{2, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("5"),
                 VectorContains(vector<int>{3, iIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("x-1"),
                 VectorContains(vector<int>{5, xIdx}));

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("x+1"),
                 VectorContains(vector<int>{7, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("x+1"),
                 VectorContains(vector<int>{18, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("x+1"),
                 VectorContains(vector<int>{23, zIdx}));

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("z+x"),
                 VectorContains(vector<int>{8, yIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("z+x"),
                 VectorContains(vector<int>{24, xIdx}));

    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("z+x+i"),
                 VectorContains(vector<int>{9, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForFullExpr("z+x+i"),
                 VectorContains(vector<int>{21, zIdx}));

    // ===============
    // sub expr
    // ===============

    REQUIRE(pkb->getAssignForVarAndSubExpr("x", "2") ==
            UNO_SET_OF_STMT_NO{1, 15});

    REQUIRE(pkb->getAssignForSubExpr("2") == UNO_SET_OF_STMT_NO{1, 15});

    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("2"),
                 VectorContains(vector<int>{1, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("2"),
                 VectorContains(vector<int>{15, xIdx}));

    REQUIRE(pkb->getAssignForSubExpr("x") ==
            UNO_SET_OF_STMT_NO{5, 7, 8, 9, 18, 19, 21, 23, 24});
    REQUIRE(pkb->getAssignForSubExpr("1") ==
            UNO_SET_OF_STMT_NO{5, 7, 11, 17, 18, 20, 23});
    REQUIRE(pkb->getAssignForSubExpr("z") ==
            UNO_SET_OF_STMT_NO{8, 9, 15, 19, 21, 24});

    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("z"),
                 VectorContains(vector<int>{8, yIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("z"),
                 VectorContains(vector<int>{9, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("z"),
                 VectorContains(vector<int>{15, xIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("z"),
                 VectorContains(vector<int>{19, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("z"),
                 VectorContains(vector<int>{21, zIdx}));
    REQUIRE_THAT(pkb->getAssignVarPairsForSubExpr("z"),
                 VectorContains(vector<int>{24, xIdx}));
  }
}