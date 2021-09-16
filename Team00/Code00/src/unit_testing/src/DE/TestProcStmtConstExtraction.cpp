
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;
using namespace Catch;

TEST_CASE("[DE][Proc & Stmt & Const] sample source") {
  REQUIRE(1 == 1);

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

  ProgramAST* ast =
      Parser(false).Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  SECTION("Different type of statements") {
    REQUIRE(
        pkb->getAllStmts() ==
        UNO_SET_OF_STMT_NO({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                            13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24}));
    REQUIRE(pkb->getAllReadStmts() == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getAllPrintStmts() == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getAllCallStmts() == UNO_SET_OF_STMT_NO({10, 12, 16}));
    REQUIRE(pkb->getAllWhileStmts() == UNO_SET_OF_STMT_NO({4, 14}));
    REQUIRE(pkb->getAllIfStmts() == UNO_SET_OF_STMT_NO({6, 13, 22}));
    REQUIRE(pkb->getAllAssignStmts() ==
            UNO_SET_OF_STMT_NO(
                {1, 2, 3, 5, 7, 8, 9, 11, 15, 17, 18, 19, 20, 21, 23, 24}));
  }

  SECTION("Procedures") {
    int exampleIdx = pkb->procTable.getProcIndex("Example");
    int pIdx = pkb->procTable.getProcIndex("p");
    int qIdx = pkb->procTable.getProcIndex("q");

    REQUIRE(pkb->getAllProcedures() ==
            UNO_SET_OF_STMT_NO{exampleIdx, pIdx, qIdx});
  }

  SECTION("Consts") {
    REQUIRE(pkb->getAllConstants() == UNO_SET_OF_STMT_NO({0, 1, 2, 3, 5}));
  }
}
