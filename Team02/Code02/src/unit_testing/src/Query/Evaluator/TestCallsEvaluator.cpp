#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/CallsEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace std;
using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("CallsEvaluator: Calls") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertAt(TableType::PROC_TABLE, "proc1");
  int proc2Idx = pkb->insertAt(TableType::PROC_TABLE, "proc2");
  int proc3Idx = pkb->insertAt(TableType::PROC_TABLE, "proc3");
  pkb->addStmt(DesignEntity::CALL, 1);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc2");
  pkb->addStmt(DesignEntity::CALL, 2);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_S, 2, TableType::PROC_TABLE, "proc3");
  pkb->addStmt(DesignEntity::CALL, 3);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_S, 3, TableType::PROC_TABLE, "proc3");

  CallsEvaluator ce(pkb);

  SECTION("Calls('proc1', 'proc2')") {
    Param left = {ParamType::NAME_LITERAL, "proc1"};
    Param right = {ParamType::NAME_LITERAL, "proc2"};
    bool result = ce.evaluateBoolCalls(left, right);
    REQUIRE(result == true);
  }

  SECTION("Calls('proc1', _)") {
    Param left = {ParamType::NAME_LITERAL, "proc1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ce.evaluateBoolCalls(left, right);
    REQUIRE(result == true);
  }

  SECTION("Calls(_, 'proc2')") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::NAME_LITERAL, "proc2"};
    bool result = ce.evaluateBoolCalls(left, right);
    REQUIRE(result == true);
  }

  SECTION("Calls(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ce.evaluateBoolCalls(left, right);
    REQUIRE(result == true);
  }

  SECTION("Calls(p1, 'proc2')") {
    Param left = {ParamType::SYNONYM, "p1"};
    Param right = {ParamType::NAME_LITERAL, "proc2"};
    unordered_set<int> result = ce.evaluateProcCalls(left, right);
    REQUIRE(result == unordered_set<int>({proc1Idx}));
  }

  SECTION("Calls('proc1', p2)") {
    Param left = {ParamType::NAME_LITERAL, "proc1"};
    Param right = {ParamType::SYNONYM, "p2"};
    unordered_set<int> result = ce.evaluateProcCalls(left, right);
    REQUIRE(result == unordered_set<int>({proc2Idx, proc3Idx}));
  }

  SECTION("Calls(p1, p2)") {
    Param left = {ParamType::SYNONYM, "p1"};
    Param right = {ParamType::SYNONYM, "p2"};
    vector<vector<int>> result = ce.evaluateProcPairCalls(left, right);
    REQUIRE_THAT(result, VectorContains(vector<int>({proc1Idx, proc2Idx})));
    REQUIRE_THAT(result, VectorContains(vector<int>({proc1Idx, proc3Idx})));
    REQUIRE_THAT(result, VectorContains(vector<int>({proc2Idx, proc3Idx})));
  }
}

TEST_CASE("CallsEvaluator: CallsT") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertAt(TableType::PROC_TABLE, "proc1");
  int proc2Idx = pkb->insertAt(TableType::PROC_TABLE, "proc2");
  int proc3Idx = pkb->insertAt(TableType::PROC_TABLE, "proc3");
  int proc4Idx = pkb->insertAt(TableType::PROC_TABLE, "proc4");
  pkb->addStmt(DesignEntity::CALL, 1);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc2");
  pkb->addStmt(DesignEntity::CALL, 2);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_S, 2, TableType::PROC_TABLE, "proc3");
  pkb->addStmt(DesignEntity::CALL, 3);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc3",
             TableType::PROC_TABLE, "proc4");
  pkb->addRs(RelationshipType::CALLS_S, 3, TableType::PROC_TABLE, "proc4");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc4");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc2",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc2",
             TableType::PROC_TABLE, "proc4");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc3",
             TableType::PROC_TABLE, "proc4");

  CallsEvaluator ce(pkb);

  SECTION("CallsT('proc1', 'proc3')") {
    Param left = {ParamType::NAME_LITERAL, "proc1"};
    Param right = {ParamType::NAME_LITERAL, "proc3"};
    bool result = ce.evaluateBoolCallsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("CallsT('proc1', _)") {
    Param left = {ParamType::NAME_LITERAL, "proc1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ce.evaluateBoolCallsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("CallsT(_, 'proc3')") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::NAME_LITERAL, "proc3"};
    bool result = ce.evaluateBoolCallsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("CallsT(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ce.evaluateBoolCallsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("CallsT(p1, 'proc4')") {
    Param left = {ParamType::SYNONYM, "p1"};
    Param right = {ParamType::NAME_LITERAL, "proc4"};
    unordered_set<int> result = ce.evaluateProcCallsT(left, right);
    REQUIRE(result == unordered_set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("CallsT('proc1', p2)") {
    Param left = {ParamType::NAME_LITERAL, "proc1"};
    Param right = {ParamType::NAME_LITERAL, "p2"};
    unordered_set<int> result = ce.evaluateProcCallsT(left, right);
    REQUIRE(result == unordered_set<int>({proc2Idx, proc3Idx, proc4Idx}));
  }

  SECTION("CallsT(p1, p2)") {
    Param left = {ParamType::SYNONYM, "p1"};
    Param right = {ParamType::SYNONYM, "p2"};
    vector<vector<int>> result = ce.evaluateProcPairCallsT(left, right);
    REQUIRE_THAT(result, VectorContains(vector<int>({proc1Idx, proc2Idx})));
    REQUIRE_THAT(result, VectorContains(vector<int>({proc1Idx, proc3Idx})));
    REQUIRE_THAT(result, VectorContains(vector<int>({proc1Idx, proc4Idx})));
    REQUIRE_THAT(result, VectorContains(vector<int>({proc2Idx, proc3Idx})));
    REQUIRE_THAT(result, VectorContains(vector<int>({proc2Idx, proc4Idx})));
    REQUIRE_THAT(result, VectorContains(vector<int>({proc3Idx, proc4Idx})));
  }
}
