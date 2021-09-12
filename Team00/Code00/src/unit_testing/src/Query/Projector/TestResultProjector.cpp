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

// todo(cs): test var, proc, const. waiting on PKB
