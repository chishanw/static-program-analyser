#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/UsesEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace query;

TEST_CASE("UsesEvaluator: UsesS - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addUsesS(1, "x");
  UsesEvaluator ue(pkb);

  SECTION("UsesS(1, 'x')") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    bool result = ue.evaluateBoolUsesS(left, right);
    REQUIRE(result == true);
  }

  SECTION("UsesS(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ue.evaluateBoolUsesS(left, right);
    REQUIRE(result == true);
  }

  SECTION("UsesS(1, v)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "v"};
    unordered_set<int> result = ue.evaluateUsesS(left, right);
    REQUIRE(result == unordered_set<int>({0}));
  }

  SECTION("UsesS(s, v)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::SYNONYM, "v"};
    vector<pair<int, vector<int>>> result = ue.evaluatePairUsesS(left, right);
    pair<int, vector<int>> resultPair = result[0];
    REQUIRE(resultPair.first == 1);
    REQUIRE(resultPair.second == vector<int>({0}));
  }
}

TEST_CASE("UsesEvaluator: UsesS - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);

  SECTION("UsesS(1, 'x')") {
    pkb->addUsesS(2, "x");
    UsesEvaluator ue(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    bool result = ue.evaluateBoolUsesS(left, right);
    REQUIRE(result == false);
  }

  SECTION("UsesS(1, _)") {
    pkb->addUsesS(2, "x");
    UsesEvaluator ue(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ue.evaluateBoolUsesS(left, right);
    REQUIRE(result == false);
  }

  SECTION("UsesS(1, v)") {
    pkb->addUsesS(2, "x");
    UsesEvaluator ue(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "v"};
    unordered_set<int> result = ue.evaluateUsesS(left, right);
    REQUIRE(result.empty());
  }

  SECTION("UsesS(s, v)") {
    UsesEvaluator ue(pkb);

    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::SYNONYM, "v"};
    vector<pair<int, vector<int>>> result = ue.evaluatePairUsesS(left, right);
    REQUIRE(result.empty());
  }
}
