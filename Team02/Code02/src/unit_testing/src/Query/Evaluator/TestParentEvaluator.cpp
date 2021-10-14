#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/ParentEvaluator.h>

#include <iostream>

#include "catch.hpp"
using namespace std;
using namespace query;

TEST_CASE("ParentEvaluator: Parent") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->setParent(1, 2);
  pkb->setParent(1, 3);

  SECTION("Evaluates both literals - Parent(1, 2)") {
    ParentEvaluator pe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = pe.evaluateBoolParent(left, right);
    REQUIRE(result == true);
  }

  SECTION("Evaluates literal & wildcard - Parent(1, _)") {
    ParentEvaluator pe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = pe.evaluateBoolParent(left, right);
    REQUIRE(result == true);
  }

  SECTION("Evaluates literal & synonym - Parent(1, s)") {
    ParentEvaluator pe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    UNO_SET_OF_STMT_NO result = pe.evaluateStmtParent(left, right);
    REQUIRE(result == UNO_SET_OF_STMT_NO({2, 3}));
  }

  SECTION("Evaluates both wildcards - Parent(_, _)") {
    ParentEvaluator pe(pkb);

    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = pe.evaluateBoolParent(left, right);
    REQUIRE(result == true);
  }
}

TEST_CASE("ParentEvaluator: ParentT") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);

  SECTION("Evaluates two literals - ParentT(1, 2)") {
    pkb->addParentT(1, 2);
    ParentEvaluator pe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = pe.evaluateBoolParentT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Evaluates literal & wildcard - ParentT(1, _)") {
    pkb->addParentT(1, 2);
    ParentEvaluator pe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = pe.evaluateBoolParentT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Evaluates literal & synonym - ParentT(1, s)") {
    pkb->addParentT(1, 2);
    pkb->addParentT(1, 3);
    ParentEvaluator pe(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    UNO_SET_OF_STMT_NO result = pe.evaluateStmtParentT(left, right);
    REQUIRE(result == UNO_SET_OF_STMT_NO({2, 3}));
  }

  SECTION("Evaluates both wildcards - ParentT(_, _)") {
    pkb->addParentT(1, 2);
    ParentEvaluator pe(pkb);

    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = pe.evaluateBoolParentT(left, right);
    REQUIRE(result == true);
  }
}
