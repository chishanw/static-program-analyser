#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/WithEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace std;
using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("WithEvaluator: Name Attributes") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addStmt(5);
  pkb->addCalls(3, "A", "D");
  pkb->addCalls(4, "B", "D");
  pkb->addCalls(5, "C", "A");
  pkb->addReadStmt(1);
  pkb->addReadStmt(2);
  pkb->addModifiesS(1, "x");
  pkb->addModifiesS(2, "y");
  int procAIdx = pkb->insertProc("A");
  int procBIdx = pkb->insertProc("B");
  int procCIdx = pkb->insertProc("C");
  int callAIdx = 3;
  int callBIdx = 4;
  int callCIdx = 5;
  int xVarIdx = pkb->insertVar("x");
  int yVarIdx = pkb->insertVar("y");
  int zVarIdx = pkb->insertVar("z");
  int rdXIdx = 1;
  int rdYIdx = 2;

  WithEvaluator we(pkb);
  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE}, {"p2", DesignEntity::PROCEDURE},
      {"c1", DesignEntity::CALL},      {"c2", DesignEntity::CALL},
      {"v1", DesignEntity::VARIABLE},  {"v2", DesignEntity::VARIABLE},
      {"rd1", DesignEntity::READ},     {"rd2", DesignEntity::READ}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1"};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2"};
  Synonym c1 = {DesignEntity::PROCEDURE, "c1"};
  Synonym c2 = {DesignEntity::PROCEDURE, "c2"};
  Synonym v1 = {DesignEntity::VARIABLE, "v1"};
  Synonym v2 = {DesignEntity::VARIABLE, "v2"};
  Synonym rd1 = {DesignEntity::READ, "rd1"};
  Synonym rd2 = {DesignEntity::READ, "rd2"};

  SECTION("with p1.procName = p2.procName") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::ATTRIBUTE_PROC_NAME, "p2"};
    vector<QueryResult> currentResults = {{{"p1", procAIdx}, {"p2", procAIdx}},
                                          {{"p1", procBIdx}, {"p2", procBIdx}},
                                          {{"p1", procBIdx}, {"p2", procCIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"p1", procAIdx}, {"p2", procAIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"p1", procBIdx}, {"p2", procBIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"p1", procBIdx}, {"p2", procCIdx}})));
  }

  SECTION("with c1.procName = c2.procName") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    Param right = {ParamType::ATTRIBUTE_PROC_NAME, "c2"};
    vector<QueryResult> currentResults = {{{"c1", callAIdx}, {"c2", callAIdx}},
                                          {{"c1", callAIdx}, {"c2", callBIdx}},
                                          {{"c1", callBIdx}, {"c2", callCIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"c1", callAIdx}, {"c2", callAIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"c1", callAIdx}, {"c2", callBIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"c1", callBIdx}, {"c2", callCIdx}})));
  }

  SECTION("with p1.procName = c1.procName") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    vector<QueryResult> currentResults = {{{"c1", callCIdx}, {"p1", procAIdx}},
                                          {{"c1", callCIdx}, {"p1", procBIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"c1", callCIdx}, {"p1", procAIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"c1", callCIdx}, {"p1", procBIdx}})));
  }

  SECTION("with p1.procName = v1.varName") {
    int aVarIdx = pkb->insertVar("A");
    int bVarIdx = pkb->insertVar("B");
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    vector<QueryResult> currentResults = {{{"p1", procAIdx}, {"v1", aVarIdx}},
                                          {{"p1", procAIdx}, {"v1", xVarIdx}},
                                          {{"p1", procBIdx}, {"v1", bVarIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"p1", procAIdx}, {"v1", aVarIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"p1", procBIdx}, {"v1", bVarIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"p1", procAIdx}, {"v1", xVarIdx}})));
  }

  SECTION("with c1.procName = v1.varName") {
    int aVarIdx = pkb->insertVar("A");
    int dVarIdx = pkb->insertVar("D");
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    vector<QueryResult> currentResults = {{{"c1", callCIdx}, {"v1", aVarIdx}},
                                          {{"c1", callAIdx}, {"v1", dVarIdx}},
                                          {{"c1", callBIdx}, {"v1", aVarIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"c1", callCIdx}, {"v1", aVarIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"c1", callAIdx}, {"v1", dVarIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"c1", callBIdx}, {"v1", aVarIdx}})));
  }

  SECTION("with p1.procName = 'A'") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::NAME_LITERAL, "A"};
    vector<QueryResult> currentResults = {
        {{"p1", procAIdx}, {"p2", procBIdx}, {"v1", xVarIdx}},
        {{"p1", procBIdx}, {"p2", procAIdx}, {"v1", yVarIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult(
                     {{"p1", procAIdx}, {"p2", procBIdx}, {"v1", xVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult(
                     {{"p1", procBIdx}, {"p2", procAIdx}, {"v1", yVarIdx}})));
  }

  SECTION("with c1.procName = 'D'") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    Param right = {ParamType::NAME_LITERAL, "D"};
    vector<QueryResult> currentResults = {{{"c1", callAIdx}},
                                          {{"c1", callBIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"c1", callAIdx}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"c1", callBIdx}})));
  }

  SECTION("with v1.varName = v2.varName") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v2"};
    vector<QueryResult> currentResults = {{{"v1", xVarIdx}, {"v2", xVarIdx}},
                                          {{"v1", yVarIdx}, {"v2", yVarIdx}},
                                          {{"v1", yVarIdx}, {"v2", zVarIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"v1", xVarIdx}, {"v2", xVarIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"v1", yVarIdx}, {"v2", yVarIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"v1", yVarIdx}, {"v2", zVarIdx}})));
  }

  SECTION("with rd1.varName = rd2.varName") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "rd1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "rd2"};
    vector<QueryResult> currentResults = {{{"rd1", rdXIdx}, {"rd2", rdXIdx}},
                                          {{"rd1", rdXIdx}, {"rd2", rdYIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"rd1", rdXIdx}, {"rd2", rdXIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"rd1", rdXIdx}, {"rd2", rdYIdx}})));
  }

  SECTION("with rd1.varName = v1.varName") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "rd1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    vector<QueryResult> currentResults = {{{"rd1", rdXIdx}, {"v1", xVarIdx}},
                                          {{"rd1", rdYIdx}, {"v1", yVarIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"rd1", rdXIdx}, {"v1", xVarIdx}})));
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"rd1", rdYIdx}, {"v1", yVarIdx}})));
  }

  SECTION("with v1.varName = 'x'") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    vector<QueryResult> currentResults = {
        {{"v1", xVarIdx}}, {{"v1", yVarIdx}}, {{"v1", yVarIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"v1", xVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"v1", yVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"v1", yVarIdx}})));
  }

  SECTION("with rd1.varName = 'x'") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "rd1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    vector<QueryResult> currentResults = {{{"rd1", rdXIdx}}, {{"rd1", rdYIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"rd1", rdXIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"rd1", rdYIdx}})));
  }

  SECTION("with 'xyz' = 'xyz'") {
    Param left = {ParamType::NAME_LITERAL, "xyz"};
    Param right = {ParamType::NAME_LITERAL, "xyz"};
    vector<QueryResult> currentResults = {{{"v1", xVarIdx}}, {{"v1", yVarIdx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"v1", xVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"v1", yVarIdx}})));
  }
}

