#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/QueryEvaluator.h>

#include <iostream>

#include "../TestQueryUtil.h"
#include "catch.hpp"
using namespace std;
using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("QueryEvaluator: Follows (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
  pkb->addRs(RelationshipType::FOLLOWS, 2, 3);
  pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
  pkb->addRs(RelationshipType::FOLLOWS_T, 2, 3);
  pkb->addRs(RelationshipType::FOLLOWS_T, 1, 3);
  pkb->addStmt(DesignEntity::READ, 1);
  pkb->addStmt(DesignEntity::READ, 2);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"r", DesignEntity::READ}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Follows(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that Follows(1, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{2}}));
  }

  SECTION("Select s1 such that Follows(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}}));
  }

  SECTION("Select s1 such that Follows(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s1 such that Follows(_, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3}));
  }

  SECTION("Select s2 such that FollowsT(_, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3}));
  }

  SECTION("Select s1 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s2 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3}));
  }

  SECTION("Select s1 such that Follows(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select r such that FollowsT(_, r)") {
    Synonym r = {DesignEntity::READ, "r", false, {}};
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "r"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {r});
    REQUIRE(results == FinalQueryResults{{2}});
  }
}

TEST_CASE("QueryEvaluator: Follows (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Follows(3, 4)") {
    pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
    pkb->addRs(RelationshipType::FOLLOWS, 2, 3);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Follows(3, _)") {
    pkb->addRs(RelationshipType::FOLLOWS, 1, 2);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Follows(3, s1)") {
    pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
    pkb->addRs(RelationshipType::FOLLOWS, 2, 3);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Follows(s1, 4)") {
    pkb->addRs(RelationshipType::FOLLOWS, 1, 2);
    pkb->addRs(RelationshipType::FOLLOWS, 2, 3);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Follows(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: FollowsT (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
  pkb->addRs(RelationshipType::FOLLOWS_T, 1, 3);
  pkb->addRs(RelationshipType::FOLLOWS_T, 2, 3);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that FollowsT(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}}));
  }

  SECTION("Select s1 such that FollowsT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}}));
  }

  SECTION("Select s2 such that FollowsT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3}));
  }

  SECTION("Select s1 such that FollowsT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s1 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s2 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3}));
  }

  SECTION("Select s1 such that FollowsT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }
}

TEST_CASE("QueryEvaluator: FollowsT (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that FollowsT(3, 4)") {
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 3);
    pkb->addRs(RelationshipType::FOLLOWS_T, 2, 3);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that FollowsT(3, _)") {
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 3);
    pkb->addRs(RelationshipType::FOLLOWS_T, 2, 3);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s2 such that FollowsT(3, s2)") {
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 2);
    pkb->addRs(RelationshipType::FOLLOWS_T, 1, 3);
    pkb->addRs(RelationshipType::FOLLOWS_T, 2, 3);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that FollowsT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Parent (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addRs(RelationshipType::PARENT, 1, 2);
  pkb->addRs(RelationshipType::PARENT, 1, 3);
  pkb->addRs(RelationshipType::PARENT, 3, 4);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Parent(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(1, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3}));
  }

  SECTION("Select s1 such that Parent(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}}));
  }

  SECTION("Select s1 such that Parent(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 3}));
  }

  SECTION("Select s1 such that Parent(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 3}));
  }

  SECTION("Select s2 such that Parent(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});
    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }
}

