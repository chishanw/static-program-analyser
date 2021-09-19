
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[DE][Parent(*) R/S] sample source") {
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

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  SECTION("Parent R/S") {
    REQUIRE(pkb->getParent(1) == -1);
    REQUIRE(pkb->getParent(2) == -1);
    REQUIRE(pkb->getParent(3) == -1);
    REQUIRE(pkb->getParent(4) == -1);
    REQUIRE(pkb->isParent(4, 5));
    REQUIRE(pkb->isParent(4, 6));
    REQUIRE(pkb->isParent(6, 7));
    REQUIRE(pkb->isParent(6, 8));
    REQUIRE(pkb->isParent(4, 9));
    REQUIRE(pkb->isParent(4, 10));
    REQUIRE(pkb->isParent(4, 11));
    REQUIRE(pkb->getParent(12) == -1);
    REQUIRE(pkb->getParent(13) == -1);
    REQUIRE(pkb->isParent(13, 14));
    REQUIRE(pkb->isParent(14, 15));
    REQUIRE(pkb->isParent(14, 16));
    REQUIRE(pkb->isParent(14, 17));
    REQUIRE(pkb->isParent(13, 18));
    REQUIRE(pkb->isParent(13, 19));
    REQUIRE(pkb->isParent(13, 20));
    REQUIRE(pkb->getParent(21) == -1);
    REQUIRE(pkb->getParent(22) == -1);
    REQUIRE(pkb->isParent(22, 23));
    REQUIRE(pkb->isParent(22, 24));
    REQUIRE(pkb->getParent(25) == -1);  // out of bound test, last stmt is #24
    REQUIRE(pkb->getParent(30) == -1);  // out of bound test, last stmt is #24
  }

  SECTION("Parent* R/S") {
    REQUIRE(pkb->getChildrenT(4) ==
            UNO_SET_OF_STMT_NO({5, 6, 7, 8, 9, 10, 11}));

    REQUIRE(pkb->getChildrenT(6) == UNO_SET_OF_STMT_NO({7, 8}));

    REQUIRE(pkb->getChildrenT(13) ==
            UNO_SET_OF_STMT_NO({14, 15, 16, 17, 18, 19, 20}));

    REQUIRE(pkb->getChildrenT(14) == UNO_SET_OF_STMT_NO({15, 16, 17}));

    REQUIRE(pkb->getChildrenT(22) == UNO_SET_OF_STMT_NO({23, 24}));
  }
}
