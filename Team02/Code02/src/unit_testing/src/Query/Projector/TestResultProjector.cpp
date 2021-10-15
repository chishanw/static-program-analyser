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
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addStmt(5);
  pkb->addStmt(6);
  pkb->addCalls(1, "procA", "procB");
  pkb->addReadStmt(2);
  pkb->addModifiesS(2, "y");
  pkb->addPrintStmt(3);
  pkb->addUsesS(3, "z");
  pkb->addWhileStmt(4);
  pkb->addIfStmt(5);
  pkb->addAssignStmt(6);
  int xVarIdx = pkb->insertVar("x");
  int yVarIdx = pkb->getVarIndex("y");
  int zVarIdx = pkb->getVarIndex("z");
  int aProcIdx = pkb->insertProc("procA");
  int bProcIdx = pkb->insertProc("procB");
  int const1Idx = pkb->insertConst("1");

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
