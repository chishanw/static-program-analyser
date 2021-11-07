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
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::STATEMENT, 5);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "A",
             TableType::PROC_TABLE, "D");
  pkb->addRs(RelationshipType::CALLS_S, 3, TableType::PROC_TABLE, "D");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "B",
             TableType::PROC_TABLE, "D");
  pkb->addRs(RelationshipType::CALLS_S, 4, TableType::PROC_TABLE, "D");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "C",
             TableType::PROC_TABLE, "A");
  pkb->addRs(RelationshipType::CALLS_S, 5, TableType::PROC_TABLE, "A");
  pkb->addStmt(DesignEntity::READ, 1);
  pkb->addStmt(DesignEntity::READ, 2);
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "y");
  int procAIdx = pkb->insertAt(TableType::PROC_TABLE, "A");
  int procBIdx = pkb->insertAt(TableType::PROC_TABLE, "B");
  int procCIdx = pkb->insertAt(TableType::PROC_TABLE, "C");
  int callAIdx = 3;
  int callBIdx = 4;
  int callCIdx = 5;
  int xVarIdx = pkb->insertAt(TableType::VAR_TABLE, "x");
  int yVarIdx = pkb->insertAt(TableType::VAR_TABLE, "y");
  int zVarIdx = pkb->insertAt(TableType::VAR_TABLE, "z");
  int rdXIdx = 1;
  int rdYIdx = 2;

  WithEvaluator we(pkb);
  unordered_map<string, DesignEntity> synonyms = {
      {"p1", DesignEntity::PROCEDURE}, {"p2", DesignEntity::PROCEDURE},
      {"c1", DesignEntity::CALL},      {"c2", DesignEntity::CALL},
      {"v1", DesignEntity::VARIABLE},  {"v2", DesignEntity::VARIABLE},
      {"rd1", DesignEntity::READ},     {"rd2", DesignEntity::READ}};
  Synonym p1 = {DesignEntity::PROCEDURE, "p1", false, {}};
  Synonym p2 = {DesignEntity::PROCEDURE, "p2", false, {}};
  Synonym c1 = {DesignEntity::PROCEDURE, "c1", false, {}};
  Synonym c2 = {DesignEntity::PROCEDURE, "c2", false, {}};
  Synonym v1 = {DesignEntity::VARIABLE, "v1", false, {}};
  Synonym v2 = {DesignEntity::VARIABLE, "v2", false, {}};
  Synonym rd1 = {DesignEntity::READ, "rd1", false, {}};
  Synonym rd2 = {DesignEntity::READ, "rd2", false, {}};

  SECTION("with p1.procName = p2.procName") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::ATTRIBUTE_PROC_NAME, "p2"};
    vector<IntermediateQueryResult> currentResults = {
        {{"p1", procAIdx}, {"p2", procAIdx}},
        {{"p1", procBIdx}, {"p2", procBIdx}},
        {{"p1", procBIdx}, {"p2", procCIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"p1", procAIdx}, {"p2", procAIdx}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"p1", procBIdx}, {"p2", procBIdx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"p1", procBIdx}, {"p2", procCIdx}})));
  }

  SECTION("with c1.procName = c2.procName") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    Param right = {ParamType::ATTRIBUTE_PROC_NAME, "c2"};
    vector<IntermediateQueryResult> currentResults = {
        {{"c1", callAIdx}, {"c2", callAIdx}},
        {{"c1", callAIdx}, {"c2", callBIdx}},
        {{"c1", callBIdx}, {"c2", callCIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", callAIdx}, {"c2", callAIdx}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", callAIdx}, {"c2", callBIdx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"c1", callBIdx}, {"c2", callCIdx}})));
  }

  SECTION("with p1.procName = c1.procName") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    vector<IntermediateQueryResult> currentResults = {
        {{"c1", callCIdx}, {"p1", procAIdx}},
        {{"c1", callCIdx}, {"p1", procBIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", callCIdx}, {"p1", procAIdx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"c1", callCIdx}, {"p1", procBIdx}})));
  }

  SECTION("with p1.procName = v1.varName") {
    int aVarIdx = pkb->insertAt(TableType::VAR_TABLE, "A");
    int bVarIdx = pkb->insertAt(TableType::VAR_TABLE, "B");
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    vector<IntermediateQueryResult> currentResults = {
        {{"p1", procAIdx}, {"v1", aVarIdx}},
        {{"p1", procAIdx}, {"v1", xVarIdx}},
        {{"p1", procBIdx}, {"v1", bVarIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"p1", procAIdx}, {"v1", aVarIdx}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"p1", procBIdx}, {"v1", bVarIdx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"p1", procAIdx}, {"v1", xVarIdx}})));
  }

  SECTION("with c1.procName = v1.varName") {
    int aVarIdx = pkb->insertAt(TableType::VAR_TABLE, "A");
    int dVarIdx = pkb->insertAt(TableType::VAR_TABLE, "D");
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    vector<IntermediateQueryResult> currentResults = {
        {{"c1", callCIdx}, {"v1", aVarIdx}},
        {{"c1", callAIdx}, {"v1", dVarIdx}},
        {{"c1", callBIdx}, {"v1", aVarIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", callCIdx}, {"v1", aVarIdx}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", callAIdx}, {"v1", dVarIdx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"c1", callBIdx}, {"v1", aVarIdx}})));
  }

  SECTION("with p1.procName = 'A'") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "p1"};
    Param right = {ParamType::NAME_LITERAL, "A"};
    vector<IntermediateQueryResult> currentResults = {
        {{"p1", procAIdx}, {"p2", procBIdx}, {"v1", xVarIdx}},
        {{"p1", procBIdx}, {"p2", procAIdx}, {"v1", yVarIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult(
                     {{"p1", procAIdx}, {"p2", procBIdx}, {"v1", xVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult(
                     {{"p1", procBIdx}, {"p2", procAIdx}, {"v1", yVarIdx}})));
  }

  SECTION("with c1.procName = 'D'") {
    Param left = {ParamType::ATTRIBUTE_PROC_NAME, "c1"};
    Param right = {ParamType::NAME_LITERAL, "D"};
    vector<IntermediateQueryResult> currentResults = {{{"c1", callAIdx}},
                                                      {{"c1", callBIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"c1", callAIdx}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"c1", callBIdx}})));
  }

  SECTION("with v1.varName = v2.varName") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v2"};
    vector<IntermediateQueryResult> currentResults = {
        {{"v1", xVarIdx}, {"v2", xVarIdx}},
        {{"v1", yVarIdx}, {"v2", yVarIdx}},
        {{"v1", yVarIdx}, {"v2", zVarIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"v1", xVarIdx}, {"v2", xVarIdx}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"v1", yVarIdx}, {"v2", yVarIdx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"v1", yVarIdx}, {"v2", zVarIdx}})));
  }

  SECTION("with rd1.varName = rd2.varName") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "rd1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "rd2"};
    vector<IntermediateQueryResult> currentResults = {
        {{"rd1", rdXIdx}, {"rd2", rdXIdx}}, {{"rd1", rdXIdx}, {"rd2", rdYIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"rd1", rdXIdx}, {"rd2", rdXIdx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"rd1", rdXIdx}, {"rd2", rdYIdx}})));
  }

  SECTION("with rd1.varName = v1.varName") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "rd1"};
    Param right = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    vector<IntermediateQueryResult> currentResults = {
        {{"rd1", rdXIdx}, {"v1", xVarIdx}}, {{"rd1", rdYIdx}, {"v1", yVarIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"rd1", rdXIdx}, {"v1", xVarIdx}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"rd1", rdYIdx}, {"v1", yVarIdx}})));
  }

  SECTION("with v1.varName = 'x'") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "v1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    vector<IntermediateQueryResult> currentResults = {
        {{"v1", xVarIdx}}, {{"v1", yVarIdx}}, {{"v1", yVarIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"v1", xVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult({{"v1", yVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult({{"v1", yVarIdx}})));
  }

  SECTION("with rd1.varName = 'x'") {
    Param left = {ParamType::ATTRIBUTE_VAR_NAME, "rd1"};
    Param right = {ParamType::NAME_LITERAL, "x"};
    vector<IntermediateQueryResult> currentResults = {{{"rd1", rdXIdx}},
                                                      {{"rd1", rdYIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"rd1", rdXIdx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult({{"rd1", rdYIdx}})));
  }

  SECTION("with 'xyz' = 'xyz'") {
    Param left = {ParamType::NAME_LITERAL, "xyz"};
    Param right = {ParamType::NAME_LITERAL, "xyz"};
    vector<IntermediateQueryResult> currentResults = {{{"v1", xVarIdx}},
                                                      {{"v1", yVarIdx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"v1", xVarIdx}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"v1", yVarIdx}})));
  }
}

