
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

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
    REQUIRE(pkb->getFollows(1) == 2);
    REQUIRE(pkb->getFollows(2) == 3);
    REQUIRE(pkb->getFollows(3) == 4);
    REQUIRE(pkb->getFollows(4) == 12);
    REQUIRE(pkb->getFollows(5) == 6);
    REQUIRE(pkb->getFollows(6) == 9);
    REQUIRE(pkb->getFollows(7) == -1);
    REQUIRE(pkb->getFollows(8) == -1);
    REQUIRE(pkb->getFollows(9) == 10);
    REQUIRE(pkb->getFollows(10) == 11);
    REQUIRE(pkb->getFollows(11) == -1);
    REQUIRE(pkb->getFollows(12) == -1);
    REQUIRE(pkb->getFollows(13) == 21);
    REQUIRE(pkb->getFollows(14) == 18);
    REQUIRE(pkb->getFollows(15) == 16);
    REQUIRE(pkb->getFollows(16) == 17);
    REQUIRE(pkb->getFollows(17) == -1);
    REQUIRE(pkb->getFollows(18) == 19);
    REQUIRE(pkb->getFollows(19) == -1);
    REQUIRE(pkb->getFollows(20) == -1);
    REQUIRE(pkb->getFollows(21) == -1);
    REQUIRE(pkb->getFollows(22) == -1);
    REQUIRE(pkb->getFollows(23) == -1);
    REQUIRE(pkb->getFollows(24) == -1);
  }

  SECTION("Follows* R/S") {
    REQUIRE(pkb->getFollowsT(1) == UNO_SET_OF_STMT_NO({2, 3, 4, 12}));
    REQUIRE(pkb->getFollowsT(2) == UNO_SET_OF_STMT_NO({3, 4, 12}));
    REQUIRE(pkb->getFollowsT(3) == UNO_SET_OF_STMT_NO({4, 12}));
    REQUIRE(pkb->getFollowsT(4) == UNO_SET_OF_STMT_NO({12}));
    REQUIRE(pkb->getFollowsT(5) == UNO_SET_OF_STMT_NO({6, 9, 10, 11}));
    REQUIRE(pkb->getFollowsT(6) == UNO_SET_OF_STMT_NO({9, 10, 11}));
    REQUIRE(pkb->getFollowsT(7) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(8) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(9) == UNO_SET_OF_STMT_NO({10, 11}));
    REQUIRE(pkb->getFollowsT(10) == UNO_SET_OF_STMT_NO({11}));
    REQUIRE(pkb->getFollowsT(11) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(12) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(13) == UNO_SET_OF_STMT_NO({21}));
    REQUIRE(pkb->getFollowsT(14) == UNO_SET_OF_STMT_NO({18, 19}));
    REQUIRE(pkb->getFollowsT(15) == UNO_SET_OF_STMT_NO({16, 17}));
    REQUIRE(pkb->getFollowsT(16) == UNO_SET_OF_STMT_NO({17}));
    REQUIRE(pkb->getFollowsT(17) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(18) == UNO_SET_OF_STMT_NO({19}));
    REQUIRE(pkb->getFollowsT(19) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(20) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(21) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(22) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(23) == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getFollowsT(24) == UNO_SET_OF_STMT_NO({}));
    }
}
