
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

  SECTION("Variables") {
    int xIdx = pkb->varTable.getVarIndex("x");
    int yIdx = pkb->varTable.getVarIndex("y");
    int zIdx = pkb->varTable.getVarIndex("z");
    int iIdx = pkb->varTable.getVarIndex("i");
    REQUIRE(pkb->getAllVariables() ==
            unordered_set<VAR_IDX>{xIdx, yIdx, zIdx, iIdx});
  }
}

TEST_CASE("[DE][GetAllVarNames & GetAllConsts] complex cond expr") {
  string program =
      "procedure Example {"
      "  if (((0 % 1 + a) == 2) && (((b * 3 - c) / 4 + d % 5 >= e) || (6 - 7 + "
      "f * g / 8 % h == 9))) then {"
      "    i = 10;"
      "  } else {"
      "    j = 11;"
      "  }"
      "  while ((12 + k) / 13 * l % (14 / m - n) != (15 + o) * (16 + p) ) {"
      "    q = 17 + 18;"
      "  }"
      "}";

  ProgramAST* ast =
      Parser(false).Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  // 17 variables [0, 1, ..., 16]
  int aIdx = pkb->varTable.getVarIndex("a");
  int bIdx = pkb->varTable.getVarIndex("b");
  int cIdx = pkb->varTable.getVarIndex("c");
  int dIdx = pkb->varTable.getVarIndex("d");
  int eIdx = pkb->varTable.getVarIndex("e");
  int fIdx = pkb->varTable.getVarIndex("f");
  int gIdx = pkb->varTable.getVarIndex("g");
  int hIdx = pkb->varTable.getVarIndex("h");
  int iIdx = pkb->varTable.getVarIndex("i");
  int jIdx = pkb->varTable.getVarIndex("j");
  int kIdx = pkb->varTable.getVarIndex("k");
  int lIdx = pkb->varTable.getVarIndex("l");
  int mIdx = pkb->varTable.getVarIndex("m");
  int nIdx = pkb->varTable.getVarIndex("n");
  int oIdx = pkb->varTable.getVarIndex("o");
  int pIdx = pkb->varTable.getVarIndex("p");
  int qIdx = pkb->varTable.getVarIndex("q");

  REQUIRE(pkb->getAllVariables() ==
          unordered_set<VAR_IDX>{aIdx, bIdx, cIdx, dIdx, eIdx, fIdx, gIdx, hIdx,
                                 iIdx, jIdx, kIdx, lIdx, mIdx, nIdx, oIdx, pIdx,
                                 qIdx});

  REQUIRE(pkb->getAllConstants() == unordered_set<int>{0, 1, 2, 3, 4, 5, 6, 7,
                                                       8, 9, 10, 11, 12, 13, 14,
                                                       15, 16, 17, 18});
}
