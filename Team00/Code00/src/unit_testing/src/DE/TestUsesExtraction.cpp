
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[DE][Uses R/S] sample source") {
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

  SECTION("Uses R/S") {
    REQUIRE(pkb->isUsesS(4, "i"));  // container stmt from 4 - 11
    REQUIRE(pkb->isUsesS(4, "x"));
    REQUIRE(pkb->isUsesS(4, "z"));
    REQUIRE(pkb->isUsesS(5, "x"));
    REQUIRE(pkb->isUsesS(6, "x"));  // container stmt from 6 - 8
    REQUIRE(pkb->isUsesS(6, "z"));
    REQUIRE(pkb->isUsesS(7, "x"));
    REQUIRE(pkb->isUsesS(8, "z"));
    REQUIRE(pkb->isUsesS(8, "x"));
    REQUIRE(pkb->isUsesS(9, "z"));
    REQUIRE(pkb->isUsesS(9, "x"));
    REQUIRE(pkb->isUsesS(9, "i"));
    REQUIRE(pkb->isUsesS(11, "i"));

    REQUIRE(pkb->isUsesS(13, "x"));  // container stmt from 13 - 20
    REQUIRE(pkb->isUsesS(13, "i"));
    REQUIRE(pkb->isUsesS(13, "z"));
    REQUIRE(pkb->isUsesS(13, "y"));

    REQUIRE(pkb->isUsesS(14, "i"));  // container stmt from 14 - 17
    REQUIRE(pkb->isUsesS(14, "z"));
    REQUIRE(pkb->isUsesS(14, "y"));
    REQUIRE(pkb->isUsesS(15, "z"));
    REQUIRE(pkb->isUsesS(15, "y"));
    REQUIRE(pkb->isUsesS(17, "i"));

    REQUIRE(pkb->isUsesS(18, "x"));
    REQUIRE(pkb->isUsesS(19, "x"));
    REQUIRE(pkb->isUsesS(19, "z"));
    REQUIRE(pkb->isUsesS(21, "z"));
    REQUIRE(pkb->isUsesS(21, "x"));
    REQUIRE(pkb->isUsesS(21, "i"));

    REQUIRE(pkb->isUsesS(22, "x"));  // container stmt from 22 - 24
    REQUIRE(pkb->isUsesS(22, "z"));
    REQUIRE(pkb->isUsesS(23, "x"));
    REQUIRE(pkb->isUsesS(24, "z"));
    REQUIRE(pkb->isUsesS(24, "x"));
  }
}
