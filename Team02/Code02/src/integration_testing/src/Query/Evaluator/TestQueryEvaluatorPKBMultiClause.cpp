#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/QueryEvaluator.h>

#include <iostream>

#include "../TestQueryUtil.h"
#include "catch.hpp"
using Catch::Matchers::VectorContains;

using namespace std;
using namespace query;

TEST_CASE("QueryEvaluator: 1 Such That + 1 Pattern Clause") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::ASSIGN, 3);
  pkb->addStmt(DesignEntity::ASSIGN, 4);

  pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
  pkb->addRs(RelationshipType::PARENT, 2, 3);
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 3, "x", "w");
  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 4, "y", "1");
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "w");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int wVarIdx = 2;

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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
  }

  // Test Inner Join Algo - 1 Overlapping Synonym (Case 1)
  SECTION("Select v such that ModifiesS(s1, v) pattern a (v, 'w')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results == FinalQueryResults({{xVarIdx}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results == FinalQueryResults({{xVarIdx}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({3, 4}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({3, 4}));
  }
}

TEST_CASE("QueryEvaluator: Multiple Such That Clauses - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
  pkb->addRs(RelationshipType::FOLLOWS, 2, 3);
  pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
  pkb->addRs(RelationshipType::FOLLOWS_T, 1, 3);
  pkb->addRs(RelationshipType::FOLLOWS_T, 2, 3);
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "x");

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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{2}}));
  }
}

TEST_CASE("QueryEvaluator: Multiple Such That Clauses - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
  pkb->addRs(RelationshipType::FOLLOWS, 2, 3);

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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("2 Follows: False Synonym Clause + False Synonym Clause") {
    // Select s1 such that Follows(s1, 5) and Follows(s1, 6)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "5"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "6"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Multiple Pattern Clauses") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::ASSIGN, 1);
  pkb->addStmt(DesignEntity::ASSIGN, 2);
  pkb->addStmt(DesignEntity::IF, 3);
  pkb->addStmt(DesignEntity::WHILE, 4);
  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 1, "x", "y");
  pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 2, "x", "z");
  pkb->addPatternRs(RelationshipType::PTT_IF, 3, "y");
  pkb->addPatternRs(RelationshipType::PTT_WHILE, 4, "x");

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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{1}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: Test With Clause + Such That/Pattern") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::ASSIGN, 2);
  pkb->addStmt(DesignEntity::ASSIGN, 4);
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "a");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
  pkb->addRs(RelationshipType::FOLLOWS, 2, 3);
  pkb->addStmt(DesignEntity::CALL, 3);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "a",
             TableType::PROC_TABLE, "b");
  pkb->addRs(RelationshipType::CALLS_S, 3, TableType::PROC_TABLE, "b");
  int const1Idx = pkb->insertAt(TableType::CONST_TABLE, "1");
  int const2Idx = pkb->insertAt(TableType::CONST_TABLE, "2");
  int const3Idx = pkb->insertAt(TableType::CONST_TABLE, "3");
  int procAIdx = pkb->insertAt(TableType::PROC_TABLE, "a");
  int procBIdx = pkb->insertAt(TableType::PROC_TABLE, "b");
  int varAIdx = pkb->getIndexOf(TableType::VAR_TABLE, "a");
  int varXIdx = pkb->getIndexOf(TableType::VAR_TABLE, "x");

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

  SECTION("Select c1 with c1.value = 1 with c1.value = 2") {
    WithClause withClause1 = {{ParamType::ATTRIBUTE_VALUE, "c1"},
                              {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back(
        {{}, {}, withClause1, ConditionClauseType::WITH});

    WithClause withClause2 = {{ParamType::ATTRIBUTE_VALUE, "c1"},
                              {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {{}, {}, withClause2, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {c1});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that Follows(s, _) with 1 = 1") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    WithClause withClause = {{ParamType::INTEGER_LITERAL, "1"},
                             {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results == FinalQueryResults({{2}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results == FinalQueryResults({{2}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results == FinalQueryResults({{2}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {cll});
    REQUIRE(results == FinalQueryResults({{3}}));
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
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

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Multiple NextT - Ensure Cache Correctness") {
  // Essentially a duplicate of the unit tests in TestNextOnDemandEvaluator
  // But we have two of the same clause in the query
  // Ensures results are stored and retrieved correctly for NextT

  PKB* pkb = new PKB();
  for (int i = 1; i <= 14; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // example procedure
  // 1: x = 1;
  // 2: while (x == 1) {
  // 3:   if (y == 2) then {
  // 4:     y = 3; }
  // 5:   else { z = 4; } }
  // 6: if (z == 4) then {
  // 7:   if (x == y) then {
  // 8:     z = 3; }
  // 9:   else { y = 4; } }
  // 10: else { z = 6; }
  // 11: while (z == 2) {
  // 12:  while (y == 7) {
  // 13:    z = x; } }
  // 14: x = 5;
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 2);
  pkb->addRs(RelationshipType::NEXT, 5, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 10);
  pkb->addRs(RelationshipType::NEXT, 10, 11);
  pkb->addRs(RelationshipType::NEXT, 7, 8);
  pkb->addRs(RelationshipType::NEXT, 7, 9);
  pkb->addRs(RelationshipType::NEXT, 8, 11);
  pkb->addRs(RelationshipType::NEXT, 9, 11);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addRs(RelationshipType::NEXT, 12, 13);
  pkb->addRs(RelationshipType::NEXT, 13, 12);
  pkb->addRs(RelationshipType::NEXT, 12, 11);
  pkb->addRs(RelationshipType::NEXT, 11, 14);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  /* Integer and Integer --------------------------- */
  SECTION("Select BOOLEAN such that NextT(2, 2) and NextT(2, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({TRUE_SELECT_BOOL_RESULT}));
  }

  /* Integer and Synonym --------------------------- */
  SECTION("Select s2 such that NextT(1, s2) and NextT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
  }

  SECTION("Select s1 such that NextT(s1, 4) and NextT(s1, 4)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5}));
  }

  /* Wildcard and Synonym -------------------------- */
  SECTION("Select s1 such that NextT(s1, _) and NextT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}));
  }

  SECTION("Select s2 such that NextT(_, s2) and NextT(_, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
  }

  /* Synonym and Synonym --------------------------- */
  SECTION("Select <s1, s2> such that NextT(s1, s2) and NextT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, s2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    // check NextT(1, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({1, i})));
    }
    REQUIRE_THAT(results, !VectorContains(vector<int>({1, 1})));
    // check NextT(2, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({2, i})));
    }
    // check NextT(3, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({3, i})));
    }
    // check NextT(4, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({4, i})));
    }
    // check NextT(5, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({5, i})));
    }
    // check NextT(6, _)
    for (int i = 7; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({6, i})));
    }
    // check NextT(7, _)
    for (int i = 8; i <= 14; i++) {
      if (i == 10) {
        REQUIRE_THAT(results, !VectorContains(vector<int>({7, i})));
      } else {
        REQUIRE_THAT(results, VectorContains(vector<int>({7, i})));
      }
    }
    // check NextT(8, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({8, i})));
    }
    // check NextT(9, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({9, i})));
    }
    // check NextT(10, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({10, i})));
    }
    // check NextT(11, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({11, i})));
    }
    // check NextT(12, _)
    for (int i = 12; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({12, i})));
    }
    // check NextT(13, _)
    for (int i = 13; i <= 14; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({13, i})));
    }
  }
}
