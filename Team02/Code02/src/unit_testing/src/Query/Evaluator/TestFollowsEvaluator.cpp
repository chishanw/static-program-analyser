#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/FollowsEvaluator.h>

#include <iostream>

#include "catch.hpp"
using namespace std;
using namespace query;

TEST_CASE("FollowsEvaluator: Follows") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);

  SECTION("Select s such that Follows(1, 2)") {
    pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = fe.evaluateBoolFollows(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that Follows(1, _)") {
    pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = fe.evaluateBoolFollows(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that Follows(1, s)") {
    pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    SetOfStmts result = fe.evaluateStmtFollows(left, right);
    REQUIRE(result == SetOfStmts({2}));
  }
}

TEST_CASE("FollowsEvaluator: FollowsT") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);

  SECTION("Select s such that FollowsT(1, 2)") {
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = fe.evaluateBoolFollowsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that FollowsT(1, _)") {
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = fe.evaluateBoolFollowsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that Follows(1, s)") {
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 3);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    SetOfStmts result = fe.evaluateStmtFollowsT(left, right);
    REQUIRE(result == SetOfStmts({2, 3}));
  }
}
