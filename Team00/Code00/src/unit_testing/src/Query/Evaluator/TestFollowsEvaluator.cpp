#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/FollowsEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace query;

TEST_CASE("FollowsEvaluator: Follows") {
  PKB* pkb = new PKB();

  SECTION("Select s such that Follows(1, 2)") {
    pkb->setFollows(1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = fe.evaluateBoolFollows(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that Follows(1, _)") {
    pkb->setFollows(1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = fe.evaluateBoolFollows(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that Follows(1, s)") {
    pkb->setFollows(1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    UNO_SET_OF_STMT_NO result = fe.evaluateStmtFollows(left, right);
    REQUIRE(result == UNO_SET_OF_STMT_NO({2}));
  }
}

TEST_CASE("FollowsEvaluator: FollowsT") {
  PKB* pkb = new PKB();

  SECTION("Select s such that FollowsT(1, 2)") {
    pkb->addFollowsT(1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = fe.evaluateBoolFollowsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that FollowsT(1, _)") {
    pkb->addFollowsT(1, 2);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = fe.evaluateBoolFollowsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Select s such that Follows(1, s)") {
    pkb->addFollowsT(1, 2);
    pkb->addFollowsT(1, 3);
    FollowsEvaluator fe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    UNO_SET_OF_STMT_NO result = fe.evaluateStmtFollowsT(left, right);
    REQUIRE(result == UNO_SET_OF_STMT_NO({2, 3}));
  }
}
