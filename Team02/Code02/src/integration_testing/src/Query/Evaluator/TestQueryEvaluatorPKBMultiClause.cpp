#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/QueryEvaluator.h>

#include <iostream>

#include "../TestQueryUtil.h"
#include "catch.hpp"
using namespace std;
using namespace query;

TEST_CASE("QueryEvaluator: 1 Such That + 1 Pattern Clause") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addAssignStmt(3);
  pkb->addAssignStmt(4);

  pkb->setFollows(1, 2);
  pkb->setParent(2, 3);
  pkb->addModifiesS(3, "x");
  pkb->addAssignPttFullExpr(3, "x", "w");
  pkb->addAssignPttFullExpr(4, "y", "1");
  pkb->addUsesS(3, "w");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int wVarIdx = 2;

  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"a", DesignEntity::ASSIGN},
      {"v", DesignEntity::VARIABLE}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  Synonym a = {DesignEntity::ASSIGN, "a"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  // Test Filter Algo - Subset of 1 Synonym
  SECTION("Select a such that Parent(s1, a) pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "a"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  // Test Filter Algo - Subset of 2 Synonyms
  SECTION("Select a such that ModifiesS(a, v) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "a"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  // Test Inner Join Algo - 1 Overlapping Synonym (Case 1)
  SECTION("Select v such that ModifiesS(s, v) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{xVarIdx}}));
  }

  // Test Inner Join Algo - Subset of 1 Synonym (Case 2)
  SECTION("Select v such that UsesS(a, _) pattern a (v, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "a"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{xVarIdx}}));
  }

  // Test Cross Product Algo - No Overlapping Synonyms
  SECTION("Select s1 such that Follows(s1, s2) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}}));
  }

  // FOLLOWS + PATTERN
  SECTION("Select s1 such that Follows(1, 2) pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(s, 2) pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}}));
  }

  SECTION("Select a such that Follows(s, 2) pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select s1 such that Follows(_, _) pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(1, 2) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(s1, 2) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}}));
  }

  SECTION("Select a such that Follows(s, 2) pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select s1 such that Follows(_, _) pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(1, 2) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(s1, 2) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}}));
  }

  SECTION("Select a such that Follows(s1, 2) pattern a ('_', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select s1 such that Follows(_, _) pattern a ('_, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(1, 2) pattern a (v, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Follows(s1, 2) pattern a (v, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1}));
  }

  SECTION("Select a such that Follows(s1, 2) pattern a ('_', '_')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}, {4}}));
  }

  SECTION("Select s1 such that Follows(_, _) pattern a (_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  // USES_S + PATTERN
  SECTION("Select s1 such that UsesS(3, 'w') pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that UsesS(s1, 'w') pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select a such that UsesS(s1, 'w') pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select s1 such that UsesS(3, 'w') pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that UsesS(s1, 'w') pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select a such that UsesS(s1, 'w') pattern a ('x', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select s1 such that UsesS(3, 'w') pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that UsesS(s1, 'w') pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select a such that UsesS(s1, 'w') pattern a ('_', 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select s1 such that UsesS(3, 'w') pattern a (v, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that UsesS(s1, 'w') pattern a (v, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({3}));
  }

  SECTION("Select a such that UsesS(s1, 'w') pattern a ('_', '_')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::NAME_LITERAL, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}, {4}}));
  }
}

TEST_CASE("QueryEvaluator: Multiple Such That Clauses - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->setFollows(1, 2);
  pkb->setFollows(2, 3);
  pkb->addFollowsT(1, 2);
  pkb->addFollowsT(1, 3);
  pkb->addFollowsT(2, 3);
  pkb->addModifiesS(2, "x");
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"v", DesignEntity::VARIABLE}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Same RelationshipType: No Common Synonym") {
    // Select s1 such that Follows(1, 2) and Follows(s1, 2)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::INTEGER_LITERAL, "1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}}));
  }

  SECTION("Same RelationshipType: Common Synonym") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, _)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Different RelationshipType: No Common Synonym") {
    // Select s1 such that Follows(s1, _) and Modifies(s2, v)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::MODIFIES_S,
                                      {ParamType::SYNONYM, "s2"},
                                      {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Different RelationshipType: Common Synonym") {
    // Select s1 such that Follows(s1, _) and Modifies(s1, v)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::MODIFIES_S,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{2}}));
  }
}

