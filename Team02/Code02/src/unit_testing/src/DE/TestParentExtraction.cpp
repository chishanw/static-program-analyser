
#include <Common/Tokenizer.h>
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>

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
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 1).empty());
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 2).empty());
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 3).empty());
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 4).empty());
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 4, 5));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 4, 6));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 6, 7));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 6, 8));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 4, 9));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 4, 10));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 4, 11));
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 12).empty());
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 13).empty());
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 13, 14));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 14, 15));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 14, 16));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 14, 17));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 13, 18));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 13, 19));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 13, 20));
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 21).empty());
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 22).empty());
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 22, 23));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 22, 24));
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 25)
                .empty());  // out of bound test, last stmt is #24
    REQUIRE(pkb->getLeft(RelationshipType::PARENT, 30)
                .empty());  // out of bound test, last stmt is #24
  }

  SECTION("Parent* R/S") {
    REQUIRE(pkb->getRight(RelationshipType::PARENT_T, 4) ==
            SetOfStmts({5, 6, 7, 8, 9, 10, 11}));

    REQUIRE(pkb->getRight(RelationshipType::PARENT_T, 6) ==
            SetOfStmts({7, 8}));

    REQUIRE(pkb->getRight(RelationshipType::PARENT_T, 13) ==
            SetOfStmts({14, 15, 16, 17, 18, 19, 20}));

    REQUIRE(pkb->getRight(RelationshipType::PARENT_T, 14) ==
            SetOfStmts({15, 16, 17}));

    REQUIRE(pkb->getRight(RelationshipType::PARENT_T, 22) ==
            SetOfStmts({23, 24}));
  }
}