TEST_CASE("WithEvaluator: Integer Attributes") {
  PKB* pkb = new PKB();
  pkb->addStmt(1);
  pkb->addStmt(2);
  pkb->addStmt(3);
  pkb->addStmt(4);
  pkb->addAssignStmt(1);
  pkb->addIfStmt(2);
  int const1Idx = pkb->insertConst("1");
  int const2Idx = pkb->insertConst("2");

  WithEvaluator we(pkb);
  unordered_map<string, DesignEntity> synonyms = {
      {"c1", DesignEntity::CONSTANT},  {"c2", DesignEntity::CONSTANT},
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
      {"a1", DesignEntity::ASSIGN},    {"a2", DesignEntity::ASSIGN},
      {"ifs", DesignEntity::IF},       {"n1", DesignEntity::PROG_LINE},
      {"n2", DesignEntity::PROG_LINE}};
  Synonym c1 = {DesignEntity::CONSTANT, "c1"};
  Synonym c2 = {DesignEntity::CONSTANT, "c2"};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  Synonym a1 = {DesignEntity::ASSIGN, "a1"};
  Synonym a2 = {DesignEntity::ASSIGN, "a2"};
  Synonym ifs = {DesignEntity::IF, "ifs"};
  Synonym n1 = {DesignEntity::PROG_LINE, "n1"};
  Synonym n2 = {DesignEntity::PROG_LINE, "n2"};

  SECTION("with n1 = n2") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::SYNONYM, "n2"};
    vector<QueryResult> currentResults = {{{"n1", 1}, {"n2", 1}},
                                          {{"n1", 2}, {"n2", 3}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"n1", 1}, {"n2", 1}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"n1", 2}, {"n2", 3}})));
  }

  SECTION("with n1 = s1.stmt#") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    vector<QueryResult> currentResults = {{{"n1", 1}, {"s1", 1}},
                                          {{"n1", 2}, {"s1", 3}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"n1", 1}, {"s1", 1}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"n1", 2}, {"s1", 3}})));
  }

  SECTION("with n1 = c1.value") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::ATTRIBUTE_VALUE, "c1"};
    vector<QueryResult> currentResults = {{{"n1", 1}, {"c1", const1Idx}},
                                          {{"n1", 2}, {"c1", const2Idx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"n1", 1}, {"c1", const1Idx}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"n1", 2}, {"c1", const2Idx}})));
  }

  SECTION("with n1 = 3") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    vector<QueryResult> currentResults = {
        {{"n1", 1}}, {{"n1", 2}}, {{"n1", 3}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(QueryResult({{"n1", 3}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(QueryResult({{"n1", 1}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(QueryResult({{"n1", 2}})));
  }

  SECTION("with c1.value = c2.value") {
    Param left = {ParamType::ATTRIBUTE_VALUE, "c1"};
    Param right = {ParamType::ATTRIBUTE_VALUE, "c2"};
    vector<QueryResult> currentResults = {
        {{"c1", const1Idx}, {"c2", const1Idx}},
        {{"c1", const1Idx}, {"c2", const2Idx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(QueryResult({{"c1", const1Idx}, {"c2", const1Idx}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(QueryResult({{"c1", const1Idx}, {"c2", const2Idx}})));
  }

  SECTION("with c1.value = s1.value") {
    Param left = {ParamType::ATTRIBUTE_VALUE, "c1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    vector<QueryResult> currentResults = {{{"c1", const1Idx}, {"s1", 1}},
                                          {{"c1", const2Idx}, {"s1", 2}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"c1", const1Idx}, {"s1", 1}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"c1", const2Idx}, {"s1", 2}})));
  }

  SECTION("with s1.value = s2.value") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s2"};
    vector<QueryResult> currentResults = {{{"s1", 1}, {"s2", 1}},
                                          {{"s1", 2}, {"s2", 3}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"s1", 1}, {"s2", 1}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"s1", 2}, {"s2", 3}})));
  }

  SECTION("with a1.value = a2.value") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "a1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "a2"};
    vector<QueryResult> currentResults = {{{"a1", 1}, {"a2", 1}},
                                          {{"a1", 2}, {"a2", 3}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"a1", 1}, {"a2", 1}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"a1", 2}, {"a2", 3}})));
  }

  SECTION("with a1.value = s1.value") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "a1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    vector<QueryResult> currentResults = {{{"a1", 1}, {"s1", 1}},
                                          {{"a1", 2}, {"s1", 2}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"a1", 1}, {"s1", 1}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"a1", 2}, {"s1", 2}})));
  }

  SECTION("with c1.value = 1") {
    Param left = {ParamType::ATTRIBUTE_VALUE, "c1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    vector<QueryResult> currentResults = {{{"c1", const1Idx}},
                                          {{"c1", const2Idx}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(QueryResult({{"c1", const1Idx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(QueryResult({{"c1", const2Idx}})));
  }

  SECTION("with s1.value = 2") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    vector<QueryResult> currentResults = {{{"s1", 1}}, {{"s1", 2}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(QueryResult({{"s1", 2}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(QueryResult({{"s1", 1}})));
  }

  SECTION("with 1 = 1") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    vector<QueryResult> currentResults = {{{"s1", 1}}, {{"s1", 2}}};
    pair<bool, vector<QueryResult>> results =
        we.evaluateAttributes(left, right, synonyms, currentResults);
    vector<QueryResult> newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(QueryResult({{"s1", 1}})));
    REQUIRE_THAT(newQueryResults, VectorContains(QueryResult({{"s1", 2}})));
  }
}