TEST_CASE("QueryEvaluator: Parent (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Parent(2, 3)") {
    pkb->addRs(RelationshipType::PARENT, 1, 2);
    pkb->addRs(RelationshipType::PARENT, 1, 3);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Parent(3, _)") {
    pkb->addRs(RelationshipType::PARENT, 1, 2);
    pkb->addRs(RelationshipType::PARENT, 1, 3);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Parent(3, s1)") {
    pkb->addRs(RelationshipType::PARENT, 1, 2);
    pkb->addRs(RelationshipType::PARENT, 1, 3);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Parent(s1, 4)") {
    pkb->addRs(RelationshipType::PARENT, 1, 2);
    pkb->addRs(RelationshipType::PARENT, 1, 3);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Parent(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Parent(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: ParentT (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addRs(RelationshipType::PARENT_T, 1, 2);
  pkb->addRs(RelationshipType::PARENT_T, 1, 3);
  pkb->addRs(RelationshipType::PARENT_T, 3, 4);
  pkb->addRs(RelationshipType::PARENT_T, 1, 4);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that ParentT(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s1 such that ParentT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results == FinalQueryResults({{1}, {2}, {3}, {4}}));
  }

  SECTION("Select s2 such that ParentT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that ParentT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 3}));
  }

  SECTION("Select s1 such that ParentT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 3}));
  }

  SECTION("Select s2 such that ParentT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that ParentT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }
}

TEST_CASE("QueryEvaluator: ParentT (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that ParentT(2, 3)") {
    pkb->addRs(RelationshipType::PARENT_T, 1, 2);
    pkb->addRs(RelationshipType::PARENT_T, 1, 3);
    pkb->addRs(RelationshipType::PARENT_T, 3, 4);
    pkb->addRs(RelationshipType::PARENT_T, 1, 4);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that ParentT(2, _)") {
    pkb->addRs(RelationshipType::PARENT_T, 1, 2);
    pkb->addRs(RelationshipType::PARENT_T, 1, 3);
    pkb->addRs(RelationshipType::PARENT_T, 3, 4);
    pkb->addRs(RelationshipType::PARENT_T, 1, 4);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s2 such that ParentT(2, s2)") {
    pkb->addRs(RelationshipType::PARENT_T, 1, 2);
    pkb->addRs(RelationshipType::PARENT_T, 1, 3);
    pkb->addRs(RelationshipType::PARENT_T, 3, 4);
    pkb->addRs(RelationshipType::PARENT_T, 1, 4);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that ParentT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that ParentT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: UsesS (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that UsesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results == FinalQueryResults({{1}}));
  }

  SECTION("Select v such that UsesS(s, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select v such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select v such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: UsesS (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that UsesS(3, 'x')") {
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(3, 'x')") {
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(3, _)") {
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(3, _)") {
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, 'x')") {
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, 'x')") {
    pkb->addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: UsesP (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertAt(TableType::PROC_TABLE, "proc1");
  int proc2Idx = pkb->insertAt(TableType::PROC_TABLE, "proc2");
  int proc3Idx = pkb->insertAt(TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc1",
             TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc1",
             TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc2",
             TableType::VAR_TABLE, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that UsesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that UsesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that UsesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that UsesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx}));
  }

  SECTION("Select p such that UsesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results == FinalQueryResults({{proc1Idx}}));
  }

  SECTION("Select v such that UsesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that UsesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that UsesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: UsesP (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "proc1");
  pkb->insertAt(TableType::PROC_TABLE, "proc2");
  pkb->insertAt(TableType::PROC_TABLE, "proc3");

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that UsesP('proc2', 'x')") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "x");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc2"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP('proc1', 'x')") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "x");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP('proc1', _)") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc3",
               TableType::VAR_TABLE, "x");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP('proc1', _)") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc3",
               TableType::VAR_TABLE, "x");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP('proc1', v)") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "x");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP('proc1', v)") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "x");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP(p, 'x')") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP(p, 'x')") {
    pkb->addRs(RelationshipType::USES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: ModifiesS (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that ModifiesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select v such that ModifiesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that ModifiesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select v such that ModifiesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that ModifiesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select v such that ModifiesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: ModifiesS (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that ModifiesS(3, 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(3, 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(3, _)") {
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(3, _)") {
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y");
    pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "z");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(s, 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(s, 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: ModifiesP (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertAt(TableType::PROC_TABLE, "proc1");
  int proc2Idx = pkb->insertAt(TableType::PROC_TABLE, "proc2");
  int proc3Idx = pkb->insertAt(TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
             TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
             TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc2",
             TableType::VAR_TABLE, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that ModifiesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that ModifiesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that ModifiesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that ModifiesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx}));
  }

  SECTION("Select p such that ModifiesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results == FinalQueryResults({{proc1Idx}}));
  }

  SECTION("Select v such that ModifiesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that ModifiesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that ModifiesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: ModifiesP (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that ModifiesP('proc3', 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP('proc3', 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP('proc3', _)") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP('proc3', _)") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP('proc3', v)") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP('proc3', v)") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "x");
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc2",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP(p, 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP(p, 'x')") {
    pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "proc1",
               TableType::VAR_TABLE, "y");

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p});
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Calls - Truthy Values") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertAt(TableType::PROC_TABLE, "proc1");
  int proc2Idx = pkb->insertAt(TableType::PROC_TABLE, "proc2");
  int proc3Idx = pkb->insertAt(TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_S, 2, TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_S, 3, TableType::PROC_TABLE, "proc3");

  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE}, {"p2", DesignEntity::PROCEDURE}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1", false, {}};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p1 such that Calls('proc1', 'proc2')") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select p1 such that Calls('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select p1 such that Calls(_, 'proc2')") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select p2 such that Calls('proc1', p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc2Idx, proc3Idx}));
  }

  SECTION("Select p1 such that Calls(p1, 'proc3')") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::NAME_LITERAL, "proc3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select p2 such that Calls(_, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc2Idx, proc3Idx}));
  }

  SECTION("Select p1 such that Calls(p1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select p1 such that Calls(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select p1 such that Calls(p1, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx}));
  }
}

TEST_CASE("QueryEvaluator: Calls - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "proc1");
  pkb->insertAt(TableType::PROC_TABLE, "proc2");
  pkb->insertAt(TableType::PROC_TABLE, "proc3");

  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE}, {"p2", DesignEntity::PROCEDURE}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1", false, {}};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p1 such that Calls('proc1', 'proc2')") {
    pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_S, 2, TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that Calls('proc1', _)") {
    pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that Calls(_, 'proc2')") {
    pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_S, 2, TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p2 such that Calls('proc1', p2)") {
    pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that Calls(p1, 'proc3')") {
    pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
               TableType::PROC_TABLE, "proc2");
    pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc2");

    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::NAME_LITERAL, "proc3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p2 such that Calls(_, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that Calls(p1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that Calls(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that Calls(p1, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: CallsT - Truthy Values") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertAt(TableType::PROC_TABLE, "proc1");
  int proc2Idx = pkb->insertAt(TableType::PROC_TABLE, "proc2");
  int proc3Idx = pkb->insertAt(TableType::PROC_TABLE, "proc3");
  int proc4Idx = pkb->insertAt(TableType::PROC_TABLE, "proc4");
  int proc5Idx = pkb->insertAt(TableType::PROC_TABLE, "proc5");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS_S, 1, TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc2",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_S, 2, TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "proc4",
             TableType::PROC_TABLE, "proc5");
  pkb->addRs(RelationshipType::CALLS_S, 4, TableType::PROC_TABLE, "proc5");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc2");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc2",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
             TableType::PROC_TABLE, "proc3");
  pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc4",
             TableType::PROC_TABLE, "proc5");

  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE}, {"p2", DesignEntity::PROCEDURE}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1", false, {}};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p1 such that CallsT('proc1', 'proc3')") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "proc3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx, proc4Idx, proc5Idx}));
  }

  SECTION("Select p1 such that CallsT('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx, proc4Idx, proc5Idx}));
  }

  SECTION("Select p1 such that CallsT(_, 'proc2')") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx, proc4Idx, proc5Idx}));
  }

  SECTION("Select p2 such that CallsT('proc1', p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc2Idx, proc3Idx}));
  }

  SECTION("Select p1 such that CallsT(p1, 'proc5')") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::NAME_LITERAL, "proc5"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc4Idx}));
  }

  SECTION("Select p2 such that CallsT(_, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc2Idx, proc3Idx, proc5Idx}));
  }

  SECTION("Select p1 such that CallsT(p1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc4Idx}));
  }

  SECTION("Select p1 such that CallsT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc3Idx, proc4Idx, proc5Idx}));
  }

  SECTION("Select p1 such that CallsT(p1, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc1Idx, proc2Idx, proc4Idx}));
  }

  SECTION("Select p2 such that CallsT(p1, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({proc2Idx, proc3Idx, proc5Idx}));
  }
}

