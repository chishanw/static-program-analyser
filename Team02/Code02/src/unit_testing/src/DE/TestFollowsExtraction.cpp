
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
            UNO_SET_OF_STMT_NO({2, 3, 4, 12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 2) ==
            UNO_SET_OF_STMT_NO({3, 4, 12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 3) ==
            UNO_SET_OF_STMT_NO({4, 12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 4) ==
            UNO_SET_OF_STMT_NO({12}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 5) ==
            UNO_SET_OF_STMT_NO({6, 9, 10, 11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 6) ==
            UNO_SET_OF_STMT_NO({9, 10, 11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 7) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 8) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 9) ==
            UNO_SET_OF_STMT_NO({10, 11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 10) ==
            UNO_SET_OF_STMT_NO({11}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 11) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 12) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 13) ==
            UNO_SET_OF_STMT_NO({21}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 14) ==
            UNO_SET_OF_STMT_NO({18, 19}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 15) ==
            UNO_SET_OF_STMT_NO({16, 17}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 16) ==
            UNO_SET_OF_STMT_NO({17}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 17) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 18) ==
            UNO_SET_OF_STMT_NO({19}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 19) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 20) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 21) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 22) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 23) ==
            UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 24) ==
            UNO_SET_OF_STMT_NO({}));
  }
}
