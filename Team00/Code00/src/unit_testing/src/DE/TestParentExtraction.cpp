
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

  // TODO(gf): add tests once pkb is ready

  SECTION("Parent R/S") {
    // Parent r/s result: <4, 5>
    // Parent r/s result: <4, 6>
    // Parent r/s result: <6, 7>
    // Parent r/s result: <6, 8>
    // Parent r/s result: <4, 9>
    // Parent r/s result: <4, 10>
    // Parent r/s result: <4, 11>
    // Parent r/s result: <13, 14>
    // Parent r/s result: <14, 15>
    // Parent r/s result: <14, 16>
    // Parent r/s result: <14, 17>
    // Parent r/s result: <13, 18>
    // Parent r/s result: <13, 19>
    // Parent r/s result: <13, 20>
    // Parent r/s result: <22, 23>
    // Parent r/s result: <22, 24>
  }

  SECTION("Parent* R/S") {
    // Parent* r/s result: <6, 7>
    // Parent* r/s result: <6, 8>
    // Parent* r/s result: <4, 5>
    // Parent* r/s result: <4, 6>
    // Parent* r/s result: <4, 7>
    // Parent* r/s result: <4, 8>
    // Parent* r/s result: <4, 9>
    // Parent* r/s result: <4, 10>
    // Parent* r/s result: <4, 11>
    // Parent* r/s result: <14, 15>
    // Parent* r/s result: <14, 16>
    // Parent* r/s result: <14, 17>
    // Parent* r/s result: <13, 14>
    // Parent* r/s result: <13, 15>
    // Parent* r/s result: <13, 16>
    // Parent* r/s result: <13, 17>
    // Parent* r/s result: <13, 18>
    // Parent* r/s result: <13, 19>
    // Parent* r/s result: <13, 20>
    // Parent* r/s result: <22, 23>
    // Parent* r/s result: <22, 24>
  }
}
