
#include <Common/Tokenizer.h>
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>

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

  REQUIRE(pkb->isStmt(DesignEntity::CALL, 10));
  REQUIRE(pkb->isStmt(DesignEntity::CALL, 12));
  REQUIRE(pkb->isStmt(DesignEntity::CALL, 16));
  REQUIRE(!pkb->isStmt(DesignEntity::CALL, 17));
  REQUIRE(pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "Example",
                    TableType::PROC_TABLE, "p"));
  REQUIRE(!pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "Example",
                     TableType::PROC_TABLE, "q"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "p",
                    TableType::PROC_TABLE, "q"));
  REQUIRE(!pkb->isStmt(DesignEntity::CALL,
                       25));  // out of bound test, last stmt is #24
  REQUIRE(!pkb->isStmt(DesignEntity::CALL,
                       30));  // out of bound test, last stmt is #24

  SECTION("[DE][AffectsInfo] sample source") {
    ProcIdx exampleIdx = pkb->getIndexOf(TableType::PROC_TABLE, "Example");
    ProcIdx pIdx = pkb->getIndexOf(TableType::PROC_TABLE, "p");
    ProcIdx qIdx = pkb->getIndexOf(TableType::PROC_TABLE, "q");

    unordered_map<ProcIdx, unordered_set<ProcIdx>> callGraph =
        pkb->getCallGraph();

    // positive test
    CHECK(callGraph.at(exampleIdx) == unordered_set<ProcIdx>{pIdx});
    CHECK(callGraph.at(pIdx) == unordered_set<ProcIdx>{qIdx});
    // neg test
    CHECK_THROWS_AS(callGraph.at(qIdx), out_of_range);
  }
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

  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                    TableType::PROC_TABLE, "b"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                    TableType::PROC_TABLE, "c"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                    TableType::PROC_TABLE, "d"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                    TableType::PROC_TABLE, "e"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                    TableType::PROC_TABLE, "f"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                    TableType::PROC_TABLE, "g"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "b",
                    TableType::PROC_TABLE, "d"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "b",
                    TableType::PROC_TABLE, "f"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "b",
                    TableType::PROC_TABLE, "g"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "c",
                    TableType::PROC_TABLE, "d"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "c",
                    TableType::PROC_TABLE, "e"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "c",
                    TableType::PROC_TABLE, "f"));
  REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "c",
                    TableType::PROC_TABLE, "g"));
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
  REQUIRE_THROWS_WITH(de.Extract(ast), "Cyclic call detected.");
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
  REQUIRE_THROWS_WITH(de.Extract(ast), "Recursive call detected.");
}

TEST_CASE("No Call") {
  string program =
      "procedure a {\n"
      "  x = 0; }\n"
      "procedure b {\n"
      "  x = 0; }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  REQUIRE_NOTHROW(de.Extract(ast));
}

TEST_CASE("Calling non-existent procedure") {
  string program =
      "procedure a {\n"
      "  call b; }\n"
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  REQUIRE_THROWS_WITH(de.Extract(ast),
                      "Found call statement calling "
                      "non-existent procedure.");
}