TEST_CASE("QueryEvaluator: CallsT - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "proc1");
  pkb->insertAt(TableType::PROC_TABLE, "proc2");
  pkb->insertAt(TableType::PROC_TABLE, "proc3");

  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE}, {"p2", DesignEntity::PROCEDURE}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1", false, {}};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p1 such that CallsT('proc1', 'proc2')") {
    pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that CallsT('proc1', _)") {
    pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that CallsT(_, 'proc2')") {
    pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
               TableType::PROC_TABLE, "proc3");
    pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p2 such that CallsT('proc1', p2)") {
    pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc2",
               TableType::PROC_TABLE, "proc3");

    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "proc2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that CallsT(p1, 'proc3')") {
    pkb->addRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "proc1",
               TableType::PROC_TABLE, "proc2");

    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::NAME_LITERAL, "proc3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p2 such that CallsT(_, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p2});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that CallsT(p1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that CallsT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 such that CallsT(p1, p2)") {
    SuchThatClause suchThatClause = {RelationshipType::CALLS_T,
                                     {ParamType::SYNONYM, "p1"},
                                     {ParamType::SYNONYM, "p2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Next - Truthy Values") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 8; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // example procedure
  // 1: x = 1;
  // 2: while (x == 1) {
  // 3:   if (y == 2) then {
  // 4:     y = 3; }
  // 5:   else { z = 4; } }
  // 6: if (z == 4) then {
  // 7:   x = y; }
  // 8: else { z = 6; }
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 2);
  pkb->addRs(RelationshipType::NEXT, 5, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 8);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Next(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8}));
  }

  SECTION("Select s1 such that Next(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8}));
  }

  SECTION("Select s1 such that Next(4, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::INTEGER_LITERAL, "4"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8}));
  }

  SECTION("Select s1 such that Next(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 4, 5}));
  }

  SECTION("Select s2 such that Next(6, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::INTEGER_LITERAL, "6"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({7, 8}));
  }

  SECTION("Select s1 such that Next(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s2 such that Next(_, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3, 4, 5, 6, 7, 8}));
  }

  SECTION("Select <s1, s2> such that Next(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, s2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 6})));
    REQUIRE_THAT(results, VectorContains(vector<int>({6, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({6, 8})));
  }
}

TEST_CASE("QueryEvaluator: Next - Falsy Values") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 6; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // example procedure
  // 1: y = 2;
  // 2: while (x == 1) {
  // 3:   if (y == 2) then {
  // 4:     y = 3; }
  // 5:   else { z = 4; } }
  // 6: x = 1;
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 2);
  pkb->addRs(RelationshipType::NEXT, 5, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 6);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"a", DesignEntity::ASSIGN},
      {"w", DesignEntity::WHILE},
      {"ifs", DesignEntity::IF}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym a = {DesignEntity::ASSIGN, "a", false, {}};
  Synonym w = {DesignEntity::WHILE, "w", false, {}};
  Synonym ifs = {DesignEntity::IF, "ifs", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Next(_, _)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Next(1, 3)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Next(6, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::INTEGER_LITERAL, "6"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Next(s1, 1)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s2 such that Next(6, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::INTEGER_LITERAL, "6"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Next(s1, _)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s2 such that Next(_, s2)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(results.empty());
  }

  SECTION("Select <s1, s2> such that Next(s1, s2)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, s2});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: NextT - Truthy Values") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 9; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  pkb->addStmt(DesignEntity::ASSIGN, 1);
  pkb->addStmt(DesignEntity::WHILE, 2);
  pkb->addStmt(DesignEntity::IF, 3);
  pkb->addStmt(DesignEntity::ASSIGN, 4);
  pkb->addStmt(DesignEntity::ASSIGN, 5);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addStmt(DesignEntity::ASSIGN, 7);
  pkb->addStmt(DesignEntity::PRINT, 8);
  pkb->addStmt(DesignEntity::READ, 9);
  // example procedure
  // 1: x = 1;
  // 2: while (x == 1) {
  // 3:   if (y == 2) then {
  // 4:     y = 3; }
  // 5:   else { z = 4; } }
  // 6: if (z == 4) then {
  // 7:   x = y; }
  // 8: else { print z; }
  // 9: read xyz;
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 2);
  pkb->addRs(RelationshipType::NEXT, 5, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 8);
  pkb->addRs(RelationshipType::NEXT, 7, 9);
  pkb->addRs(RelationshipType::NEXT, 8, 9);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"pr", DesignEntity::PRINT},
      {"w", DesignEntity::WHILE},
      {"ifs", DesignEntity::IF}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym pr = {DesignEntity::PRINT, "pr", false, {}};
  Synonym w = {DesignEntity::WHILE, "w", false, {}};
  Synonym ifs = {DesignEntity::IF, "ifs", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that NextT(1, 3)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  SECTION("Select s1 such that NextT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  SECTION("Select s1 such that NextT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  SECTION("Select s1 such that NextT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6, 7, 8}));
  }

  SECTION("Select s2 such that NextT(_, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3, 4, 5, 6, 7, 8, 9}));
  }

  SECTION("Select s1 such that NextT(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5}));
  }

  SECTION("Select s2 such that NextT(6, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "6"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({7, 8, 9}));
  }

  SECTION("Select s1 such that NextT(s1, pr)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "pr"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select ifs such that NextT(w, ifs)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "w"},
                                     {ParamType::SYNONYM, "ifs"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {ifs});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({3, 6}));
  }

  SECTION("Select <s1, s2> such that NextT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, s2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    // NextT(1, _), NextT(2, _), NextT(3, _), NextT(4, _), NextT(5, _)
    for (int i = 1; i <= 5; i++) {
      for (int j = 2; j <= 9; j++) {
        REQUIRE_THAT(results, VectorContains(vector<int>({i, j})));
      }
    }
    // NextT(6, _)
    for (int i = 7; i <= 9; i++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({6, i})));
    }
    // NextT(7, _)
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 9})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 8})));
    // NextT(8, _)
    REQUIRE_THAT(results, VectorContains(vector<int>({8, 9})));
  }
}

