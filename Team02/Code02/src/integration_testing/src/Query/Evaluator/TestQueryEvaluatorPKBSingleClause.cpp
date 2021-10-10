#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/QueryEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace query;

TEST_CASE("QueryEvaluator: Follows (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->setFollows(1, 2);
  pkb->setFollows(2, 3);
  pkb->addFollowsT(1, 2);
  pkb->addFollowsT(2, 3);
  pkb->addFollowsT(1, 3);
  pkb->addReadStmt(1);
  pkb->addReadStmt(2);

  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"r", DesignEntity::READ}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Follows(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Follows(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Follows(1, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2}));
  }

  SECTION("Select s1 such that Follows(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1}));
  }

  SECTION("Select s1 such that Follows(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2}));
  }

  SECTION("Select s1 such that Follows(_, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3}));
  }

  SECTION("Select s2 such that FollowsT(_, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2}));
  }

  SECTION("Select s2 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that Follows(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});
    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>{1, 2, 3, 4});
  }

  SECTION("Select r such that FollowsT(_, r)") {
    Synonym r = {DesignEntity::READ, "r"};
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::SYNONYM, "r"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});
    SelectClause select = {{r}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>{2});
  }
}

TEST_CASE("QueryEvaluator: Follows (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Follows(3, 4)") {
    pkb->setFollows(1, 2);
    pkb->setFollows(2, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Follows(3, _)") {
    pkb->setFollows(1, 2);
    pkb->setFollows(2, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Follows(3, s1)") {
    pkb->setFollows(1, 2);
    pkb->setFollows(2, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Follows(s1, 4)") {
    pkb->setFollows(1, 2);
    pkb->setFollows(2, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Follows(s1, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Follows(s1, s2)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }
}

TEST_CASE("QueryEvaluator: FollowsT (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addFollowsT(1, 2);
  pkb->addFollowsT(1, 3);
  pkb->addFollowsT(2, 3);
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that FollowsT(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select s1 such that FollowsT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select s2 such that FollowsT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    qe.evaluateQuery(synonyms, select);
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that FollowsT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select s1 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select s2 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that FollowsT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }
}

TEST_CASE("QueryEvaluator: FollowsT (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that FollowsT(3, 4)") {
    pkb->addFollowsT(1, 2);
    pkb->addFollowsT(1, 3);
    pkb->addFollowsT(2, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s1 such that FollowsT(3, _)") {
    pkb->addFollowsT(1, 2);
    pkb->addFollowsT(1, 3);
    pkb->addFollowsT(2, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s2 such that FollowsT(3, s2)") {
    pkb->addFollowsT(1, 2);
    pkb->addFollowsT(1, 3);
    pkb->addFollowsT(2, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s1 such that FollowsT(s1, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s1 such that FollowsT(s1, s2)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }
}

TEST_CASE("QueryEvaluator: Parent (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->setParent(1, 2);
  pkb->setParent(1, 3);
  pkb->setParent(3, 4);
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Parent(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(1, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that Parent(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1}));
  }

  SECTION("Select s1 such that Parent(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 3}));
  }

  SECTION("Select s1 such that Parent(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 3}));
  }

  SECTION("Select s2 such that Parent(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});
    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>{1, 2, 3, 4});
  }
}

TEST_CASE("QueryEvaluator: Parent (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Parent(2, 3)") {
    pkb->setParent(1, 2);
    pkb->setParent(1, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Parent(3, _)") {
    pkb->setParent(1, 2);
    pkb->setParent(1, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Parent(3, s1)") {
    pkb->setParent(1, 2);
    pkb->setParent(1, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Parent(s1, 4)") {
    pkb->setParent(1, 2);
    pkb->setParent(1, 3);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Parent(s1, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  SECTION("Select s1 such that Parent(s1, s2)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }
}

TEST_CASE("QueryEvaluator: ParentT (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addParentT(1, 2);
  pkb->addParentT(1, 3);
  pkb->addParentT(3, 4);
  pkb->addParentT(1, 4);
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that ParentT(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that ParentT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s2 such that ParentT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    qe.evaluateQuery(synonyms, select);
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that ParentT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 3}));
  }

  SECTION("Select s1 such that ParentT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 3}));
  }

  SECTION("Select s2 such that ParentT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that ParentT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3, 4}));
  }
}

TEST_CASE("QueryEvaluator: ParentT (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that ParentT(2, 3)") {
    pkb->addParentT(1, 2);
    pkb->addParentT(1, 3);
    pkb->addParentT(3, 4);
    pkb->addParentT(1, 4);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::INTEGER_LITERAL, "3"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s1 such that ParentT(2, _)") {
    pkb->addParentT(1, 2);
    pkb->addParentT(1, 3);
    pkb->addParentT(3, 4);
    pkb->addParentT(1, 4);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s2 such that ParentT(2, s2)") {
    pkb->addParentT(1, 2);
    pkb->addParentT(1, 3);
    pkb->addParentT(3, 4);
    pkb->addParentT(1, 4);
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "2"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s2}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s1 such that ParentT(s1, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("Select s1 such that ParentT(s1, s2)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s1}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }
}

TEST_CASE("QueryEvaluator: UsesS (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addUsesS(1, "x");
  pkb->addUsesS(1, "y");
  pkb->addUsesS(2, "z");
  QueryEvaluator qe(pkb);

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that UsesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("Select v such that UsesS(s, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select v such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select v such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: UsesS (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that UsesS(3, 'x')") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(3, 'x')") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(3, _)") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(3, _)") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, 'x')") {
    pkb->addUsesS(1, "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, 'x')") {
    pkb->addUsesS(1, "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: UsesP (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertProc("proc1");
  int proc2Idx = pkb->insertProc("proc2");
  int proc3Idx = pkb->insertProc("proc3");
  pkb->addUsesP("proc1", "x");
  pkb->addUsesP("proc1", "y");
  pkb->addUsesP("proc2", "z");
  QueryEvaluator qe(pkb);

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that UsesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that UsesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that UsesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that UsesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx}));
  }

  SECTION("Select p such that UsesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx}));
  }

  SECTION("Select v such that UsesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that UsesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that UsesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that UsesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: UsesP (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->insertProc("proc1");
  pkb->insertProc("proc2");
  pkb->insertProc("proc3");

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that UsesP('proc2', 'x')") {
    pkb->addUsesP("proc1", "x");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc2"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP('proc1', 'x')") {
    pkb->addUsesP("proc1", "y");
    pkb->addUsesP("proc2", "x");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP('proc1', _)") {
    pkb->addUsesP("proc2", "x");
    pkb->addUsesP("proc3", "x");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP('proc1', _)") {
    pkb->addUsesP("proc2", "x");
    pkb->addUsesP("proc3", "x");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP('proc1', v)") {
    pkb->addUsesP("proc2", "x");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP('proc1', v)") {
    pkb->addUsesP("proc2", "x");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP(p, 'x')") {
    pkb->addUsesP("proc1", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP(p, 'x')") {
    pkb->addUsesP("proc1", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP(p, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP(p, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that UsesP(p, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesP(p, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::USES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: ModifiesS (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addModifiesS(1, "x");
  pkb->addModifiesS(1, "y");
  pkb->addModifiesS(2, "z");
  QueryEvaluator qe(pkb);

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that ModifiesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that ModifiesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that ModifiesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that ModifiesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that ModifiesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select v such that ModifiesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: ModifiesS (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"v", DesignEntity::VARIABLE}};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s such that ModifiesS(3, 'x')") {
    pkb->addModifiesS(1, "x");
    pkb->addModifiesS(1, "y");
    pkb->addModifiesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(3, 'x')") {
    pkb->addModifiesS(1, "x");
    pkb->addModifiesS(1, "y");
    pkb->addModifiesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(3, _)") {
    pkb->addModifiesS(1, "x");
    pkb->addModifiesS(1, "y");
    pkb->addModifiesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(3, _)") {
    pkb->addModifiesS(1, "x");
    pkb->addModifiesS(1, "y");
    pkb->addModifiesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(s, 'x')") {
    pkb->addModifiesS(1, "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(s, 'x')") {
    pkb->addModifiesS(1, "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(s, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(s, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that ModifiesS(s, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{s}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesS(s, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: ModifiesP (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  int proc1Idx = pkb->insertProc("proc1");
  int proc2Idx = pkb->insertProc("proc2");
  int proc3Idx = pkb->insertProc("proc3");
  pkb->addModifiesP("proc1", "x");
  pkb->addModifiesP("proc1", "y");
  pkb->addModifiesP("proc2", "z");
  QueryEvaluator qe(pkb);

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that ModifiesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that ModifiesP('proc1', 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that ModifiesP('proc1', _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx, proc3Idx}));
  }

  SECTION("Select v such that ModifiesP('proc1', v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc1"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx}));
  }

  SECTION("Select p such that ModifiesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx}));
  }

  SECTION("Select v such that ModifiesP(p, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that ModifiesP(p, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select p such that ModifiesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({proc1Idx, proc2Idx}));
  }

  SECTION("Select v such that ModifiesP(p, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: ModifiesP (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  unordered_map<string, DesignEntity> synonyms = {
      {"p", DesignEntity::PROCEDURE}, {"v", DesignEntity::VARIABLE}};
  Synonym p = {DesignEntity::PROCEDURE, "p"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select p such that ModifiesP('proc3', 'x')") {
    pkb->addModifiesP("proc1", "x");
    pkb->addModifiesP("proc2", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP('proc3', 'x')") {
    pkb->addModifiesP("proc1", "x");
    pkb->addModifiesP("proc2", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP('proc3', _)") {
    pkb->addModifiesP("proc1", "x");
    pkb->addModifiesP("proc2", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP('proc3', _)") {
    pkb->addModifiesP("proc1", "x");
    pkb->addModifiesP("proc2", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP('proc3', v)") {
    pkb->addModifiesP("proc1", "x");
    pkb->addModifiesP("proc2", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP('proc3', v)") {
    pkb->addModifiesP("proc1", "x");
    pkb->addModifiesP("proc2", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::NAME_LITERAL, "proc3"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP(p, 'x')") {
    pkb->addModifiesP("proc1", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP(p, 'x')") {
    pkb->addModifiesP("proc1", "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP(p, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP(p, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select p such that ModifiesP(p, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{p}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that ModifiesP(p, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_P,
                                     {ParamType::SYNONYM, "p"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: Assignment Pattern (1 Clause) - Truthy Values") {
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

  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"a", DesignEntity::ASSIGN}, {"v", DesignEntity::VARIABLE}};
  Synonym a = {DesignEntity::ASSIGN, "a"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select a pattern a ('x', 'w')") {
    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("Select a pattern a ('x', _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("Select a pattern a ('x', _)") {
    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("Select a pattern a (_, 'w')") {
    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select a pattern a (_, _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select a pattern a (_, _)") {
    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select a pattern a (v, 'w')") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select a pattern a (v, _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select a pattern a (v, _)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v pattern a (v, 'w')") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx}));
  }

  SECTION("Select v pattern a (v, _'w'_)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx}));
  }

  SECTION("Select v pattern a (v, _)") {
    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: Assignment Pattern (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"a", DesignEntity::ASSIGN}, {"v", DesignEntity::VARIABLE}};
  Synonym a = {DesignEntity::ASSIGN, "a"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select a pattern a ('x', 'w')") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttFullExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a ('x', _'w'_)") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttSubExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a ('x', _)") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttFullExpr(1, "y", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::NAME_LITERAL, "x"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (_, 'w')") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttFullExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (_, _'w'_)") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttSubExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (_, _)") {
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::WILDCARD, "_"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (v, 'w')") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttFullExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (v, _'w'_)") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttSubExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select a pattern a (v, _)") {
    pkb->addAssignStmt(1);
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{a}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern a (v, 'w')") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttFullExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::EXACT, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern a (v, _'w'_)") {
    pkb->addAssignStmt(1);
    pkb->addAssignPttSubExpr(1, "x", "www");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "w"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern a (v, _)") {
    pkb->addAssignStmt(1);
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::ANY, "_"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: If Pattern (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addIfStmt(1);
  pkb->addIfStmt(2);
  pkb->addIfStmt(3);

  pkb->addIfPtt(1, "x");
  pkb->addIfPtt(1, "y");
  pkb->addIfPtt(2, "x");
  pkb->addIfPtt(3, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"ifs", DesignEntity::IF}, {"v", DesignEntity::VARIABLE}};
  Synonym ifs = {DesignEntity::IF, "ifs"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select ifs pattern ifs ('x', _, _)") {
    PatternClause patternClause = {ifs, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{ifs}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select ifs pattern ifs (_, _, _)") {
    PatternClause patternClause = {ifs, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{ifs}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select ifs pattern ifs (v, _, _)") {
    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{ifs}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v pattern ifs (v, _, _)") {
    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: If Pattern (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"ifs", DesignEntity::IF}, {"v", DesignEntity::VARIABLE}};
  Synonym ifs = {DesignEntity::IF, "ifs"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select ifs pattern ifs ('x', _, _)") {
    pkb->addIfStmt(1);
    pkb->addIfPtt(1, "y");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {ifs, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{ifs}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select ifs pattern ifs (_, _, _)") {
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {ifs, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{ifs}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select ifs pattern ifs (v, _, _)") {
    pkb->addIfStmt(1);  // e.g. if condition has const but no var
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{ifs}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern ifs (v, _, _)") {
    pkb->addIfStmt(1);
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {ifs, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: While Pattern (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addWhileStmt(1);
  pkb->addWhileStmt(2);
  pkb->addWhileStmt(3);
  pkb->addWhileStmt(4);

  pkb->addWhilePtt(1, "x");
  pkb->addWhilePtt(1, "y");
  pkb->addWhilePtt(2, "x");
  pkb->addWhilePtt(3, "z");

  int xVarIdx = 0;
  int yVarIdx = 1;
  int zVarIdx = 2;

  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"w", DesignEntity::WHILE}, {"v", DesignEntity::VARIABLE}};
  Synonym w = {DesignEntity::WHILE, "w"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select w pattern w ('x', _)") {
    PatternClause patternClause = {w, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{w}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select w pattern w (_, _)") {
    PatternClause patternClause = {w, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{w}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select w pattern w (v, _)") {
    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{w}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v pattern w (v, _)") {
    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }
}

TEST_CASE("QueryEvaluator: While Pattern (1 Clause) - Falsy Values") {
  PKB* pkb = new PKB();

  unordered_map<string, DesignEntity> synonyms = {
      {"w", DesignEntity::WHILE}, {"v", DesignEntity::VARIABLE}};
  Synonym w = {DesignEntity::WHILE, "w"};
  Synonym v = {DesignEntity::VARIABLE, "v"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select w pattern w ('x', _, _)") {
    pkb->addWhileStmt(1);
    pkb->addWhilePtt(1, "y");
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {w, {ParamType::NAME_LITERAL, "x"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{w}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select w pattern w (_, _, _)") {
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {w, {ParamType::WILDCARD, "_"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{w}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select w pattern w (v, _, _)") {
    pkb->addWhileStmt(1);  // e.g. while condition has const but no var
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{w}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v pattern w (v, _, _)") {
    pkb->addWhileStmt(1);
    QueryEvaluator qe(pkb);

    PatternClause patternClause = {w, {ParamType::SYNONYM, "v"}, {}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {{v}, SelectType::SYNONYMS, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}
