#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/QueryEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace query;

TEST_CASE("Select all design entities") {
  SECTION("stmt s; Select s") {
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

  SECTION("prog_line n; Select n") {
    PKB* pkb = new PKB();
    pkb->addStmt(1);
    pkb->addStmt(2);
    pkb->addStmt(3);

    QueryEvaluator qe(pkb);
    unordered_map<string, DesignEntity> synonyms = {
        {"n", DesignEntity::PROG_LINE}};

    Synonym n = {DesignEntity::PROG_LINE, "n"};
    vector<ConditionClause> conditionClauses = {};

    SelectClause select = {n, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({1, 2, 3}));
  }

  SECTION("procedure p; Select p") {
    PKB* pkb = new PKB();
    int aIdx = pkb->insertProc("procA");
    int bIdx = pkb->insertProc("procB");
    int cIdx = pkb->insertProc("procC");

    QueryEvaluator qe(pkb);
    unordered_map<string, DesignEntity> synonyms = {
        {"p", DesignEntity::PROCEDURE}};

    Synonym p = {DesignEntity::PROCEDURE, "p"};
    vector<ConditionClause> conditionClauses = {};

    SelectClause select = {p, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({aIdx, bIdx, cIdx}));
  }

  SECTION("variable v; Select v") {
    PKB* pkb = new PKB();
    int xIdx = pkb->insertVar("x");
    int yIdx = pkb->insertVar("y");
    int zIdx = pkb->insertVar("z");

    QueryEvaluator qe(pkb);
    unordered_map<string, DesignEntity> synonyms = {
        {"v", DesignEntity::VARIABLE}};

    Synonym v = {DesignEntity::VARIABLE, "v"};
    vector<ConditionClause> conditionClauses = {};

    SelectClause select = {v, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({xIdx, yIdx, zIdx}));
  }

  SECTION("constant c; Select c") {
    PKB* pkb = new PKB();
    pkb->insertConst("1");
    pkb->insertConst("3");
    pkb->insertConst("5");

    QueryEvaluator qe(pkb);
    unordered_map<string, DesignEntity> synonyms = {
        {"c", DesignEntity::CONSTANT}};

    Synonym c = {DesignEntity::CONSTANT, "c"};
    vector<ConditionClause> conditionClauses = {};

    SelectClause select = {c, conditionClauses};
    unordered_set<int> result = qe.evaluateQuery(synonyms, select);
    REQUIRE(result == unordered_set<int>({0, 1, 2}));
  }
}

TEST_CASE("QueryEvaluator: Different Design Entities") {
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
  pkb->addAssignPttFullExpr(6, "x", "1");
  pkb->addAssignPttSubExpr(6, "x", "1");
  pkb->setParent(4, 5);
  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"n", DesignEntity::PROG_LINE},
      {"rd", DesignEntity::READ},     {"pr", DesignEntity::PRINT},
      {"cll", DesignEntity::CALL},    {"w", DesignEntity::WHILE},
      {"ifs", DesignEntity::IF},      {"a", DesignEntity::ASSIGN}};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym n = {DesignEntity::PROG_LINE, "n"};
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

  SECTION("prog_line n; Select n such that Follows(n, _)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "n"},
                                     {ParamType::WILDCARD, "_"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {n, conditionClauses};
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

  // Test a combo with different design entities and multiple clauses
  SECTION("Select a such that Follows(rd, a) pattern a (v, _'1'_)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "rd"},
                                     {ParamType::SYNONYM, "a"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    PatternClause patternClause = {
        a, {ParamType::SYNONYM, "v"}, {MatchType::SUB_EXPRESSION, "1"}};
    conditionClauses.push_back(
        {{}, patternClause, ConditionClauseType::PATTERN});

    SelectClause select = {a, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results == unordered_set<int>({}));
  }
}

TEST_CASE("Test clauses with same synonym for both params") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addAssignStmt(3);
  pkb->addAssignStmt(4);
  pkb->setFollows(1, 2);
  pkb->setFollows(3, 4);
  pkb->addFollowsT(1, 2);
  pkb->addFollowsT(3, 4);
  pkb->setParent(2, 3);
  pkb->addParentT(2, 3);

  QueryEvaluator qe(pkb);

  unordered_map<string, DesignEntity> synonyms = {
      {"s", DesignEntity::STATEMENT}, {"a", DesignEntity::ASSIGN}};
  Synonym s = {DesignEntity::STATEMENT, "s"};
  Synonym a = {DesignEntity::ASSIGN, "a"};
  vector<ConditionClause> conditionClauses = {};

  SECTION("stmt s; Select s such that Follows(s, s)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "s"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("stmt s; Select s such that FollowsT(s, s)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS_T,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "s"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("assign a; Select a such that Follows(a, a)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "a"},
                                     {ParamType::SYNONYM, "a"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {a, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("stmt s; Select s such that Follows(s, s)") {
    SuchThatClause suchThatClause = {RelationshipType::FOLLOWS,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "s"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("stmt s; Select s such that Parent(s, s)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "s"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }

  SECTION("stmt s; Select s such that ParentT(s, s)") {
    SuchThatClause suchThatClause = {RelationshipType::PARENT_T,
                                     {ParamType::SYNONYM, "s"},
                                     {ParamType::SYNONYM, "s"}};
    conditionClauses.push_back(
        {suchThatClause, {}, ConditionClauseType::SUCH_THAT});

    SelectClause select = {s, conditionClauses};
    unordered_set<int> results = qe.evaluateQuery(synonyms, select);
    REQUIRE(results.empty());
  }
}