TEST_CASE("QueryEvaluator: NextT - Falsy Values") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 9; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  pkb->addStmt(DesignEntity::ASSIGN, 1);
  pkb->addStmt(DesignEntity::WHILE, 2);
  pkb->addStmt(DesignEntity::IF, 3);
  pkb->addStmt(DesignEntity::ASSIGN, 4);
  pkb->addStmt(DesignEntity::ASSIGN, 5);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addStmt(DesignEntity::ASSIGN, 7);
  pkb->addStmt(DesignEntity::PRINT, 8);
  pkb->addStmt(DesignEntity::READ, 9);
  // example procedure
  // 1: x = 1;
  // 2: while (x == 1) {
  // 3:   if (y == 2) then {
  // 4:     y = 3; }
  // 5:   else { z = 4; } }
  // 6: if (z == 4) then {
  // 7:   x = y; }
  // 8: else { print z; }
  // 9: read xyz;
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 2);
  pkb->addRs(RelationshipType::NEXT, 5, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 8);
  pkb->addRs(RelationshipType::NEXT, 7, 9);
  pkb->addRs(RelationshipType::NEXT, 8, 9);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"w", DesignEntity::WHILE},
      {"pr", DesignEntity::PRINT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym w = {DesignEntity::WHILE, "w", false, {}};
  Synonym pr = {DesignEntity::PRINT, "pr", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that NextT(1, 10)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "10"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that NextT(9, _)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "9"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that NextT(_, _)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that NextT(s1, _)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s2 such that NextT(_, s2)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(results.empty());
  }

  SECTION("Select pr such that NextT(pr, 7)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "pr"},
                                     {ParamType::INTEGER_LITERAL, "7"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {pr});
    REQUIRE(results.empty());
  }

  SECTION("Select w such that NextT(6, w)") {
    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::INTEGER_LITERAL, "6"},
                                     {ParamType::SYNONYM, "w"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {w});
    REQUIRE(results.empty());
  }

  SECTION("Select <s1, s2> such that NextT(s1, s2)") {
    PKB* emptyPkb = new PKB();

    SuchThatClause suchThatClause = {RelationshipType::NEXT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        emptyPkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, s2});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: NextBip & NextBipT") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 6; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // proc A {
  // 1: while (x != 0) {
  // 2:   x = x - 1; }}
  // proc B {
  // 3: if (x == 2) then {
  // 4:   read x; }
  // 5: else { x = 3; } }
  // proc C {
  // 6: y = 2; }

  RelationshipType nextBipRsType = RelationshipType::NEXT_BIP;
  RelationshipType nextBipTRsType = RelationshipType::NEXT_BIP_T;
  pkb->addRs(nextBipRsType, 1, 2);
  pkb->addRs(nextBipRsType, 2, 1);
  pkb->addRs(nextBipRsType, 1, 3);
  pkb->addRs(nextBipRsType, 3, 4);
  pkb->addRs(nextBipRsType, 3, 5);
  pkb->addRs(nextBipRsType, 4, 6);
  pkb->addRs(nextBipRsType, 5, 6);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  /* NextBip ------------------------------------------------------ */
  SECTION("Select s1 such that NextBip(1, 3)") {
    SuchThatClause suchThatClause = {nextBipRsType,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s1 such that NextBip(_, _)") {
    SuchThatClause suchThatClause = {
        nextBipRsType, {ParamType::WILDCARD, "_"}, {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s1 such that NextBip(4, _)") {
    SuchThatClause suchThatClause = {nextBipRsType,
                                     {ParamType::INTEGER_LITERAL, "4"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s1 such that NextBip(s1, 6)") {
    SuchThatClause suchThatClause = {nextBipRsType,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "6"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({4, 5}));
  }

  SECTION("Select s2 such that NextBip(1, s2)") {
    SuchThatClause suchThatClause = {nextBipRsType,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({2, 3}));
  }

  SECTION("Select s1 such that NextBip(s1, _)") {
    SuchThatClause suchThatClause = {
        nextBipRsType, {ParamType::SYNONYM, "s1"}, {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5}));
  }

  SECTION("Select s2 such that NextBip(_, s2)") {
    SuchThatClause suchThatClause = {
        nextBipRsType, {ParamType::WILDCARD, "_"}, {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select <s1, s2> such that NextBip(s1, s2)") {
    SuchThatClause suchThatClause = {
        nextBipRsType, {ParamType::SYNONYM, "s1"}, {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, s2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 6})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 6})));
  }

  /* NextBipT ------------------------------------------------------ */
  SECTION("Select s1 such that NextBipT(1, 4)") {
    SuchThatClause suchThatClause = {nextBipTRsType,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s1 such that NextBipT(_, _)") {
    SuchThatClause suchThatClause = {
        nextBipTRsType, {ParamType::WILDCARD, "_"}, {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s1 such that NextBipT(4, _)") {
    SuchThatClause suchThatClause = {nextBipTRsType,
                                     {ParamType::INTEGER_LITERAL, "4"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s1 such that NextBipT(s1, 6)") {
    SuchThatClause suchThatClause = {nextBipTRsType,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "6"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5}));
  }

  SECTION("Select s2 such that NextBipT(1, s2)") {
    SuchThatClause suchThatClause = {nextBipTRsType,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select s2 such that NextBipT(4, s2)") {
    SuchThatClause suchThatClause = {nextBipTRsType,
                                     {ParamType::INTEGER_LITERAL, "4"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({6}));
  }

  SECTION("Select s1 such that NextBipT(s1, _)") {
    SuchThatClause suchThatClause = {
        nextBipTRsType, {ParamType::SYNONYM, "s1"}, {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5}));
  }

  SECTION("Select s2 such that NextBipT(_, s2)") {
    SuchThatClause suchThatClause = {
        nextBipTRsType, {ParamType::WILDCARD, "_"}, {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s2});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("Select <s1, s2> such that NextBipT(s1, s2)") {
    SuchThatClause suchThatClause = {
        nextBipTRsType, {ParamType::SYNONYM, "s1"}, {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, s2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    for (int i = 1; i < 6; i++) {
      for (int j = i + 1; j <= 6; j++) {
        if (i == 4 && j == 5) {
          REQUIRE_THAT(results, !VectorContains(vector<int>({i, j})));
        } else {
          REQUIRE_THAT(results, VectorContains(vector<int>({i, j})));
        }
      }
    }
  }
}

TEST_CASE("QueryEvaluator: Affects - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 9);
  for (int i = 1; i <= 13; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 7, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 2);
  pkb->addRs(RelationshipType::NEXT, 5, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 8);

  pkb->addRs(RelationshipType::NEXT, 9, 10);
  pkb->addRs(RelationshipType::NEXT, 10, 9);
  pkb->addRs(RelationshipType::NEXT, 9, 11);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addRs(RelationshipType::NEXT, 11, 13);

  unordered_set<int> whileStmts = {2, 6, 9};
  for (int w : whileStmts) {
    pkb->addStmt(DesignEntity::WHILE, w);
  }
  unordered_set<int> ifStmts = {4, 11};
  for (int ifs : ifStmts) {
    pkb->addStmt(DesignEntity::IF, ifs);
  }
  pkb->addNextStmtForIfStmt(4, 2);
  for (int i = 1; i <= 13; i++) {
    if (whileStmts.find(i) == whileStmts.end() &&
        ifStmts.find(i) == ifStmts.end()) {
      pkb->addStmt(DesignEntity::ASSIGN, i);
    }
  }

  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 8, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 8, TableType::VAR_TABLE, "x");

  pkb->addRs(RelationshipType::MODIFIES_S, 10, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 12, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 13, TableType::VAR_TABLE, "y");

  // procedure A {
  // 1: x = 0;
  // 2: while (i != 0) {
  // 3:   x = x + 2 * y;
  // 4:   if (i != 0) then {
  // 5:     x = x + 2; }
  // NA:  else {
  // 6:     while (x != y) {
  // 7:       x = x; } }
  // 8: y = x + 1;
  // }
  // procedure B {
  // 9: while (x == 2) {
  // 10:  x = 3; }
  // 11: if (z != 0) then {
  // 12:  y = x + 3; }
  // 13: else { y = 2; }
  // }

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"a1", DesignEntity::ASSIGN},
      {"a2", DesignEntity::ASSIGN}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym a1 = {DesignEntity::ASSIGN, "a1", false, {}};
  Synonym a2 = {DesignEntity::ASSIGN, "a2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  /* Affects Bool -------------------------------------------- */
  SECTION("Select BOOLEAN such that Affects(5, 8)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::INTEGER_LITERAL, "5"},
                                     {ParamType::INTEGER_LITERAL, "8"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results,
                 VectorContains(vector<int>({TRUE_SELECT_BOOL_RESULT})));
  }

  SECTION("Select BOOLEAN such that Affects(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results,
                 VectorContains(vector<int>({TRUE_SELECT_BOOL_RESULT})));
  }

  SECTION("Select a1 such that Affects(3, 3)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a1});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    vector<int> expectedResults = vector<int>({1, 3, 5, 7, 8, 10, 12, 13});
    for (auto stmt : expectedResults) {
      REQUIRE_THAT(results, VectorContains(vector<int>({stmt})));
    }
  }

  SECTION("Select a1 such that Affects(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a1});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    vector<int> expectedResults = vector<int>({1, 3, 5, 7, 8, 10, 12, 13});
    for (auto stmt : expectedResults) {
      REQUIRE_THAT(results, VectorContains(vector<int>({stmt})));
    }
  }

  SECTION("Select s1 such that Affects(_, 8)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::INTEGER_LITERAL, "8"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    for (int stmt = 1; stmt <= 13; stmt++) {
      REQUIRE_THAT(results, VectorContains(vector<int>({stmt})));
    }
  }

  /* Affects Stmts ------------------------------------------- */
  SECTION("Select a1 such that Affects(a1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::SYNONYM, "a1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a1});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    vector<int> expectedResults = vector<int>({1, 3, 5, 7, 10});
    for (auto stmt : expectedResults) {
      REQUIRE_THAT(results, VectorContains(vector<int>({stmt})));
    }
  }

  SECTION("Select a2 such that Affects(_, a2)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "a2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    vector<int> expectedResults = vector<int>({3, 5, 7, 8, 12});
    for (auto stmt : expectedResults) {
      REQUIRE_THAT(results, VectorContains(vector<int>({stmt})));
    }
  }

  SECTION("Select a1 such that Affects(a1, 8)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::SYNONYM, "a1"},
                                     {ParamType::INTEGER_LITERAL, "8"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a1});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    vector<int> expectedResults = vector<int>({1, 3, 5, 7});
    for (auto stmt : expectedResults) {
      REQUIRE_THAT(results, VectorContains(vector<int>({stmt})));
    }
  }

  SECTION("Select a2 such that Affects(10, a2)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::INTEGER_LITERAL, "10"},
                                     {ParamType::SYNONYM, "a2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({12})));
  }

  /* Affects Pairs ------------------------------------------- */
  SECTION("Select <a1, a2> such that Affects(a1, a2)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::SYNONYM, "a1"},
                                     {ParamType::SYNONYM, "a2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a1, a2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 12})));

    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 5})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({10, 10})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({12, 13})));
  }

  SECTION("Select <s1, a2> such that Affects(s1, a2)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "a2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1, a2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 12})));

    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 5})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({10, 10})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({12, 13})));
  }
}