TEST_CASE("WithEvaluator: Integer Attributes") {
  PKB* pkb = new PKB();
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::ASSIGN, 1);
  pkb->addStmt(DesignEntity::IF, 2);
  int const1Idx = pkb->insertAt(TableType::CONST_TABLE, "1");
  int const2Idx = pkb->insertAt(TableType::CONST_TABLE, "2");

  WithEvaluator we(pkb);
  unordered_map<string, DesignEntity> synonyms = {
      {"c1", DesignEntity::CONSTANT},  {"c2", DesignEntity::CONSTANT},
      {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
      {"a1", DesignEntity::ASSIGN},    {"a2", DesignEntity::ASSIGN},
      {"ifs", DesignEntity::IF},       {"n1", DesignEntity::PROG_LINE},
      {"n2", DesignEntity::PROG_LINE}};
  Synonym c1 = {DesignEntity::CONSTANT, "c1", false, {}};
  Synonym c2 = {DesignEntity::CONSTANT, "c2", false, {}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1", false, {}};
  Synonym s2 = {DesignEntity::STATEMENT, "s2", false, {}};
  Synonym a1 = {DesignEntity::ASSIGN, "a1", false, {}};
  Synonym a2 = {DesignEntity::ASSIGN, "a2", false, {}};
  Synonym ifs = {DesignEntity::IF, "ifs", false, {}};
  Synonym n1 = {DesignEntity::PROG_LINE, "n1", false, {}};
  Synonym n2 = {DesignEntity::PROG_LINE, "n2", false, {}};

  SECTION("with n1 = n2") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::SYNONYM, "n2"};
    vector<IntermediateQueryResult> currentResults = {{{"n1", 1}, {"n2", 1}},
                                                      {{"n1", 2}, {"n2", 3}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(IntermediateQueryResult({{"n1", 1}, {"n2", 1}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(IntermediateQueryResult({{"n1", 2}, {"n2", 3}})));
  }

  SECTION("with n1 = s1.stmt#") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    vector<IntermediateQueryResult> currentResults = {{{"n1", 1}, {"s1", 1}},
                                                      {{"n1", 2}, {"s1", 3}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(IntermediateQueryResult({{"n1", 1}, {"s1", 1}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(IntermediateQueryResult({{"n1", 2}, {"s1", 3}})));
  }

  SECTION("with n1 = c1.value") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::ATTRIBUTE_VALUE, "c1"};
    vector<IntermediateQueryResult> currentResults = {
        {{"n1", 1}, {"c1", const1Idx}}, {{"n1", 2}, {"c1", const2Idx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"n1", 1}, {"c1", const1Idx}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"n1", 2}, {"c1", const2Idx}})));
  }

  SECTION("with n1 = 3") {
    Param left = {ParamType::SYNONYM, "n1"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    vector<IntermediateQueryResult> currentResults = {
        {{"n1", 1}}, {{"n1", 2}}, {{"n1", 3}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"n1", 3}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult({{"n1", 1}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult({{"n1", 2}})));
  }

  SECTION("with c1.value = c2.value") {
    Param left = {ParamType::ATTRIBUTE_VALUE, "c1"};
    Param right = {ParamType::ATTRIBUTE_VALUE, "c2"};
    vector<IntermediateQueryResult> currentResults = {
        {{"c1", const1Idx}, {"c2", const1Idx}},
        {{"c1", const1Idx}, {"c2", const2Idx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", const1Idx}, {"c2", const1Idx}})));
    REQUIRE_THAT(newQueryResults, !VectorContains(IntermediateQueryResult(
                                      {{"c1", const1Idx}, {"c2", const2Idx}})));
  }

  SECTION("with c1.value = s1.value") {
    Param left = {ParamType::ATTRIBUTE_VALUE, "c1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    vector<IntermediateQueryResult> currentResults = {
        {{"c1", const1Idx}, {"s1", 1}}, {{"c1", const2Idx}, {"s1", 2}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", const1Idx}, {"s1", 1}})));
    REQUIRE_THAT(newQueryResults, VectorContains(IntermediateQueryResult(
                                      {{"c1", const2Idx}, {"s1", 2}})));
  }

  SECTION("with s1.value = s2.value") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s2"};
    vector<IntermediateQueryResult> currentResults = {{{"s1", 1}, {"s2", 1}},
                                                      {{"s1", 2}, {"s2", 3}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(IntermediateQueryResult({{"s1", 1}, {"s2", 1}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(IntermediateQueryResult({{"s1", 2}, {"s2", 3}})));
  }

  SECTION("with a1.value = a2.value") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "a1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "a2"};
    vector<IntermediateQueryResult> currentResults = {{{"a1", 1}, {"a2", 1}},
                                                      {{"a1", 2}, {"a2", 3}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(IntermediateQueryResult({{"a1", 1}, {"a2", 1}})));
    REQUIRE_THAT(
        newQueryResults,
        !VectorContains(IntermediateQueryResult({{"a1", 2}, {"a2", 3}})));
  }

  SECTION("with a1.value = s1.value") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "a1"};
    Param right = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    vector<IntermediateQueryResult> currentResults = {{{"a1", 1}, {"s1", 1}},
                                                      {{"a1", 2}, {"s1", 2}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(IntermediateQueryResult({{"a1", 1}, {"s1", 1}})));
    REQUIRE_THAT(
        newQueryResults,
        VectorContains(IntermediateQueryResult({{"a1", 2}, {"s1", 2}})));
  }

  SECTION("with c1.value = 1") {
    Param left = {ParamType::ATTRIBUTE_VALUE, "c1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    vector<IntermediateQueryResult> currentResults = {{{"c1", const1Idx}},
                                                      {{"c1", const2Idx}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"c1", const1Idx}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult({{"c1", const2Idx}})));
  }

  SECTION("with s1.value = 2") {
    Param left = {ParamType::ATTRIBUTE_STMT_NUM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    vector<IntermediateQueryResult> currentResults = {{{"s1", 1}}, {{"s1", 2}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"s1", 2}})));
    REQUIRE_THAT(newQueryResults,
                 !VectorContains(IntermediateQueryResult({{"s1", 1}})));
  }

  SECTION("with 1 = 1") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    vector<IntermediateQueryResult> currentResults = {{{"s1", 1}}, {{"s1", 2}}};
    auto results = we.evaluateAttributes(left, right, synonyms, currentResults);
    auto newQueryResults = get<1>(results);
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"s1", 1}})));
    REQUIRE_THAT(newQueryResults,
                 VectorContains(IntermediateQueryResult({{"s1", 2}})));
  }
}
