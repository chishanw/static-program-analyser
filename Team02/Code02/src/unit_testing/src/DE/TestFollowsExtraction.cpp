
#include <Common/Tokenizer.h>
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[DE][Follows(*) R/S] sample source") {
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

  SECTION("Follows R/S") {
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 1, 2));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 2, 3));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 3, 4));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 4, 12));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 5, 6));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 6, 9));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 7).empty());
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 8).empty());
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 9, 10));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 10, 11));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 11).empty());
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 12).empty());
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 13, 21));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 14, 18));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 15, 16));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 16, 17));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 17).empty());
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS, 18, 19));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 19).empty());
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 20).empty());
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 21).empty());
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 22).empty());
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 23).empty());
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 24).empty());
  }

  SECTION("Follows* R/S") {
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 1) ==
            SetOfStmts({2, 3, 4, 12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 2) ==
            SetOfStmts({3, 4, 12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 3) ==
            SetOfStmts({4, 12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 4) ==
            SetOfStmts({12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 5) ==
            SetOfStmts({6, 9, 10, 11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 6) ==
            SetOfStmts({9, 10, 11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 7) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 8) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 9) ==
            SetOfStmts({10, 11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 10) ==
            SetOfStmts({11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 11) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 12) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 13) ==
            SetOfStmts({21}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 14) ==
            SetOfStmts({18, 19}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 15) ==
            SetOfStmts({16, 17}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 16) ==
            SetOfStmts({17}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 17) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 18) ==
            SetOfStmts({19}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 19) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 20) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 21) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 22) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 23) ==
            SetOfStmts({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 24) ==
            SetOfStmts({}));
  }
}
