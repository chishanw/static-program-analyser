#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Projector/ResultProjector.h>

#include <iostream>

#include "catch.hpp"
using namespace std;
using namespace query;

TEST_CASE("Project each design entity correctly") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);

  pkb->insertAt(TableType::VAR_TABLE, "x");
  pkb->insertAt(TableType::VAR_TABLE, "y");
  pkb->insertAt(TableType::VAR_TABLE, "z");

  pkb->insertAt(TableType::PROC_TABLE, "procA");
  pkb->insertAt(TableType::PROC_TABLE, "procB");
  pkb->insertAt(TableType::PROC_TABLE, "procC");

  pkb->insertAt(TableType::CONST_TABLE, "1");
  pkb->insertAt(TableType::CONST_TABLE, "3");
  pkb->insertAt(TableType::CONST_TABLE, "5");

  ResultProjector projector(pkb);

  SECTION("stmt s; Select s") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS, {{DesignEntity::STATEMENT, "s"}},
        {{1}, {2}, {3}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "2", "3"}));
  }

  SECTION("assign a; Select a") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS, {{DesignEntity::ASSIGN, "a"}}, {{1}, {2}, {3}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "2", "3"}));
  }

  SECTION("variable v; Select v") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS, {{DesignEntity::VARIABLE, "v"}}, {{0}, {1}, {2}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"x", "y", "z"}));
  }

  SECTION("procedure p; Select p") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS, {{DesignEntity::PROCEDURE, "p"}},
        {{0}, {1}, {2}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"procA", "procB", "procC"}));
  }

  SECTION("constant c; Select c") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS, {{DesignEntity::CONSTANT, "c"}}, {{0}, {1}, {2}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "3", "5"}));
  }
}

TEST_CASE("Project boolean results") {
  PKB* pkb = new PKB();
  ResultProjector projector(pkb);

  SECTION("Select BOOLEAN - TRUE") {
    list<string> result =
        projector.formatResults(SelectType::BOOLEAN, {}, {{1}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"TRUE"}));
  }

  SECTION("Select BOOLEAN - FALSE") {
    list<string> result =
        projector.formatResults(SelectType::BOOLEAN, {}, {{0}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"FALSE"}));
  }
}

TEST_CASE("Project tuple results") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::STATEMENT, 5);
  pkb->addStmt(DesignEntity::STATEMENT, 6);
  pkb->addStmt(DesignEntity::ASSIGN, 2);
  pkb->addStmt(DesignEntity::ASSIGN, 4);
  pkb->addStmt(DesignEntity::ASSIGN, 6);
  int xVarIdx = pkb->insertAt(TableType::VAR_TABLE, "x");
  int yVarIdx = pkb->insertAt(TableType::VAR_TABLE, "y");
  int aProcIdx = pkb->insertAt(TableType::PROC_TABLE, "procA");
  int bProcIdx = pkb->insertAt(TableType::PROC_TABLE, "procB");

  ResultProjector projector(pkb);

  SECTION("Select <s>") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS, {{DesignEntity::STATEMENT, "s"}},
        {{1}, {2}, {3}, {4}, {5}, {6}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "2", "3", "4", "5", "6"}));
  }

  SECTION("Select <s, a>") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::STATEMENT, "s"}, {DesignEntity::ASSIGN, "a"}},
        {{1, 2}, {3, 4}, {5, 6}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1 2", "3 4", "5 6"}));
  }

  SECTION("Select <a, v, p>") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::ASSIGN, "a"},
         {DesignEntity::VARIABLE, "v"},
         {DesignEntity::PROCEDURE, "p"}},
        {{1, xVarIdx, aProcIdx}, {2, yVarIdx, bProcIdx}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1 x procA", "2 y procB"}));
  }
}

TEST_CASE("Project with attributes") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::STATEMENT, 5);
  pkb->addStmt(DesignEntity::STATEMENT, 6);
  pkb->addCalls(1, "procA", "procB");
  pkb->addStmt(DesignEntity::READ, 2);
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "y");
  pkb->addStmt(DesignEntity::PRINT, 3);
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "z");
  pkb->addStmt(DesignEntity::WHILE, 4);
  pkb->addStmt(DesignEntity::IF, 5);
  pkb->addStmt(DesignEntity::ASSIGN, 6);
  int xVarIdx = pkb->insertAt(TableType::VAR_TABLE, "x");
  int yVarIdx = pkb->getIndexOf(TableType::VAR_TABLE, "y");
  int zVarIdx = pkb->getIndexOf(TableType::VAR_TABLE, "z");
  int aProcIdx = pkb->insertAt(TableType::PROC_TABLE, "procA");
  int bProcIdx = pkb->insertAt(TableType::PROC_TABLE, "procB");
  int const1Idx = pkb->insertAt(TableType::CONST_TABLE, "1");

  ResultProjector projector(pkb);

  SECTION("Select p.procName") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::PROCEDURE, "p", true, Attribute::PROC_NAME}},
        {{aProcIdx}, {bProcIdx}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"procA", "procB"}));
  }

  SECTION("Select cll.procName") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::CALL, "cll", true, Attribute::PROC_NAME}}, {{1}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"procB"}));
  }

  SECTION("Select v.varName") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::VARIABLE, "v", true, Attribute::VAR_NAME}},
        {{xVarIdx}, {yVarIdx}, {zVarIdx}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"x", "y", "z"}));
  }

  SECTION("Select rd.varName") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::READ, "rd", true, Attribute::VAR_NAME}}, {{2}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"y"}));
  }

  SECTION("Select pr.varName") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::PRINT, "pr", true, Attribute::VAR_NAME}}, {{3}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"z"}));
  }

  SECTION("Select c.value") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::CONSTANT, "c", true, Attribute::VALUE}}, {{const1Idx}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"1"}));
  }

  SECTION("Select s.stmt#") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::STATEMENT, "s", true, Attribute::STMT_NUM}},
        {{1}, {2}, {3}, {4}, {5}, {6}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "2", "3", "4", "5", "6"}));
  }

  SECTION("Select cll.stmt#") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::CALL, "cll", true, Attribute::STMT_NUM}}, {{1}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"1"}));
  }

  SECTION("Select rd.stmt#") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::READ, "rd", true, Attribute::STMT_NUM}}, {{2}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"2"}));
  }

  SECTION("Select pr.stmt#") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::PRINT, "pr", true, Attribute::STMT_NUM}}, {{3}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"3"}));
  }

  SECTION("Select w.stmt#") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::WHILE, "w", true, Attribute::STMT_NUM}}, {{4}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"4"}));
  }

  SECTION("Select ifs.stmt#") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::IF, "ifs", true, Attribute::STMT_NUM}}, {{5}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"5"}));
  }

  SECTION("Select a.stmt#") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::ASSIGN, "a", true, Attribute::STMT_NUM}}, {{6}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"6"}));
  }

  SECTION("Select <p, p.procName, v, s.stmt#>") {
    list<string> result = projector.formatResults(
        SelectType::SYNONYMS,
        {{DesignEntity::PROCEDURE, "p", false},
         {DesignEntity::PROCEDURE, "p", true, Attribute::PROC_NAME},
         {DesignEntity::VARIABLE, "v", false},
         {DesignEntity::STATEMENT, "s", true, Attribute::STMT_NUM}},
        {{aProcIdx, aProcIdx, xVarIdx, 4}, {bProcIdx, bProcIdx, yVarIdx, 5}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"procA procA x 4", "procB procB y 5"}));
  }
}
