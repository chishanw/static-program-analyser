
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[DE][Calls(*) R/S] sample source") {
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
      "    call p;\n"
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

  SECTION("Calls R/S") {
    REQUIRE(pkb->isCallStmt(10));
    REQUIRE(pkb->isCallStmt(12));
    REQUIRE(pkb->isCallStmt(16));
    REQUIRE(!pkb->isCallStmt(17));
    REQUIRE(pkb->isCalls("Example", "p"));
    REQUIRE(!pkb->isCalls("Example", "q"));
    REQUIRE(pkb->isCalls("p", "q"));
    REQUIRE(!pkb->isCallStmt(25));  // out of bound test, last stmt is #24
    REQUIRE(!pkb->isCallStmt(30));  // out of bound test, last stmt is #24
  }

  SECTION("Calls* R/S") {
    REQUIRE(pkb->isCallsT("Example", "p"));
    REQUIRE(pkb->isCallsT("Example", "q"));
    REQUIRE(pkb->isCallsT("p", "q"));
  }
}
