
#include <Common/Tokenizer.h>
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>

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
    REQUIRE(pkb->getAllStmts(DesignEntity::STATEMENT) ==
            SetOfStmts({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24}));
    REQUIRE(pkb->getAllStmts(DesignEntity::READ) == SetOfStmts({}));
    REQUIRE(pkb->getAllStmts(DesignEntity::PRINT) == SetOfStmts({}));
    REQUIRE(pkb->getAllStmts(DesignEntity::CALL) == SetOfStmts({10, 12, 16}));
    REQUIRE(pkb->getAllStmts(DesignEntity::WHILE) == SetOfStmts({4, 14}));
    REQUIRE(pkb->getAllStmts(DesignEntity::IF) == SetOfStmts({6, 13, 22}));
    REQUIRE(
        pkb->getAllStmts(DesignEntity::ASSIGN) ==
        SetOfStmts({1, 2, 3, 5, 7, 8, 9, 11, 15, 17, 18, 19, 20, 21, 23, 24}));
  }

  SECTION("Procedures") {
    int exampleIdx = pkb->getIndexOf(TableType::PROC_TABLE, "Example");
    int pIdx = pkb->getIndexOf(TableType::PROC_TABLE, "p");
    int qIdx = pkb->getIndexOf(TableType::PROC_TABLE, "q");

    REQUIRE(pkb->getAllElementsAt(TableType::PROC_TABLE) ==
            SetOfStmts{exampleIdx, pIdx, qIdx});
  }

  SECTION("Consts") {
    int c0idx = pkb->getIndexOf(TableType::CONST_TABLE, "0");
    int c1idx = pkb->getIndexOf(TableType::CONST_TABLE, "1");
    int c2idx = pkb->getIndexOf(TableType::CONST_TABLE, "2");
    int c3idx = pkb->getIndexOf(TableType::CONST_TABLE, "3");
    int c5idx = pkb->getIndexOf(TableType::CONST_TABLE, "5");

    REQUIRE(pkb->getAllElementsAt(TableType::CONST_TABLE) ==
            SetOfStmts{c0idx, c1idx, c2idx, c3idx, c5idx});
  }

  SECTION("Variables") {
    int xIdx = pkb->getIndexOf(TableType::VAR_TABLE, "x");
    int yIdx = pkb->getIndexOf(TableType::VAR_TABLE, "y");
    int zIdx = pkb->getIndexOf(TableType::VAR_TABLE, "z");
    int iIdx = pkb->getIndexOf(TableType::VAR_TABLE, "i");
    REQUIRE(pkb->getAllElementsAt(TableType::VAR_TABLE) ==
            unordered_set<ConstIdx>{xIdx, yIdx, zIdx, iIdx});
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
  int aIdx = pkb->getIndexOf(TableType::VAR_TABLE, "a");
  int bIdx = pkb->getIndexOf(TableType::VAR_TABLE, "b");
  int cIdx = pkb->getIndexOf(TableType::VAR_TABLE, "c");
  int dIdx = pkb->getIndexOf(TableType::VAR_TABLE, "d");
  int eIdx = pkb->getIndexOf(TableType::VAR_TABLE, "e");
  int fIdx = pkb->getIndexOf(TableType::VAR_TABLE, "f");
  int gIdx = pkb->getIndexOf(TableType::VAR_TABLE, "g");
  int hIdx = pkb->getIndexOf(TableType::VAR_TABLE, "h");
  int iIdx = pkb->getIndexOf(TableType::VAR_TABLE, "i");
  int jIdx = pkb->getIndexOf(TableType::VAR_TABLE, "j");
  int kIdx = pkb->getIndexOf(TableType::VAR_TABLE, "k");
  int lIdx = pkb->getIndexOf(TableType::VAR_TABLE, "l");
  int mIdx = pkb->getIndexOf(TableType::VAR_TABLE, "m");
  int nIdx = pkb->getIndexOf(TableType::VAR_TABLE, "n");
  int oIdx = pkb->getIndexOf(TableType::VAR_TABLE, "o");
  int pIdx = pkb->getIndexOf(TableType::VAR_TABLE, "p");
  int qIdx = pkb->getIndexOf(TableType::VAR_TABLE, "q");

  int c0idx = pkb->getIndexOf(TableType::CONST_TABLE, "0");
  int c1idx = pkb->getIndexOf(TableType::CONST_TABLE, "1");
  int c2idx = pkb->getIndexOf(TableType::CONST_TABLE, "2");
  int c3idx = pkb->getIndexOf(TableType::CONST_TABLE, "3");
  int c4idx = pkb->getIndexOf(TableType::CONST_TABLE, "4");
  int c5idx = pkb->getIndexOf(TableType::CONST_TABLE, "5");
  int c6idx = pkb->getIndexOf(TableType::CONST_TABLE, "6");
  int c7idx = pkb->getIndexOf(TableType::CONST_TABLE, "7");
  int c8idx = pkb->getIndexOf(TableType::CONST_TABLE, "8");
  int c9idx = pkb->getIndexOf(TableType::CONST_TABLE, "9");
  int c10idx = pkb->getIndexOf(TableType::CONST_TABLE, "10");
  int c11idx = pkb->getIndexOf(TableType::CONST_TABLE, "11");
  int c12idx = pkb->getIndexOf(TableType::CONST_TABLE, "12");
  int c13idx = pkb->getIndexOf(TableType::CONST_TABLE, "13");
  int c14idx = pkb->getIndexOf(TableType::CONST_TABLE, "14");
  int c15idx = pkb->getIndexOf(TableType::CONST_TABLE, "15");
  int c16idx = pkb->getIndexOf(TableType::CONST_TABLE, "16");
  int c17idx = pkb->getIndexOf(TableType::CONST_TABLE, "17");
  int c18idx = pkb->getIndexOf(TableType::CONST_TABLE, "18");

  REQUIRE(pkb->getAllElementsAt(TableType::VAR_TABLE) ==
          unordered_set<VarIdx>{aIdx, bIdx, cIdx, dIdx, eIdx, fIdx, gIdx, hIdx,
                                iIdx, jIdx, kIdx, lIdx, mIdx, nIdx, oIdx, pIdx,
                                qIdx});

  REQUIRE(pkb->getAllElementsAt(TableType::CONST_TABLE) ==
          unordered_set<ConstIdx>{c0idx, c1idx, c2idx, c3idx, c4idx, c5idx,
                                  c6idx, c7idx, c8idx, c9idx, c10idx, c11idx,
                                  c12idx, c13idx, c14idx, c15idx, c16idx,
                                  c17idx, c18idx});
}

TEST_CASE("Multiple procedures with same name") {
  string program =
      "procedure a {\n"
      "  x = 0; }\n"
      "procedure a {\n"
      "  y = 0; }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  REQUIRE_THROWS_WITH(de.Extract(ast), "2 procedures with same name detected.");
}
