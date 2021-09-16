#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Projector/ResultProjector.h>

#include <iostream>

#include "catch.hpp"

using namespace query;

TEST_CASE("stmt s; Select s") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);

  ResultProjector projector(pkb);
  list<string> result = projector.formatResults(DesignEntity::STATEMENT,
                                                unordered_set({1, 2, 3}));
  REQUIRE(set<string>(result.begin(), result.end()) ==
          set<string>({"1", "2", "3"}));
}

TEST_CASE("assign a; Select a") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);

  ResultProjector projector(pkb);
  list<string> result =
      projector.formatResults(DesignEntity::ASSIGN, unordered_set({1, 2, 3}));
  REQUIRE(set<string>(result.begin(), result.end()) ==
          set<string>({"1", "2", "3"}));
}

TEST_CASE("variable v; Select v") {
  PKB* pkb = new PKB();
  pkb->varTable.insertVar("x");
  pkb->varTable.insertVar("y");
  pkb->varTable.insertVar("z");

  ResultProjector projector(pkb);

  list<string> result =
      projector.formatResults(DesignEntity::VARIABLE, unordered_set({0, 1, 2}));
  REQUIRE(set<string>(result.begin(), result.end()) ==
          set<string>({"x", "y", "z"}));
}

TEST_CASE("procedure p; Select p") {
  PKB* pkb = new PKB();
  pkb->procTable.insertProc("procA");
  pkb->procTable.insertProc("procB");
  pkb->procTable.insertProc("procC");

  ResultProjector projector(pkb);

  list<string> result = projector.formatResults(DesignEntity::PROCEDURE,
                                                unordered_set({0, 1, 2}));
  REQUIRE(set<string>(result.begin(), result.end()) ==
          set<string>({"procA", "procB", "procC"}));
}

TEST_CASE("constant c; Select c") {
  PKB* pkb = new PKB();
  pkb->addConstant(1);
  pkb->addConstant(3);
  pkb->addConstant(5);

  ResultProjector projector(pkb);

  list<string> result =
      projector.formatResults(DesignEntity::CONSTANT, unordered_set({1, 3, 5}));
  REQUIRE(set<string>(result.begin(), result.end()) ==
          set<string>({"1", "3", "5"}));
}
