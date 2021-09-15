#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/QueryEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace query;

TEST_CASE("Select s") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);

  QueryEvaluator qe(pkb);
  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}};

  Synonym s = {DesignEntity::STATEMENT, "s"};
  vector<ConditionClause> conditionClauses = {};

  SelectClause select = {s, conditionClauses};
  unordered_set<int> result = qe.evaluateQuery(synonyms, select);
  REQUIRE(result == unordered_set<int>({1, 2, 3}));
}

TEST_CASE("QueryEvaluator: Follows (1 Clause) - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->setFollows(1, 2);
  pkb->setFollows(2, 3);
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("Select s1 such that Follows(1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Follows(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Follows(1, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2}));
  }

  SECTION("Select s1 such that Follows(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1}));
  }

  SECTION("Select s1 such that Follows(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2}));
  }

  SECTION("Select s1 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2}));
  }

  SECTION("Select s2 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that Follows(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});
    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>{1, 2, 3, 4});
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select s1 such that FollowsT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select s2 such that FollowsT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select s1 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select s2 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that FollowsT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s2, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(1, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that Parent(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1}));
  }

  SECTION("Select s1 such that Parent(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 3}));
  }

  SECTION("Select s1 such that Parent(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 3}));
  }

  SECTION("Select s2 such that Parent(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that Parent(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});
    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that ParentT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s2 such that ParentT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 3}));
  }

  SECTION("Select s1 such that ParentT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 3}));
  }

  SECTION("Select s2 such that ParentT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3, 4}));
  }

  SECTION("Select s1 such that ParentT(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s2, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s2, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s1, conditionClauses};
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

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("Select v such that UsesS(s, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select v such that UsesS(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select v such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::USES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
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

    SelectClause select = {s, conditionClauses};
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

    SelectClause select = {v, conditionClauses};
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

    SelectClause select = {s, conditionClauses};
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

    SelectClause select = {v, conditionClauses};
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

    SelectClause select = {s, conditionClauses};
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

    SelectClause select = {v, conditionClauses};
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

    SelectClause select = {s, conditionClauses};
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

    SelectClause select = {v, conditionClauses};
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

    SelectClause select = {s, conditionClauses};
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

    SelectClause select = {v, conditionClauses};
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

  SECTION("Select s such that UsesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, 'x')") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select v such that UsesS(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({xVarIdx, yVarIdx, zVarIdx}));
  }

  SECTION("Select s such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select v such that UsesS(s, v)") {
    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
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

  SECTION("Select s such that UsesS(3, 'x')") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(3, 'x')") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(3, _)") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(3, _)") {
    pkb->addUsesS(1, "x");
    pkb->addUsesS(1, "y");
    pkb->addUsesS(2, "z");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::INTEGER_LITERAL, "3"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, 'x')") {
    pkb->addUsesS(1, "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, 'x')") {
    pkb->addUsesS(1, "y");
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::NAME_LITERAL, "x"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, _)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select s such that UsesS(s, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("Select v such that UsesS(s, v)") {
    QueryEvaluator qe(pkb);

    SuchThatClause suchThatClause = {RelationshipType::MODIFIES_S,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "v"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {v, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}

TEST_CASE("QueryEvaluator: 2 Clauses - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->setFollows(1, 2);
  pkb->setFollows(2, 3);
  pkb->addFollowsT(1, 2);
  pkb->addFollowsT(1, 3);
  pkb->addFollowsT(2, 3);
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

  SECTION("2 Follows: Bool + Synonym") {
    // Select s1 such that Follows(1, 2) and Follows(s1, 2)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::INTEGER_LITERAL, "1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION(
      "2 Follows: Synonym Clause + Synonym Clause: Filter Algo - One Common "
      "Synonym") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, 2)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("2 Follows: Synonym + Synonym: Filter Algo - Both Common Synonyms") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, s2)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};

    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION(
      "2 Follows: Synonym + Synonym: Inner Join Algo - One Common Synonym") {
    // Select s1 such that Follows(s1, s2) and Follows(s2, s3)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s2"},
                                      {ParamType::SYNONYM, "s3"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION(
      "2 Follows: Synonym + Synonym: Inner Join Algo - One Common Synonym w/ "
      "Wildcard") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, _)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION(
      "2 Follows: Synonym + Synonym: Cross Product Algo - No Common Synonyms") {
    // Select s1 such that Follows(s1, s2) and Follows(s3, s4)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s3"},
                                      {ParamType::SYNONYM, "s4"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION(
      "2 Follows: Synonym + Synonym: Cross Product Algo - No Common Synonyms "
      "w/ Wildcard") {
    // Select s1 such that Follows(s1, _) and Follows(s2, _)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s2"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }
}

TEST_CASE("QueryEvaluator: 2 Clauses - Falsy Values") {
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
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("2 Follows: False Bool Clause + False Synonym Clause") {
    // Select s1 such that Follows(3, 4) and Follows(s1, 1)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::INTEGER_LITERAL, "3"},
                                      {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("2 Follows: True Synonym Clause + False Synonym Clause") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, 5)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "5"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("2 Follows: False Synonym Clause + False Synonym Clause") {
    // Select s1 such that Follows(s1, 5) and Follows(s1, 6)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "5"}};
    conditionClauses.push_back(
        {suchThatClause1, {}, ConditionClauseType::SUCH_THAT});

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "6"}};
    conditionClauses.push_back(
        {suchThatClause2, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }
}

TEST_CASE("QueryEvaluator: 1 Clause, Different Design Entities") {
  PKB* pkb = new PKB();
  for (int i = 1; i < 8; i++) {
    pkb->addStmt(i);
    if (i != 7) {
      pkb->setFollows(i, i + 1);
    }
  }
  pkb->addReadStmt(1);
  pkb->addPrintStmt(2);
  pkb->addCallStmt(3);
  pkb->addWhileStmt(4);
  pkb->addIfStmt(5);
  pkb->addAssignStmt(6);
  pkb->addAssignStmt(7);
  pkb->setParent(4, 5);
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"rd", DesignEntity::READ},
      {"pr", DesignEntity::PRINT},    {"cll", DesignEntity::CALL},
      {"w", DesignEntity::WHILE},     {"ifs", DesignEntity::IF},
      {"a", DesignEntity::ASSIGN}};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym rd = {DesignEntity::READ, "rd"};
  Synonym pr = {DesignEntity::PRINT, "pr"};
  Synonym cll = {DesignEntity::CALL, "cll"};
  Synonym w = {DesignEntity::WHILE, "w"};
  Synonym ifs = {DesignEntity::IF, "ifs"};
  Synonym a = {DesignEntity::ASSIGN, "a"};
  vector<ConditionClause> conditionClauses = {};

  // CHECK SELECTION OF INDIVIDUAL STMT TYPES
  SECTION("stmt s; Select s such that Follows(s, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3, 4, 5, 6}));
  }

  SECTION("read rd; Select rd such that Follows(rd, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "rd"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {rd, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("print pr; Select pr such that Follows(pr, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "pr"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {pr, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2}));
  }

  SECTION("call cll; Select cll such that Follows(cll, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "cll"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {cll, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({3}));
  }

  SECTION("while w; Select w such that Follows(w, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "w"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {w, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({4}));
  }

  SECTION("if ifs; Select ifs such that Follows(ifs, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "ifs"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {ifs, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({5}));
  }

  SECTION("assign a; Select a such that Follows(a, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "a"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {a, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({6}));
  }

  // CHECK COMBINATIONS OF PARAM STMT TYPES
  SECTION("read rd; Select rd such that Follows(rd, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "rd"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {rd, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("print pr; Select cll such that Follows(1, pr)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "pr"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {pr, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2}));
  }

  SECTION("read rd; print pr; Select rd such that Follows(rd, pr)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "rd"},
                                     {ParamType::SYNONYM, "pr"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {rd, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("assign a; Select a such that Follows(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {a, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({6, 7}));
  }

  SECTION("while w; Select w such that Parent(w, 5)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "w"},
                                     {ParamType::INTEGER_LITERAL, "5"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {w, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({4}));
  }

  SECTION("if ifs; Select ifs such that Parent(4, ifs)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::INTEGER_LITERAL, "4"},
                                     {ParamType::SYNONYM, "ifs"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {ifs, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({5}));
  }

  SECTION("while w; if ifs; Select w such that Parent(w, ifs)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "w"},
                                     {ParamType::SYNONYM, "ifs"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {w, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({4}));
  }

  SECTION("call cll; Select cll such that Parent(_, _)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::WILDCARD, "_"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {cll, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({3}));
  }
}
