#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/PatternEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("PatternEvaluator: Assign") {
  PKB* pkb = new PKB();
  pkb->addAssignStmt(1);
  pkb->addAssignStmt(2);
  pkb->addAssignStmt(3);

  pkb->addAssignPttFullExpr(1, "x", "w");
  pkb->addAssignPttSubExpr(1, "x", "w");

  pkb->addAssignPttFullExpr(2, "y", "w");
  pkb->addAssignPttSubExpr(2, "y", "w");

  pkb->addAssignPttFullExpr(3, "z", "1");
  pkb->addAssignPttSubExpr(3, "z", "1");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  PatternEvaluator pe(pkb);

  SECTION("a ('x', 'w')") {
    Param varParam = {ParamType::NAME_LITERAL, "x"};
    PatternExpr patternExpr = {MatchType::EXACT, "w"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("a ('x', _'w'_)") {
    Param varParam = {ParamType::NAME_LITERAL, "x"};
    PatternExpr patternExpr = {MatchType::SUB_EXPRESSION, "w"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("a ('x', _)") {
    Param varParam = {ParamType::NAME_LITERAL, "x"};
    PatternExpr patternExpr = {MatchType::ANY, "_"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("a ('z', '1')") {
    Param varParam = {ParamType::NAME_LITERAL, "z"};
    PatternExpr patternExpr = {MatchType::EXACT, "1"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({3}));
  }

  SECTION("a ('z', _'1'_)") {
    Param varParam = {ParamType::NAME_LITERAL, "z"};
    PatternExpr patternExpr = {MatchType::SUB_EXPRESSION, "1"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({3}));
  }

  SECTION("a (_, 'w')") {
    Param varParam = {ParamType::WILDCARD, "_"};
    PatternExpr patternExpr = {MatchType::EXACT, "w"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("a (_, _'w'_)") {
    Param varParam = {ParamType::WILDCARD, "_"};
    PatternExpr patternExpr = {MatchType::SUB_EXPRESSION, "w"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("a (_, '1')") {
    Param varParam = {ParamType::WILDCARD, "_"};
    PatternExpr patternExpr = {MatchType::EXACT, "1"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({3}));
  }

  SECTION("a (_, _'1'_)") {
    Param varParam = {ParamType::WILDCARD, "_"};
    PatternExpr patternExpr = {MatchType::SUB_EXPRESSION, "1"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({3}));
  }

  SECTION("a (_, _)") {
    Param varParam = {ParamType::WILDCARD, "_"};
    PatternExpr patternExpr = {MatchType::ANY, "_"};
    unordered_set<int> results =
        pe.evaluateAssignPattern(varParam, patternExpr);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("a (v, 'w')") {
    Param varParam = {ParamType::SYNONYM, "v"};
    PatternExpr patternExpr = {MatchType::EXACT, "w"};
    vector<vector<int>> results =
        pe.evaluateAssignPairPattern(varParam, patternExpr);
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, yVarIdx})));
  }

  SECTION("a (v, _'w'_)") {
    Param varParam = {ParamType::SYNONYM, "v"};
    PatternExpr patternExpr = {MatchType::SUB_EXPRESSION, "w"};
    vector<vector<int>> results =
        pe.evaluateAssignPairPattern(varParam, patternExpr);
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, yVarIdx})));
  }

  SECTION("a (v, _)") {
    Param varParam = {ParamType::SYNONYM, "v"};
    PatternExpr patternExpr = {MatchType::ANY, "_"};
    vector<vector<int>> results =
        pe.evaluateAssignPairPattern(varParam, patternExpr);
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, yVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, zVarIdx})));
  }
}
