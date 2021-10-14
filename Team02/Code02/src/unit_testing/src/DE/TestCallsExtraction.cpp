
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Common/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[DE][Calls R/S] sample source") {
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

TEST_CASE("[DE][Calls(*) R/S] sample source") {
  REQUIRE(1 == 1);

  string program =
      "procedure a {\n"
      "  call b;\n"
      "  call c; }\n"
      "\n"
      "procedure b {\n"
      "  call d; }\n"
      "\n"
      "procedure c {\n"
      "  call b;\n"
      "  call e; }\n"
      "\n"
      "procedure d {\n"
      "  call f;\n"
      "  call g; }\n"
      "procedure e {\n"
      "  x = 0; }\n"
      "procedure f {\n"
      "  x = 1; }\n"
      "procedure g {\n"
      "  x = 2; }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  REQUIRE(pkb->isCallsT("a", "b"));
  REQUIRE(pkb->isCallsT("a", "c"));
  REQUIRE(pkb->isCallsT("a", "d"));
  REQUIRE(pkb->isCallsT("a", "e"));
  REQUIRE(pkb->isCallsT("a", "f"));
  REQUIRE(pkb->isCallsT("a", "g"));
  REQUIRE(pkb->isCallsT("b", "d"));
  REQUIRE(pkb->isCallsT("b", "f"));
  REQUIRE(pkb->isCallsT("b", "g"));
  REQUIRE(pkb->isCallsT("c", "d"));
  REQUIRE(pkb->isCallsT("c", "e"));
  REQUIRE(pkb->isCallsT("c", "f"));
  REQUIRE(pkb->isCallsT("c", "g"));
}

TEST_CASE("Cyclic Calls") {
  REQUIRE(1 == 1);

  string program =
      "procedure a {\n"
      "  call b; }\n"
      "\n"
      "procedure b {\n"
      "  call c; }\n"
      "\n"
      "procedure c {\n"
      "  call a; }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  REQUIRE_THROWS_WITH(de.Extract(ast), "Cyclic/Recursive loop detected.");
}

TEST_CASE("Recursive Call") {
  REQUIRE(1 == 1);

  string program =
      "procedure a {\n"
      "  call a; }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  REQUIRE_THROWS_WITH(de.Extract(ast), "Cyclic/Recursive loop detected.");
}

TEST_CASE("No Call") {
  string program =
      "procedure a {"
      "  x = 0;"
      "}"
      "procedure b {"
      "  x = 0;"
      "}";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  REQUIRE_NOTHROW(de.Extract(ast));
}
