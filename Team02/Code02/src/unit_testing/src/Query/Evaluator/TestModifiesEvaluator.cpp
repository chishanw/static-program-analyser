#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/ModifiesEvaluator.h>

#include <iostream>

#include "catch.hpp"
using namespace std;
using namespace query;

TEST_CASE("ModifiesEvaluator: ModifiesS - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addModifiesS(1, "x");
  ModifiesEvaluator me(pkb);

  SECTION("ModifiesS(1, 'x')") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    bool result = me.evaluateBoolModifiesS(left, right);
    REQUIRE(result == true);
  }

  SECTION("ModifiesS(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = me.evaluateBoolModifiesS(left, right);
    REQUIRE(result == true);
  }

  SECTION("ModifiesS(1, v)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "v"};
    unordered_set<int> result = me.evaluateModifiesS(left, right);
    REQUIRE(result == unordered_set<int>({0}));
  }

  SECTION("ModifiesS(s, v)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::SYNONYM, "v"};
    vector<pair<int, vector<int>>> result =
        me.evaluatePairModifiesS(left, right);
    pair<int, vector<int>> resultPair = result[0];
    REQUIRE(resultPair.first == 1);
    REQUIRE(resultPair.second == vector<int>({0}));
  }
}

TEST_CASE("ModifiesEvaluator: ModifiesS - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);

  SECTION("ModifiesS(1, 'x')") {
    pkb->addModifiesS(2, "x");
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    bool result = me.evaluateBoolModifiesS(left, right);
    REQUIRE(result == false);
  }

  SECTION("ModifiesS(1, _)") {
    pkb->addModifiesS(2, "x");
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = me.evaluateBoolModifiesS(left, right);
    REQUIRE(result == false);
  }

  SECTION("ModifiesS(1, v)") {
    pkb->addModifiesS(2, "x");
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "v"};
    unordered_set<int> result = me.evaluateModifiesS(left, right);
    REQUIRE(result.empty());
  }

  SECTION("ModifiesS(s, v)") {
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::SYNONYM, "v"};
    vector<pair<int, vector<int>>> result =
        me.evaluatePairModifiesS(left, right);
    REQUIRE(result.empty());
  }
}

TEST_CASE("ModifiesEvaluator: ModifiesP - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addModifiesP("someProc", "x");
  ModifiesEvaluator me(pkb);

  SECTION("ModifiesP('someProc', 'x')") {
    Param left = {ParamType::NAME_LITERAL, "someProc"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    bool result = me.evaluateBoolModifiesP(left, right);
    REQUIRE(result == true);
  }

  SECTION("ModifiesP('someProc', _)") {
    Param left = {ParamType::NAME_LITERAL, "someProc"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = me.evaluateBoolModifiesP(left, right);
    REQUIRE(result == true);
  }

  SECTION("ModifiesP('someProc', v)") {
    Param left = {ParamType::NAME_LITERAL, "someProc"};
    Param right = {ParamType::SYNONYM, "v"};
    unordered_set<int> result = me.evaluateModifiesP(left, right);
    REQUIRE(result == unordered_set<int>({0}));
  }

  SECTION("ModifiesP(p, v)") {
    Param left = {ParamType::SYNONYM, "p"};
    Param right = {ParamType::SYNONYM, "v"};
    vector<pair<int, vector<int>>> result =
        me.evaluatePairModifiesP(left, right);
    pair<int, vector<int>> resultPair = result[0];
    REQUIRE(resultPair.first == 0);
    REQUIRE(resultPair.second == vector<int>({0}));
  }
}

TEST_CASE("ModifiesEvaluator: ModifiesP - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);

  SECTION("ModifiesP('someProc', 'x')") {
    pkb->addModifiesP("otherProc", "x");
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::NAME_LITERAL, "someProc"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    bool result = me.evaluateBoolModifiesP(left, right);
    REQUIRE(result == false);
  }

  SECTION("ModifiesP('someProc', _)") {
    pkb->addModifiesP("otherProc", "x");
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::NAME_LITERAL, "someProc"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = me.evaluateBoolModifiesP(left, right);
    REQUIRE(result == false);
  }

  SECTION("ModifiesP('someProc', v)") {
    pkb->addModifiesP("otherProc", "x");
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::NAME_LITERAL, "someProc"};
    Param right = {ParamType::SYNONYM, "v"};
    unordered_set<int> result = me.evaluateModifiesP(left, right);
    REQUIRE(result.empty());
  }

  SECTION("ModifiesP(p, v)") {
    ModifiesEvaluator me(pkb);

    Param left = {ParamType::SYNONYM, "p"};
    Param right = {ParamType::SYNONYM, "v"};
    vector<pair<int, vector<int>>> result =
        me.evaluatePairModifiesS(left, right);
    REQUIRE(result.empty());
  }
}
