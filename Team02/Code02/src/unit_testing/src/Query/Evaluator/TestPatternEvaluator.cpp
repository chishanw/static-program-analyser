#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/PatternEvaluator.h>

#include <iostream>

#include "catch.hpp"
using namespace std;
using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("PatternEvaluator: Assign") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::ASSIGN, 1);
  pkb->addStmt(DesignEntity::ASSIGN, 2);
  pkb->addStmt(DesignEntity::ASSIGN, 3);

  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 1, "x", "w");
  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, 1, "x", "w");

  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 2, "y", "w");
  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, 2, "y", "w");

  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 3, "z", "1");
  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, 3, "z", "1");

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
    auto setOfResults = pe.evaluateAssignPairPattern(varParam, patternExpr);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, yVarIdx})));
  }

  SECTION("a (v, _'w'_)") {
    Param varParam = {ParamType::SYNONYM, "v"};
    PatternExpr patternExpr = {MatchType::SUB_EXPRESSION, "w"};
    auto setOfResults = pe.evaluateAssignPairPattern(varParam, patternExpr);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, yVarIdx})));
  }

  SECTION("a (v, _)") {
    Param varParam = {ParamType::SYNONYM, "v"};
    PatternExpr patternExpr = {MatchType::ANY, "_"};
    auto setOfResults = pe.evaluateAssignPairPattern(varParam, patternExpr);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, yVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, zVarIdx})));
  }
}

TEST_CASE("PatternEvaluator: If") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::IF, 1);
  pkb->addStmt(DesignEntity::IF, 2);
  pkb->addStmt(DesignEntity::IF, 3);

  pkb->addPatternRs(RelationshipType::PTT_IF, 1, "x");
  pkb->addPatternRs(RelationshipType::PTT_IF, 1, "y");
  pkb->addPatternRs(RelationshipType::PTT_IF, 2, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  PatternEvaluator pe(pkb);

  SECTION("ifs ('x', _, _)") {
    Param varParam = {ParamType::NAME_LITERAL, "x"};
    unordered_set<int> results = pe.evaluateIfPattern(varParam);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("ifs (_, _, _)") {
    Param varParam = {ParamType::WILDCARD, "_"};
    unordered_set<int> results = pe.evaluateIfPattern(varParam);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("ifs (v, _, _)") {
    Param varParam = {ParamType::SYNONYM, "v"};
    auto setOfResults = pe.evaluateIfPairPattern(varParam);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, yVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, zVarIdx})));
  }
}

TEST_CASE("PatternEvaluator: While") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::WHILE, 1);
  pkb->addStmt(DesignEntity::WHILE, 2);
  pkb->addStmt(DesignEntity::WHILE, 3);

  pkb->addPatternRs(RelationshipType::PTT_WHILE, 1, "x");
  pkb->addPatternRs(RelationshipType::PTT_WHILE, 1, "y");
  pkb->addPatternRs(RelationshipType::PTT_WHILE, 2, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  PatternEvaluator pe(pkb);

  SECTION("w ('x', _)") {
    Param varParam = {ParamType::NAME_LITERAL, "x"};
    unordered_set<int> results = pe.evaluateWhilePattern(varParam);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("w (_, _)") {
    Param varParam = {ParamType::WILDCARD, "_"};
    unordered_set<int> results = pe.evaluateWhilePattern(varParam);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("w (v, _)") {
    Param varParam = {ParamType::SYNONYM, "v"};
    auto setOfResults = pe.evaluateWhilePairPattern(varParam);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, xVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, yVarIdx})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, zVarIdx})));
  }
}
