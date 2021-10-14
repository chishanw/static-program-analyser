#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Projector/ResultProjector.h>

#include <iostream>

#include "catch.hpp"
using namespace std;
using namespace query;

TEST_CASE("Project each design entity correctly") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);

  pkb->insertVar("x");
  pkb->insertVar("y");
  pkb->insertVar("z");

  pkb->insertProc("procA");
  pkb->insertProc("procB");
  pkb->insertProc("procC");

  pkb->insertConst("1");
  pkb->insertConst("3");
  pkb->insertConst("5");

  ResultProjector projector(pkb);

  SECTION("stmt s; Select s") {
    list<string> result = projector.formatResults(
        {{"s", DesignEntity::STATEMENT}}, SelectType::SYNONYMS,
        {{DesignEntity::STATEMENT, "s"}}, {{1}, {2}, {3}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "2", "3"}));
  }

  SECTION("assign a; Select a") {
    list<string> result = projector.formatResults(
        {{"a", DesignEntity::ASSIGN}}, SelectType::SYNONYMS,
        {{DesignEntity::ASSIGN, "a"}}, {{1}, {2}, {3}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "2", "3"}));
  }

  SECTION("variable v; Select v") {
    list<string> result = projector.formatResults(
        {{"v", DesignEntity::VARIABLE}}, SelectType::SYNONYMS,
        {{DesignEntity::VARIABLE, "v"}}, {{0}, {1}, {2}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"x", "y", "z"}));
  }

  SECTION("procedure p; Select p") {
    list<string> result = projector.formatResults(
        {{"p", DesignEntity::PROCEDURE}}, SelectType::SYNONYMS,
        {{DesignEntity::PROCEDURE, "p"}}, {{0}, {1}, {2}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"procA", "procB", "procC"}));
  }

  SECTION("constant c; Select c") {
    list<string> result = projector.formatResults(
        {{"c", DesignEntity::CONSTANT}}, SelectType::SYNONYMS,
        {{DesignEntity::CONSTANT, "c"}}, {{0}, {1}, {2}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "3", "5"}));
  }
}

TEST_CASE("Project boolean results") {
  PKB* pkb = new PKB();
  ResultProjector projector(pkb);

  SECTION("Select BOOLEAN - TRUE") {
    list<string> result =
        projector.formatResults({}, SelectType::BOOLEAN, {}, {{1}});
    REQUIRE(set<string>(result.begin(), result.end()) == set<string>({"TRUE"}));
  }

  SECTION("Select BOOLEAN - FALSE") {
    list<string> result =
        projector.formatResults({}, SelectType::BOOLEAN, {}, {{0}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"FALSE"}));
  }
}

TEST_CASE("Project tuple results") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addStmt(5);
  pkb->addStmt(6);
  pkb->addAssignStmt(2);
  pkb->addAssignStmt(4);
  pkb->addAssignStmt(6);
  int xVarIdx = pkb->insertVar("x");
  int yVarIdx = pkb->insertVar("y");
  int aProcIdx = pkb->insertProc("procA");
  int bProcIdx = pkb->insertProc("procB");

  ResultProjector projector(pkb);

  SECTION("Select <s>") {
    list<string> result = projector.formatResults(
        {{"s", DesignEntity::STATEMENT}}, SelectType::SYNONYMS,
        {{DesignEntity::STATEMENT, "s"}}, {{1}, {2}, {3}, {4}, {5}, {6}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1", "2", "3", "4", "5", "6"}));
  }

  SECTION("Select <s, a>") {
    list<string> result = projector.formatResults(
        {{"s", DesignEntity::STATEMENT}, {"a", DesignEntity::ASSIGN}},
        SelectType::SYNONYMS,
        {{DesignEntity::STATEMENT, "s"}, {DesignEntity::ASSIGN, "a"}},
        {{1, 2}, {3, 4}, {5, 6}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1 2", "3 4", "5 6"}));
  }

  SECTION("Select <a, v, p>") {
    list<string> result = projector.formatResults(
        {{"a", DesignEntity::ASSIGN},
         {"p", DesignEntity::PROCEDURE},
         {"v", DesignEntity::VARIABLE}},
        SelectType::SYNONYMS,
        {{DesignEntity::ASSIGN, "a"},
         {DesignEntity::VARIABLE, "v"},
         {DesignEntity::PROCEDURE, "p"}},
        {{1, xVarIdx, aProcIdx}, {2, yVarIdx, bProcIdx}});
    REQUIRE(set<string>(result.begin(), result.end()) ==
            set<string>({"1 x procA", "2 y procB"}));
  }
}
