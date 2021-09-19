
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
    int exampleIdx = pkb->getProcIndex("Example");
    int pIdx = pkb->getProcIndex("p");
    int qIdx = pkb->getProcIndex("q");

    REQUIRE(pkb->getAllProcedures() ==
            UNO_SET_OF_STMT_NO{exampleIdx, pIdx, qIdx});
  }

  SECTION("Consts") {
    int c0idx = pkb->getConstIndex("0");
    int c1idx = pkb->getConstIndex("1");
    int c2idx = pkb->getConstIndex("2");
    int c3idx = pkb->getConstIndex("3");
    int c5idx = pkb->getConstIndex("5");

    REQUIRE(pkb->getAllConstants() ==
            UNO_SET_OF_STMT_NO{c0idx, c1idx, c2idx, c3idx, c5idx});
  }

  SECTION("Variables") {
    int xIdx = pkb->getVarIndex("x");
    int yIdx = pkb->getVarIndex("y");
    int zIdx = pkb->getVarIndex("z");
    int iIdx = pkb->getVarIndex("i");
    REQUIRE(pkb->getAllVariables() ==
            unordered_set<CONST_IDX>{xIdx, yIdx, zIdx, iIdx});
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
  int aIdx = pkb->getVarIndex("a");
  int bIdx = pkb->getVarIndex("b");
  int cIdx = pkb->getVarIndex("c");
  int dIdx = pkb->getVarIndex("d");
  int eIdx = pkb->getVarIndex("e");
  int fIdx = pkb->getVarIndex("f");
  int gIdx = pkb->getVarIndex("g");
  int hIdx = pkb->getVarIndex("h");
  int iIdx = pkb->getVarIndex("i");
  int jIdx = pkb->getVarIndex("j");
  int kIdx = pkb->getVarIndex("k");
  int lIdx = pkb->getVarIndex("l");
  int mIdx = pkb->getVarIndex("m");
  int nIdx = pkb->getVarIndex("n");
  int oIdx = pkb->getVarIndex("o");
  int pIdx = pkb->getVarIndex("p");
  int qIdx = pkb->getVarIndex("q");

  int c0idx = pkb->getConstIndex("0");
  int c1idx = pkb->getConstIndex("1");
  int c2idx = pkb->getConstIndex("2");
  int c3idx = pkb->getConstIndex("3");
  int c4idx = pkb->getConstIndex("4");
  int c5idx = pkb->getConstIndex("5");
  int c6idx = pkb->getConstIndex("6");
  int c7idx = pkb->getConstIndex("7");
  int c8idx = pkb->getConstIndex("8");
  int c9idx = pkb->getConstIndex("9");
  int c10idx = pkb->getConstIndex("10");
  int c11idx = pkb->getConstIndex("11");
  int c12idx = pkb->getConstIndex("12");
  int c13idx = pkb->getConstIndex("13");
  int c14idx = pkb->getConstIndex("14");
  int c15idx = pkb->getConstIndex("15");
  int c16idx = pkb->getConstIndex("16");
  int c17idx = pkb->getConstIndex("17");
  int c18idx = pkb->getConstIndex("18");

  REQUIRE(pkb->getAllVariables() ==
          unordered_set<VAR_IDX>{aIdx, bIdx, cIdx, dIdx, eIdx, fIdx, gIdx, hIdx,
                                 iIdx, jIdx, kIdx, lIdx, mIdx, nIdx, oIdx, pIdx,
                                 qIdx});

  REQUIRE(pkb->getAllConstants() ==
          unordered_set<CONST_IDX>{c0idx, c1idx, c2idx, c3idx, c4idx, c5idx,
                                   c6idx, c7idx, c8idx, c9idx, c10idx, c11idx,
                                   c12idx, c13idx, c14idx, c15idx, c16idx,
                                   c17idx, c18idx});
}
