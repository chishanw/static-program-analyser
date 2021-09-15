
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[DE][Modifies R/S] sample source") {
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

  SECTION("Modifies R/S") {
    REQUIRE(pkb->getModifiesS("x") == UNO_SET_OF_STMT_NO({1, 4, 5, 13, 14,
        15, 18, 22, 24}));
    REQUIRE(pkb->getModifiesS("i") == UNO_SET_OF_STMT_NO({3, 4, 11,
        13, 14, 17}));
    REQUIRE(pkb->getModifiesS("z") == UNO_SET_OF_STMT_NO({2, 4, 6, 7, 9,
        13, 19, 20, 21, 22, 23}));
    REQUIRE(pkb->getModifiesS("y") == UNO_SET_OF_STMT_NO({4, 6, 8}));
  }
}