TEST_CASE("QueryEvaluator: Multiple Such That Clauses - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->setFollows(1, 2);
  pkb->setFollows(2, 3);
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"s3", DesignEntity::STATEMENT},
      {"s4", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  Synonym s3 = {DesignEntity::STATEMENT, "s3"};
  Synonym s4 = {DesignEntity::STATEMENT, "s4"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("2 Follows: False Bool Clause + True Synonym Clause") {
    // Select s1 such that Follows(3, 4) and Follows(s1, 2)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::INTEGER_LITERAL, "3"},
                                      {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("2 Follows: False Bool Clause + False Synonym Clause") {
    // Select s1 such that Follows(3, 4) and Follows(s1, 1)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::INTEGER_LITERAL, "3"},
                                      {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("2 Follows: True Synonym Clause + False Synonym Clause") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, 5)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "5"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("2 Follows: False Synonym Clause + False Synonym Clause") {
    // Select s1 such that Follows(s1, 5) and Follows(s1, 6)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "5"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "6"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Multiple Pattern Clauses") {
  PKB* pkb = new PKB();
  pkb->addAssignStmt(1);
  pkb->addAssignStmt(2);
  pkb->addIfStmt(3);
  pkb->addWhileStmt(4);
  pkb->addAssignPttFullExpr(1, "x", "y");
  pkb->addAssignPttFullExpr(2, "x", "z");
  pkb->addIfPtt(3, "y");
  pkb->addWhilePtt(4, "x");
  QueryEvaluator qe(pkb);

  int xVarIdx = 0;

  unordered_map<string, DesignEntity> synonyms = {
      {"a", DesignEntity::ASSIGN},
      {"ifs", DesignEntity::IF},
      {"w", DesignEntity::WHILE},
      {"v", DesignEntity::VARIABLE}};
  Synonym a = {DesignEntity::ASSIGN, "a"};
  Synonym ifs = {DesignEntity::IF, "ifs"};
  Synonym w = {DesignEntity::WHILE, "w"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Same RelationshipType") {
    // Select a pattern a ("x", _) and a (v, "y")
    PatternClause patternClause1 = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause1, {}, ConditionClauseType::PATTERN});

    PatternClause patternClause2 = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "y"}};
    conditionClauses.push_back(
        {{}, patternClause2, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}}));
  }

  SECTION("Different RelationshipType: No Common Synonym") {
    // Select a pattern a ("x", _) and ifs (v, _, _)
    PatternClause patternClause1 = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause1, {}, ConditionClauseType::PATTERN});

    PatternClause patternClause2 = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause2, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{1}, {2}}));
  }

  SECTION("Different RelationshipType: Common Synonym") {
    // Select v pattern a (v, _) and w (v, _)
    PatternClause patternClause1 = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause1, {}, ConditionClauseType::PATTERN});

    PatternClause patternClause2 = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause2, {}, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: Test With Clause + Such That/Pattern") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addAssignStmt(2);
  pkb->addAssignStmt(4);
  pkb->addModifiesS(2, "a");
  pkb->addModifiesS(4, "x");
  pkb->setFollows(1, 2);
  pkb->setFollows(2, 3);
  pkb->addCalls(3, "a", "b");
  int const1Idx = pkb->insertConst("1");
  int const2Idx = pkb->insertConst("2");
  int const3Idx = pkb->insertConst("3");
  int procAIdx = pkb->insertProc("a");
  int procBIdx = pkb->insertProc("b");
  int varAIdx = pkb->getVarIndex("a");
  int varXIdx = pkb->getVarIndex("x");
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE}, {"p2", DesignEntity::PROCEDURE},
      {"v1", DesignEntity::VARIABLE},  {"v2", DesignEntity::VARIABLE},
      {"s", DesignEntity::STATEMENT},  {"a", DesignEntity::ASSIGN},
      {"c1", DesignEntity::CONSTANT},  {"c2", DesignEntity::CONSTANT},
      {"cll", DesignEntity::CALL}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1"};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2"};
  Synonym v1 = {DesignEntity::VARIABLE, "v1"};
  Synonym v2 = {DesignEntity::VARIABLE, "v2"};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym c1 = {DesignEntity::CONSTANT, "c1"};
  Synonym c2 = {DesignEntity::CONSTANT, "c2"};
  Synonym cll = {DesignEntity::CALL, "cll"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that Follows(s, _) with 1 = 1") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::INTEGER_LITERAL, "1"},
                             {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s such that Follows(s, _) with s.stmt# = 2") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s"},
                             {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{2}}));
  }

  SECTION("Select s with s.stmt# = 2 such that Follows(s, _)") {
    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s"},
                             {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{2}}));
  }

  SECTION(
      "Select s such that Follows(s, _) and Follows(a, _) with s.stmt# = "
      "a.stmt#") {
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s"},
                                      {ParamType::WILDCARD, "_"}};
    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "a"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s"},
                             {ParamType::ATTRIBUTE_STMT_NUM, "a"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{2}}));
  }

  SECTION("Select s such that Follows(s, _) with s.stmt# = c.value") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s"},
                             {ParamType::ATTRIBUTE_VALUE, "c1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s such that Follows(s, _) with c1.value = c2.value") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::ATTRIBUTE_VALUE, "c1"},
                             {ParamType::ATTRIBUTE_VALUE, "c2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s such that Follows(s, _) with 'x' = 'x'") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::NAME_LITERAL, "x"},
                             {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(TestQueryUtil::getUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select cll such that Follows(_, cll) with cll.procName = 'b'") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "cll"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "cll"},
                             {ParamType::NAME_LITERAL, "b"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{cll}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{3}}));
  }

  SECTION("Select v1 such that ModifiesS(_, v1) with v1.varName = 'a'") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "v1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::ATTRIBUTE_VAR_NAME, "v1"},
                             {ParamType::NAME_LITERAL, "a"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{v1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{varAIdx}}));
  }

  SECTION(
      "Select v1 such that ModifiesS(_, v1) with p1.procName = v1.varName") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "v1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::ATTRIBUTE_VAR_NAME, "v1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{v1}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == vector<vector<int>>({{varAIdx}}));
  }

  SECTION("Select s such that Follows(s, _) with 1 = 2") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::INTEGER_LITERAL, "1"},
                             {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that Follows(s, _) with 'x' = 'y'") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::NAME_LITERAL, "x"},
                             {ParamType::NAME_LITERAL, "y"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    vector<vector<int>> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}
