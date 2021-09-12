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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Follows(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4}));
  }

  SECTION("Select s1 such that Follows(1, s1)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s1"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2}));
  }

  SECTION("Select s1 such that Follows(s1, 2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1}));
  }

  SECTION("Select s1 such that Follows(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2}));
  }

  SECTION("Select s1 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2}));
  }

  SECTION("Select s2 such that Follows(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s2, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({2, 3}));
  }

  // SECTION("Select s1 such that Follows(_, _)") {
  //   suchThatClauses.push_back({RelationshipType::FOLLOWS,
  //                              {ParamType::WILDCARD, "_"},
  //                              {ParamType::WILDCARD, "_"}});
  //   SelectClause select = {s, suchThatClauses, patternClauses};
  //   unordered_set<int> result = qe.evaluateQuery(synonyms, select);
  //   REQUIRE(result == unordered_set<int>{"1", "2"});
  //   todo after pkb implements necessary methods
  // }
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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({}));
  }

  // TODO(cs): Uncomment once PKB implementation done
  // SECTION("Select s1 such that Follows(s1, _)") {
  //   QueryEvaluator qe(pkb);

  //   suchThatClauses.push_back({RelationshipType::FOLLOWS,
  //                              {ParamType::SYNONYM, "s1"},
  //                              {ParamType::WILDCARD, "_"}});
  //   SelectClause select = {s1, suchThatClauses, patternClauses};
  //   unordered_set<int> result = qe.evaluateQuery(synonyms, select);
  //   REQUIRE(result == unordered_set<int>({}));
  // }

  // SECTION("Select s1 such that Follows(s1, s2)") {
  //   QueryEvaluator qe(pkb);

  //   suchThatClauses.push_back({RelationshipType::FOLLOWS,
  //                              {ParamType::SYNONYM, "s1"},
  //                              {ParamType::SYNONYM, "s2"}});
  //   SelectClause select = {s1, suchThatClauses, patternClauses};
  //   unordered_set<int> result = qe.evaluateQuery(synonyms, select);
  //   REQUIRE(result == unordered_set<int>({}));
  // }
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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select s1 such that FollowsT(1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2, 3}));
  }

  SECTION("Select s2 such that FollowsT(1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::INTEGER_LITERAL, "1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s2, conditionClauses};
    qe.evaluateQuery(synonyms, select);
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3}));
  }

  SECTION("Select s1 such that FollowsT(s1, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select s1 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1, 2}));
  }

  SECTION("Select s2 such that FollowsT(s1, s2)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s1"},
                                     {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s2, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({2, 3}));
  }

  // SECTION("Select s1 such that FollowsT(_, _)") {
  //   suchThatClauses.push_back({RelationshipType::FOLLOWS_T,
  //                              {ParamType::WILDCARD, "_"},
  //                              {ParamType::WILDCARD, "_"}});
  //   SelectClause select = {s, suchThatClauses, patternClauses};
  //   qe.evaluateQuery(synonyms, select);
  //   todo after pkb implements necessary methods
  // }
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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s2, conditionClauses};
    qe.evaluateQuery(synonyms, select);
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  // TODO(cs): Uncomment once PKB implementation done
  // SECTION("Select s1 such that FollowsT(s1, _)") {
  //   QueryEvaluator qe(pkb);

  //   suchThatClauses.push_back({RelationshipType::FOLLOWS_T,
  //                              {ParamType::SYNONYM, "s1"},
  //                              {ParamType::WILDCARD, "_"}});
  //   SelectClause select = {s1, suchThatClauses, patternClauses};
  //   unordered_set<int> results = qe.evaluateQuery(synonyms, select);
  //   REQUIRE(results == unordered_set<int>({}));
  // }

  // SECTION("Select s1 such that FollowsT(s1, s2)") {
  //   QueryEvaluator qe(pkb);

  //   suchThatClauses.push_back({RelationshipType::FOLLOWS_T,
  //                              {ParamType::SYNONYM, "s1"},
  //                              {ParamType::SYNONYM, "s2"}});
  //   SelectClause select = {s1, suchThatClauses, patternClauses};
  //   unordered_set<int> results = qe.evaluateQuery(synonyms, select);
  //   REQUIRE(results == unordered_set<int>({}));
  // }
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
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                               {ParamType::SYNONYM, "s1"},
                               {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({1}));
  }

  SECTION("2 Follows: Synonym + Synonym: Filter Algo - Both Common Synonyms") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, s2)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
     {ParamType::SYNONYM, "s1"},
     {ParamType::SYNONYM, "s2"}};

     conditionClauses.push_back({
       suchThatClause2,
       {},
       ConditionClauseType::SUCH_THAT
     });

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
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s2"},
                                      {ParamType::SYNONYM, "s3"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s3"},
                                      {ParamType::SYNONYM, "s4"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s2"},
                                      {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

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
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "2"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("2 Follows: False Bool Clause + False Synonym Clause") {
    // Select s1 such that Follows(3, 4) and Follows(s1, 1)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::INTEGER_LITERAL, "3"},
                                      {ParamType::INTEGER_LITERAL, "4"}};
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "1"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("2 Follows: True Synonym Clause + False Synonym Clause") {
    // Select s1 such that Follows(s1, s2) and Follows(s1, 5)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "s2"}};
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "5"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }

  SECTION("2 Follows: False Synonym Clause + False Synonym Clause") {
    // Select s1 such that Follows(s1, 5) and Follows(s1, 6)
    SuchThatClause suchThatClause1 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::SYNONYM, "5"}};
    conditionClauses.push_back({
      suchThatClause1,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SuchThatClause suchThatClause2 = {RelationshipType::FOLLOWS,
                                      {ParamType::SYNONYM, "s1"},
                                      {ParamType::INTEGER_LITERAL, "6"}};
    conditionClauses.push_back({
      suchThatClause2,
      {},
      ConditionClauseType::SUCH_THAT
    });

    SelectClause select = {s1, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }
}
