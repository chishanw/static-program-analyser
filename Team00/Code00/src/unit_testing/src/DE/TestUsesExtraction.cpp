
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
    // TODO(gf): add tests once pkb is ready
    // Uses r/s result: <4, i>
    // Uses r/s result: <5, x>
    // Uses r/s result: <6, x>
    // Uses r/s result: <7, x>
    // Uses r/s result: <8, z>
    // Uses r/s result: <8, x>
    // Uses r/s result: <9, z>
    // Uses r/s result: <9, x>
    // Uses r/s result: <9, i>
    // Uses r/s result: <11, i>
    // Uses r/s result: <13, x>
    // Uses r/s result: <14, i>
    // Uses r/s result: <15, z>
    // Uses r/s result: <15, y>
    // Uses r/s result: <17, i>
    // Uses r/s result: <18, x>
    // Uses r/s result: <19, x>
    // Uses r/s result: <19, z>
    // Uses r/s result: <21, z>
    // Uses r/s result: <21, x>
    // Uses r/s result: <21, i>
    // Uses r/s result: <22, x>
    // Uses r/s result: <23, x>
    // Uses r/s result: <24, z>
    // Uses r/s result: <24, x>
  }
}
