
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Common/Tokenizer.h>

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
      "      x = z * 3 + 2 * y + a;\n"
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
      "    z = x + 1 + b; }\n"
      "  else {\n"
      "    x = z + x; } }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  SECTION("Uses R/S") {
    RelationshipType rs = RelationshipType::USES_S;
    TableType rightType = TableType::VAR_TABLE;
    TableType leftType = TableType::PROC_TABLE;
    REQUIRE(pkb->isRs(rs, 4, rightType,
                      "i"));  // container stmt from 4 - 11
    REQUIRE(pkb->isRs(rs, 4, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 4, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 5, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 6, rightType,
                      "x"));  // container stmt from 6 - 8
    REQUIRE(pkb->isRs(rs, 6, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 7, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 8, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 8, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 9, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 9, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 9, rightType, "i"));
    REQUIRE(pkb->isRs(rs, 11, rightType, "i"));

    REQUIRE(pkb->isRs(rs, 13, rightType, "x"));  // container stmt from 13 - 20
    REQUIRE(pkb->isRs(rs, 13, rightType, "i"));
    REQUIRE(pkb->isRs(rs, 13, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 13, rightType, "y"));

    REQUIRE(pkb->isRs(rs, 14, rightType, "i"));  // container stmt from 14 - 17
    REQUIRE(pkb->isRs(rs, 14, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 14, rightType, "y"));
    REQUIRE(pkb->isRs(rs, 15, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 15, rightType, "y"));
    REQUIRE(pkb->isRs(rs, 17, rightType, "i"));

    REQUIRE(pkb->isRs(rs, 18, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 19, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 19, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 21, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 21, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 21, rightType, "i"));

    REQUIRE(pkb->isRs(rs, 22, rightType, "x"));  // container stmt from 22 - 24
    REQUIRE(pkb->isRs(rs, 22, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 23, rightType, "x"));
    REQUIRE(pkb->isRs(rs, 24, rightType, "z"));
    REQUIRE(pkb->isRs(rs, 24, rightType, "x"));

    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "x") ==
            SetOfStmts({4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 16, 18, 19,
                                21, 22, 23, 24}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "i") ==
            SetOfStmts({4, 9, 11, 12, 13, 14, 17, 21}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "z") == SetOfStmts({4, 6, 8, 9, 10, 12, 13, 14,
                                                     15, 16, 19, 21, 22, 24}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "y") == SetOfStmts({12, 13, 14, 15}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "a") == SetOfStmts({12, 13, 14, 15}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "b") ==
            SetOfStmts({4, 10, 12, 13, 14, 16, 22, 23}));

    rs = RelationshipType::USES_P;
    REQUIRE(pkb->isRs(rs, leftType, "q", rightType, "z"));
    REQUIRE(pkb->isRs(rs, leftType, "q", rightType, "b"));
    REQUIRE(pkb->isRs(rs, leftType, "q", rightType, "x"));

    REQUIRE(pkb->isRs(rs, leftType, "p", rightType, "a"));
    REQUIRE(pkb->isRs(rs, leftType, "p", rightType, "y"));
    REQUIRE(pkb->isRs(rs, leftType, "p", rightType, "x"));
    REQUIRE(pkb->isRs(rs, leftType, "p", rightType, "i"));
    REQUIRE(pkb->isRs(rs, leftType, "p", rightType, "b"));
    REQUIRE(pkb->isRs(rs, leftType, "p", rightType, "z"));

    REQUIRE(pkb->isRs(rs, leftType, "Example", rightType, "a"));
    REQUIRE(pkb->isRs(rs, leftType, "Example", rightType, "x"));
    REQUIRE(pkb->isRs(rs, leftType, "Example", rightType, "i"));
    REQUIRE(pkb->isRs(rs, leftType, "Example", rightType, "b"));
    REQUIRE(pkb->isRs(rs, leftType, "Example", rightType, "z"));
    REQUIRE(pkb->isRs(rs, leftType, "Example", rightType, "y"));
  }
}