TEST_CASE("QueryEvaluator: Affects") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 8; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 7, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 2);
  pkb->addRs(RelationshipType::NEXT, 5, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 8);

  unordered_set<int> whileStmts = {2, 6};
  for (int w : whileStmts) {
    pkb->addStmt(DesignEntity::WHILE, w);
  }
  unordered_set<int> ifStmts = {4};
  for (int ifs : ifStmts) {
    pkb->addStmt(DesignEntity::IF, ifs);
  }
  pkb->addNextStmtForIfStmt(4, 2);
  unordered_set<int> assignStmts = {1, 3, 7};
  for (int a : assignStmts) {
    pkb->addStmt(DesignEntity::ASSIGN, a);
  }

  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 8, TableType::VAR_TABLE, "x");

  // procedure A {
  // 1: x = 0;
  // 2: while (i != 0) {
  // 3:   x = 1;
  // 4:   if (i != 0) then {
  // 5:     read x; }
  // NA:  else {
  // 6:     while (x != y) {
  // 7:       x = y; } }
  // 8: print x;
  // }

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"a1", DesignEntity::ASSIGN},
      {"a2", DesignEntity::ASSIGN}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym a1 = {DesignEntity::ASSIGN, "a1", false, {}};
  Synonym a2 = {DesignEntity::ASSIGN, "a2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  /* Affects Bool -------------------------------------------- */
  SECTION("Select BOOLEAN such that Affects(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results,
                 VectorContains(vector<int>({FALSE_SELECT_BOOL_RESULT})));
  }

  SECTION("Select BOOLEAN such that Affects(1, 3)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results,
                 VectorContains(vector<int>({FALSE_SELECT_BOOL_RESULT})));
  }

  /* Affects Stmts ------------------------------------------- */
  SECTION("Select a1 such that Affects(a1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::SYNONYM, "a1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 such that Affects(_, a2)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "a2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  /* Affects Pairs ------------------------------------------- */
  SECTION("Select <a1, a2> such that Affects(a1, a2)") {
    SuchThatClause suchThatClause = {RelationshipType::AFFECTS,
                                     {ParamType::SYNONYM, "a1"},
                                     {ParamType::SYNONYM, "a2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, {}, ConditionClauseType::SUCH_THAT});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a1, a2});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Assignment Pattern (1 Clause) - Truthy Values") {
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

  unordered_map<string, DesignEntity> synonyms = {
      {"a", DesignEntity::ASSIGN}, {"v", DesignEntity::VARIABLE}};
  Synonym a = {DesignEntity::ASSIGN, "a", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select a pattern a ('x', 'w')") {
    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{1}}));
  }

  SECTION("Select a pattern a ('x', _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{1}}));
  }

  SECTION("Select a pattern a ('x', _)") {
    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{1}}));
  }

  SECTION("Select a pattern a (_, 'w')") {
    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select a pattern a (_, _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select a pattern a (_, _)") {
    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select a pattern a (v, 'w')") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{1}, {2}}));
  }

  SECTION("Select a pattern a (v, _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results == FinalQueryResults({{1}, {2}}));
  }

  SECTION("Select a pattern a (v, _)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select v pattern a (v, 'w')") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results == FinalQueryResults({{xVarIdx}, {yVarIdx}}));
  }

  SECTION("Select v pattern a (v, _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results == FinalQueryResults({{xVarIdx}, {yVarIdx}}));
  }

  SECTION("Select v pattern a (v, _)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: Assignment Pattern (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"a", DesignEntity::ASSIGN}, {"v", DesignEntity::VARIABLE}};
  Synonym a = {DesignEntity::ASSIGN, "a", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select a pattern a ('x', 'w')") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a ('x', _'w'_)") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a ('x', _)") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 1, "y", "www");

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (_, 'w')") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (_, _'w'_)") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (_, _)") {
    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (v, 'w')") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (v, _'w'_)") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (v, _)") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {a});
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern a (v, 'w')") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern a (v, _'w'_)") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);
    pkb->addPatternRs(RelationshipType::PTT_ASSIGN_SUB_EXPR, 1, "x", "www");

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern a (v, _)") {
    pkb->addStmt(DesignEntity::ASSIGN, 1);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: If Pattern (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::IF, 1);
  pkb->addStmt(DesignEntity::IF, 2);
  pkb->addStmt(DesignEntity::IF, 3);

  pkb->addPatternRs(RelationshipType::PTT_IF, 1, "x");
  pkb->addPatternRs(RelationshipType::PTT_IF, 1, "y");
  pkb->addPatternRs(RelationshipType::PTT_IF, 2, "x");
  pkb->addPatternRs(RelationshipType::PTT_IF, 3, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"ifs", DesignEntity::IF}, {"v", DesignEntity::VARIABLE}};
  Synonym ifs = {DesignEntity::IF, "ifs", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select ifs pattern ifs ('x', _, _)") {
    PatternClause patternClause = {ifs, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {ifs});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select ifs pattern ifs (_, _, _)") {
    PatternClause patternClause = {ifs, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {ifs});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select ifs pattern ifs (v, _, _)") {
    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {ifs});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select v pattern ifs (v, _, _)") {
    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: If Pattern (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"ifs", DesignEntity::IF}, {"v", DesignEntity::VARIABLE}};
  Synonym ifs = {DesignEntity::IF, "ifs", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select ifs pattern ifs ('x', _, _)") {
    pkb->addStmt(DesignEntity::IF, 1);
    pkb->addPatternRs(RelationshipType::PTT_IF, 1, "y");

    PatternClause patternClause = {ifs, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {ifs});
    REQUIRE(results.empty());
  }

  SECTION("Select ifs pattern ifs (_, _, _)") {
    PatternClause patternClause = {ifs, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {ifs});
    REQUIRE(results.empty());
  }

  SECTION("Select ifs pattern ifs (v, _, _)") {
    pkb->addStmt(DesignEntity::IF,
                 1);  // e.g. if condition has const but no var

    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {ifs});
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern ifs (v, _, _)") {
    pkb->addStmt(DesignEntity::IF, 1);

    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: While Pattern (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::WHILE, 1);
  pkb->addStmt(DesignEntity::WHILE, 2);
  pkb->addStmt(DesignEntity::WHILE, 3);
  pkb->addStmt(DesignEntity::WHILE, 4);

  pkb->addPatternRs(RelationshipType::PTT_WHILE, 1, "x");
  pkb->addPatternRs(RelationshipType::PTT_WHILE, 1, "y");
  pkb->addPatternRs(RelationshipType::PTT_WHILE, 2, "x");
  pkb->addPatternRs(RelationshipType::PTT_WHILE, 3, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"w", DesignEntity::WHILE}, {"v", DesignEntity::VARIABLE}};
  Synonym w = {DesignEntity::WHILE, "w", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select w pattern w ('x', _)") {
    PatternClause patternClause = {w, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {w});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select w pattern w (_, _)") {
    PatternClause patternClause = {w, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {w});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select w pattern w (v, _)") {
    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {w});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3}));
  }

  SECTION("Select v pattern w (v, _)") {
    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: While Pattern (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"w", DesignEntity::WHILE}, {"v", DesignEntity::VARIABLE}};
  Synonym w = {DesignEntity::WHILE, "w", false, {}};
  Synonym v = {DesignEntity::VARIABLE, "v", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select w pattern w ('x', _, _)") {
    pkb->addStmt(DesignEntity::WHILE, 1);
    pkb->addPatternRs(RelationshipType::PTT_WHILE, 1, "y");

    PatternClause patternClause = {w, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {w});
    REQUIRE(results.empty());
  }

  SECTION("Select w pattern w (_, _, _)") {
    PatternClause patternClause = {w, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {w});
    REQUIRE(results.empty());
  }

  SECTION("Select w pattern w (v, _, _)") {
    pkb->addStmt(DesignEntity::WHILE,
                 1);  // e.g. while condition has const but no var

    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {w});
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern w (v, _, _)") {
    pkb->addStmt(DesignEntity::WHILE, 1);

    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, {}, ConditionClauseType::PATTERN});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: With (Name Attributes) - Truthy Values") {
  PKB* pkb = new PKB();
  int procAIdx = pkb->insertAt(TableType::PROC_TABLE, "A");
  int procBIdx = pkb->insertAt(TableType::PROC_TABLE, "B");
  int varAIdx = pkb->insertAt(TableType::VAR_TABLE, "A");
  int varBIdx = pkb->insertAt(TableType::VAR_TABLE, "B");

  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE},
      {"p2", DesignEntity::PROCEDURE},
      {"v1", DesignEntity::VARIABLE},
      {"v2", DesignEntity::VARIABLE}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1", false, {}};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2", false, {}};
  Synonym v1 = {DesignEntity::VARIABLE, "v1", false, {}};
  Synonym v2 = {DesignEntity::VARIABLE, "v2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p1 with p1.procName = p2.procName") {
    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::ATTRIBUTE_PROC_NAME, "p2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({procAIdx, procBIdx}));
  }

  SECTION("Select p1 with p1.procName = v1.varName") {
    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::ATTRIBUTE_VAR_NAME, "v1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({procAIdx, procBIdx}));
  }

  SECTION("Select v1 with p1.procName = v1.varName") {
    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::ATTRIBUTE_VAR_NAME, "v1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({varAIdx, varBIdx}));
  }

  SECTION("Select p1 with p1.procName = 'A'") {
    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::NAME_LITERAL, "A"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({procAIdx}));
  }

  SECTION("Select v1 with v1.varName = 'A'") {
    WithClause withClause = {{ParamType::ATTRIBUTE_VAR_NAME, "v1"},
                             {ParamType::NAME_LITERAL, "A"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({varAIdx}));
  }

  SECTION("Select v1 with 'A' = 'A'") {
    WithClause withClause = {{ParamType::NAME_LITERAL, "A"},
                             {ParamType::NAME_LITERAL, "A"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({varAIdx, varBIdx}));
  }

  SECTION("Select BOOLEAN with 'A' = 'A'") {
    WithClause withClause = {{ParamType::NAME_LITERAL, "A"},
                             {ParamType::NAME_LITERAL, "A"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({TRUE_SELECT_BOOL_RESULT}));
  }
}

TEST_CASE("QueryEvaluator: With (Name Attributes) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE},
      {"p2", DesignEntity::PROCEDURE},
      {"v1", DesignEntity::VARIABLE},
      {"v2", DesignEntity::VARIABLE}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1", false, {}};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2", false, {}};
  Synonym v1 = {DesignEntity::VARIABLE, "v1", false, {}};
  Synonym v2 = {DesignEntity::VARIABLE, "v2", false, {}};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p1 with p1.procName = p2.procName") {
    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::ATTRIBUTE_PROC_NAME, "p2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 with p1.procName = v1.varName") {
    pkb->insertAt(TableType::PROC_TABLE, "A");
    pkb->insertAt(TableType::VAR_TABLE, "x");

    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::ATTRIBUTE_VAR_NAME, "v1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select v1 with p1.procName = v1.varName") {
    pkb->insertAt(TableType::PROC_TABLE, "A");
    pkb->insertAt(TableType::VAR_TABLE, "x");

    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::ATTRIBUTE_VAR_NAME, "v1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v1});
    REQUIRE(results.empty());
  }

  SECTION("Select p1 with p1.procName = 'aaa'") {
    pkb->insertAt(TableType::PROC_TABLE, "AAA");

    WithClause withClause = {{ParamType::ATTRIBUTE_PROC_NAME, "p1"},
                             {ParamType::NAME_LITERAL, "aaa"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {p1});
    REQUIRE(results.empty());
  }

  SECTION("Select v1 with v1.varName = 'xxx'") {
    pkb->insertAt(TableType::VAR_TABLE, "x");

    WithClause withClause = {{ParamType::ATTRIBUTE_VAR_NAME, "v1"},
                             {ParamType::NAME_LITERAL, "a"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v1});
    REQUIRE(results.empty());
  }

  SECTION("Select v1 with 'A' = 'a'") {
    pkb->insertAt(TableType::VAR_TABLE, "x");

    WithClause withClause = {{ParamType::NAME_LITERAL, "A"},
                             {ParamType::NAME_LITERAL, "a"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {v1});
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: With (Integer Attributes) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  int const1Idx = pkb->insertAt(TableType::CONST_TABLE, "1");
  int const2Idx = pkb->insertAt(TableType::CONST_TABLE, "2");

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
      {"n1", DesignEntity::PROG_LINE}, {"n2", DesignEntity::PROG_LINE},
      {"c1", DesignEntity::CONSTANT},  {"c2", DesignEntity::CONSTANT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym n1 = {DesignEntity::PROG_LINE, "n1", false, {}};
  Synonym n2 = {DesignEntity::PROG_LINE, "n2", false, {}};
  Synonym c1 = {DesignEntity::CONSTANT, "c1", false, {}};
  Synonym c2 = {DesignEntity::CONSTANT, "c2", false, {}};

  vector<ConditionClause> conditionClauses = {};

  SECTION("Select n1 with n1 = n2") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::SYNONYM, "n2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select <n1, n2> with n1 = n2") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::SYNONYM, "n2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults setOfResults = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1, n2});
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 4})));
  }

  SECTION("Select n1 with n1 = s1.stmt#") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::ATTRIBUTE_STMT_NUM, "s1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select n1 with n1 = c1.value") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::ATTRIBUTE_VALUE, "c1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select n1 with n1 = 1") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1}));
  }

  SECTION("Select s1 with s1.stmt# = s2.stmt#") {
    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s1"},
                             {ParamType::ATTRIBUTE_STMT_NUM, "s2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 with s1.stmt# = c1.value") {
    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s1"},
                             {ParamType::ATTRIBUTE_VALUE, "c1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1, 2}));
  }

  SECTION("Select s1 with s1.stmt# = 1") {
    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s1"},
                             {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({1}));
  }

  SECTION("Select c1 with c1.value = c2.value") {
    WithClause withClause = {{ParamType::ATTRIBUTE_VALUE, "c1"},
                             {ParamType::ATTRIBUTE_VALUE, "c2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {c1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({const1Idx, const2Idx}));
  }

  SECTION("Select c1 with c1.value = 2") {
    WithClause withClause = {{ParamType::ATTRIBUTE_VALUE, "c1"},
                             {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {c1});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({const2Idx}));
  }

  SECTION("Select BOOLEAN with 1 = 1") {
    WithClause withClause = {{ParamType::INTEGER_LITERAL, "1"},
                             {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({TRUE_SELECT_BOOL_RESULT}));
  }
}

TEST_CASE("QueryEvaluator: With (Integer Attributes) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
      {"n1", DesignEntity::PROG_LINE}, {"n2", DesignEntity::PROG_LINE},
      {"c1", DesignEntity::CONSTANT},  {"c2", DesignEntity::CONSTANT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym n1 = {DesignEntity::PROG_LINE, "n1", false, {}};
  Synonym n2 = {DesignEntity::PROG_LINE, "n2", false, {}};
  Synonym c1 = {DesignEntity::CONSTANT, "c1", false, {}};
  Synonym c2 = {DesignEntity::CONSTANT, "c2", false, {}};

  vector<ConditionClause> conditionClauses = {};

  SECTION("Select n1 with n1 = n2") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::SYNONYM, "n2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(results.empty());
  }

  SECTION("Select <n1, n2> with n1 = n2") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::SYNONYM, "n2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1, n2});
    REQUIRE(results.empty());
  }

  SECTION("Select n1 with n1 = s1.stmt#") {
    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::ATTRIBUTE_STMT_NUM, "s1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(results.empty());
  }

  SECTION("Select n1 with n1 = c1.value") {
    pkb->addStmt(DesignEntity::STATEMENT, 1);
    pkb->insertAt(TableType::CONST_TABLE, "2");

    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::ATTRIBUTE_VALUE, "c1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(results.empty());
  }

  SECTION("Select n1 with n1 = 3") {
    pkb->addStmt(DesignEntity::STATEMENT, 1);
    pkb->addStmt(DesignEntity::STATEMENT, 2);

    WithClause withClause = {{ParamType::SYNONYM, "n1"},
                             {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {n1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 with s1.stmt# = s2.stmt#") {
    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s1"},
                             {ParamType::ATTRIBUTE_STMT_NUM, "s2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 with s1.stmt# = c1.value") {
    pkb->addStmt(DesignEntity::STATEMENT, 1);
    pkb->addStmt(DesignEntity::STATEMENT, 2);
    pkb->insertAt(TableType::CONST_TABLE, "3");

    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s1"},
                             {ParamType::ATTRIBUTE_VALUE, "c1"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select s1 with s1.stmt# = 2") {
    pkb->addStmt(DesignEntity::STATEMENT, 1);

    WithClause withClause = {{ParamType::ATTRIBUTE_STMT_NUM, "s1"},
                             {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {s1});
    REQUIRE(results.empty());
  }

  SECTION("Select c1 with c1.value = c2.value") {
    WithClause withClause = {{ParamType::ATTRIBUTE_VALUE, "c1"},
                             {ParamType::ATTRIBUTE_VALUE, "c2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {c1});
    REQUIRE(results.empty());
  }

  SECTION("Select c1 with c1.value = 2") {
    pkb->insertAt(TableType::CONST_TABLE, "1");

    WithClause withClause = {{ParamType::ATTRIBUTE_VALUE, "c1"},
                             {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::SYNONYMS, synonyms, {c1});
    REQUIRE(results.empty());
  }

  SECTION("Select BOOLEAN with 1 = 2") {
    WithClause withClause = {{ParamType::INTEGER_LITERAL, "1"},
                             {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    FinalQueryResults results = TestQueryUtil::EvaluateQuery(
        pkb, conditionClauses, SelectType::BOOLEAN, synonyms, {});
    REQUIRE(TestQueryUtil::GetUniqueSelectSingleQEResults(results) ==
            set<int>({FALSE_SELECT_BOOL_RESULT}));
  }
}
