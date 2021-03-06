
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Common/Tokenizer.h>

#include <vector>

#include "catch.hpp"
#include "Query/Common.h"

using namespace std;
using namespace query;

TEST_CASE("[DE][Modifies R/S] sample source") {
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
    "    a = 0;\n"
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
    "    z = x + 1;\n"
    "    b = z;}\n"
    "  else {\n"
    "    x = z + x; } }\n"
    "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  SECTION("Modifies R/S") {
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "x") ==
            SetOfStmts({1, 4, 5, 10, 12, 13, 15, 16, 17, 19, 23, 26}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "i") ==
            SetOfStmts({3, 4, 11, 12, 13, 15, 18}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "z") ==
            SetOfStmts(
                {2, 4, 6, 7, 9, 10, 12, 13, 15, 17, 20, 21, 22, 23, 24}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "y") == SetOfStmts({4, 6, 8}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "a") == SetOfStmts({12, 13, 14}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "b") ==
            SetOfStmts({4, 10, 12, 13, 15, 17, 23, 25}));

    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "q",
                      TableType::VAR_TABLE, "z"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "q",
                      TableType::VAR_TABLE, "b"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "q",
                      TableType::VAR_TABLE, "x"));

    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "p",
                      TableType::VAR_TABLE, "a"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "p",
                      TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "p",
                      TableType::VAR_TABLE, "i"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "p",
                      TableType::VAR_TABLE, "b"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "p",
                      TableType::VAR_TABLE, "z"));

    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                      "Example", TableType::VAR_TABLE, "a"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                      "Example", TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                      "Example", TableType::VAR_TABLE, "i"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                      "Example", TableType::VAR_TABLE, "b"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                      "Example", TableType::VAR_TABLE, "z"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                      "Example", TableType::VAR_TABLE, "y"));
  }
}
