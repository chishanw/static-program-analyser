#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/AffectsOnDemandEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace std;
using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("AffectsOnDemandEvaluator: Affects, No Nested If/While") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 12; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // example procedure
  // 1: x = 0;
  // 2: i = 5;
  // 3: while (i != 0) {
  // 4:   x = x + 2 * y;
  // 5:   i = i - 1; }
  // 6: if (x == 1) then {
  // 7:   x = x + 1; }
  // 8: else { x = 2;
  // 9:   z = 1; }
  // 10: z = z + x + i;
  // 11: x = 3;
  // 12: x = x * y + z;
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 8);
  pkb->addRs(RelationshipType::NEXT, 8, 9);
  pkb->addRs(RelationshipType::NEXT, 7, 10);
  pkb->addRs(RelationshipType::NEXT, 9, 10);
  pkb->addRs(RelationshipType::NEXT, 10, 11);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addStmt(DesignEntity::WHILE, 3);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addNextStmtForIfStmt(6, 10);
  for (int i = 1; i <= 12; i++) {
    if (i != 3 && i != 6) {
      pkb->addStmt(DesignEntity::ASSIGN, i);
    }
  }
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 8, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 10, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 12, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "z");

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS;

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(1, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(1, 1)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(3, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "3"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11})));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({12})));
  }

  SECTION("Affects(s1, 4)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 4}));
  }

  SECTION("Affects(s1, 10)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "10"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({2, 5, 7, 8, 9}));
  }

  SECTION("Affects(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({4, 7}));
  }

  SECTION("Affects(5, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({5, 10}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 12})));
  }

  /* Cache correctness ---------------------------------------- */
  SECTION("Affects(s1, s2) - Complete Cache, Hit") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    ae.evaluatePairAffects(rsType, left, right);

    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 12})));
  }

  SECTION("Affects(s1, s2) - Complete Cache, Miss") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);

    Param left2 = {ParamType::SYNONYM, "s1"};
    Param right2 = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left2, right2);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 12})));
  }

  SECTION("Affects(s1, _) & (_, s2) - Complete Cache, Hit") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    ae.evaluatePairAffects(rsType, left, right);

    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::WILDCARD, "_"};
    auto setOfResults1 = ae.evaluatePairAffects(rsType, left1, right1);
    vector<vector<int>> results1(setOfResults1.begin(), setOfResults1.end());
    REQUIRE_THAT(results1, VectorContains(vector<int>({1})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({2})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({4})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({8})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({9})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results1, VectorContains(vector<int>({11})));

    Param left2 = {ParamType::WILDCARD, "_"};
    Param right2 = {ParamType::SYNONYM, "s2"};
    auto setOfResults2 = ae.evaluatePairAffects(rsType, left2, right2);
    vector<vector<int>> results2(setOfResults2.begin(), setOfResults2.end());
    REQUIRE_THAT(results2, VectorContains(vector<int>({4})));
    REQUIRE_THAT(results2, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results2, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results2, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results2, VectorContains(vector<int>({12})));
  }

  SECTION("Affects(s1, 10) & Affects(1, s2) - Complete Cache, Hit") {
    Param left0 = {ParamType::SYNONYM, "s1"};
    Param right0 = {ParamType::WILDCARD, "_"};
    ae.evaluatePairAffects(rsType, left0, right0);

    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::INTEGER_LITERAL, "10"};
    auto results1 = ae.evaluateStmtAffects(rsType, left1, right1);
    REQUIRE(results1 == unordered_set<STMT_NO>({2, 5, 7, 8, 9}));

    Param left2 = {ParamType::INTEGER_LITERAL, "1"};
    Param right2 = {ParamType::SYNONYM, "s2"};
    auto results2 = ae.evaluateStmtAffects(rsType, left2, right2);
    REQUIRE(results2 == unordered_set<STMT_NO>({4, 7}));
  }

  SECTION("Affects(s1, 10) & Affects(1, s2) - Incomplete Cache, Miss") {
    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::INTEGER_LITERAL, "10"};
    auto results1 = ae.evaluateStmtAffects(rsType, left1, right1);
    REQUIRE(results1 == unordered_set<STMT_NO>({2, 5, 7, 8, 9}));

    Param left2 = {ParamType::INTEGER_LITERAL, "1"};
    Param right2 = {ParamType::SYNONYM, "s2"};
    auto results2 = ae.evaluateStmtAffects(rsType, left2, right2);
    REQUIRE(results2 == unordered_set<STMT_NO>({4, 7}));
  }

  SECTION("Affects(1, 4) & Affects(_, _) - Incomplete Cache, Hit") {
    Param left1 = {ParamType::INTEGER_LITERAL, "1"};
    Param right1 = {ParamType::INTEGER_LITERAL, "4"};
    bool result1 = ae.evaluateBoolAffects(rsType, left1, right1);
    REQUIRE(result1 == true);

    Param left2 = {ParamType::WILDCARD, "_"};
    Param right2 = {ParamType::WILDCARD, "_"};
    bool result2 = ae.evaluateBoolAffects(rsType, left2, right2);
    REQUIRE(result2 == true);
  }

  SECTION("Affects(_, _) & Affects(1, 4) - Incomplete Cache, Hit") {
    Param left1 = {ParamType::WILDCARD, "_"};
    Param right1 = {ParamType::WILDCARD, "_"};
    bool result1 = ae.evaluateBoolAffects(rsType, left1, right1);
    REQUIRE(result1 == true);

    Param left2 = {ParamType::INTEGER_LITERAL, "1"};
    Param right2 = {ParamType::INTEGER_LITERAL, "4"};
    bool result2 = ae.evaluateBoolAffects(rsType, left2, right2);
    REQUIRE(result2 == true);
  }

  SECTION("Affects(_, _) & Affects(8, 10) - Incomplete Cache, Miss") {
    Param left1 = {ParamType::WILDCARD, "_"};
    Param right1 = {ParamType::WILDCARD, "_"};
    bool result1 = ae.evaluateBoolAffects(rsType, left1, right1);
    REQUIRE(result1 == true);

    Param left2 = {ParamType::INTEGER_LITERAL, "8"};
    Param right2 = {ParamType::INTEGER_LITERAL, "10"};
    bool result2 = ae.evaluateBoolAffects(rsType, left2, right2);
    REQUIRE(result2 == true);
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects, Nested If/While Separately") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 13; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 11);
  pkb->addRs(RelationshipType::NEXT, 7, 8);
  pkb->addRs(RelationshipType::NEXT, 8, 9);
  pkb->addRs(RelationshipType::NEXT, 8, 10);
  pkb->addRs(RelationshipType::NEXT, 9, 12);
  pkb->addRs(RelationshipType::NEXT, 10, 12);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addRs(RelationshipType::NEXT, 12, 13);

  unordered_set<int> whileStmts = {2, 4};
  for (int w : whileStmts) {
    pkb->addStmt(DesignEntity::WHILE, w);
  }
  unordered_set<int> ifStmts = {6, 8};
  for (int ifs : ifStmts) {
    pkb->addStmt(DesignEntity::IF, ifs);
  }
  pkb->addNextStmtForIfStmt(6, 12);
  pkb->addNextStmtForIfStmt(8, 12);
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
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 10, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 12, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 13, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 13, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 13, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 13, TableType::VAR_TABLE, "z");

  // example procedure
  // 1: x = 0;
  // 2: while (i != 0) {
  // 3:   x = x + 2 * y;
  // 4:   while (i != 0) {
  // 5:     x = x + 1; } }
  // 6: if (x == 1) then {
  // 7:   x = x + 1;
  // 8:   if (y == 2) then {
  // 9:     x = 3; }
  // 10:  else { y = x * 2; }
  // 11: else { x = 4; } }
  // 12: z = z + x;
  // 13: x = x * y + z;

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS;

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(1, 7)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(1, 5))") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(7, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, 11)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "11"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11})));
    REQUIRE_THAT(results, VectorContains(vector<int>({12})));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({13})));
  }

  SECTION("Affects(s1, 7)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 3, 5}));
  }

  SECTION("Affects(s1, 13)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "13"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({7, 9, 10, 11, 12}));
  }

  SECTION("Affects(9, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "9"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({12, 13}));
  }

  SECTION("Affects(7, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({10, 12, 13}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 13})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 13})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11, 13})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 13})));
    REQUIRE_THAT(results, VectorContains(vector<int>({12, 13})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({1, 5})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 9})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({9, 10})));
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects, Nested If/While Together") {
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
  for (int i = 1; i <= 8; i++) {
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

  // example procedure
  // 1: x = 0;
  // 2: while (i != 0) {
  // 3:   x = x + 2 * y;
  // 4:   if (i != 0) then {
  // 5:     x = x + 2; }
  // NA:  else {
  // 6:     while (x != y) {
  // 7:       x = x; } }
  // 8: y = x + 1;

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS;

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(5, 3)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(7, 5))") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(7, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, 6)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8})));
  }

  SECTION("Affects(s1, 8)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "8"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 3, 5, 7}));
  }

  SECTION("Affects(s1, 3)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 3, 5, 7}));
  }

  SECTION("Affects(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({3, 8}));
  }

  SECTION("Affects(7, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({3, 7, 8}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
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
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 5})));
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects, Multiple Procedures") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 5);
  for (int i = 1; i <= 7; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }

  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 2, 4);
  pkb->addRs(RelationshipType::NEXT, 5, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 7);

  unordered_set<int> whileStmts = {5};
  for (int w : whileStmts) {
    pkb->addStmt(DesignEntity::WHILE, w);
  }
  unordered_set<int> ifStmts = {2};
  for (int ifs : ifStmts) {
    pkb->addStmt(DesignEntity::IF, ifs);
  }
  for (int i = 1; i <= 7; i++) {
    if (whileStmts.find(i) == whileStmts.end() &&
        ifStmts.find(i) == ifStmts.end()) {
      pkb->addStmt(DesignEntity::ASSIGN, i);
    }
  }

  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "x");

  pkb->addRs(RelationshipType::MODIFIES_S, 6, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "y");

  // example program w/ multiple procs
  // procedure A {
  // 1: x = 0;
  // 2: if (x == 0) then {
  // 3:   x = x + 1 ; }
  // 4: else { y = x; } }
  // procedure B {
  // 5: while (x == 2) {
  // 6:   y = 3; }
  // 7: z = y * 2; }

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS;

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(1, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(1, 1))") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(6, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "6"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, 7)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({6})));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
  }

  SECTION("Affects(s1, 7)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({6}));
  }

  SECTION("Affects(s1, 4)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1}));
  }

  SECTION("Affects(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({3, 4}));
  }

  SECTION("Affects(6, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "6"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<STMT_NO>({7}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 4})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({3, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({6, 7})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({6, 6})));
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects, Test Call & Read Stmts") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 6);
  pkb->insertAt(TableType::PROC_TABLE, "C");
  pkb->addFirstStmtOfProc("C", 9);
  for (int i = 1; i <= 9; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }

  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 7, 8);
  pkb->addRs(RelationshipType::NEXT, 8, 6);

  pkb->addStmt(DesignEntity::WHILE, 6);
  pkb->addStmt(DesignEntity::IF, 3);
  pkb->addStmt(DesignEntity::READ, 2);
  pkb->addStmt(DesignEntity::CALL, 8);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "B",
             TableType::PROC_TABLE, "C");
  pkb->addRs(RelationshipType::CALLS_S, 8, TableType::PROC_TABLE, "C");
  vector<int> assignStmts = {1, 4, 5, 7, 9};
  for (auto stmt : assignStmts) {
    pkb->addStmt(DesignEntity::ASSIGN, stmt);
  }

  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "x");

  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 8, TableType::VAR_TABLE, "y");

  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "y");

  // example program w/ read & call stmts
  // procedure A {
  // 1: x = 3;
  // 2: read x;
  // 3: if (x == 0) then {
  // 4:   x = x + 1 ; }
  // 5: else { y = x; } }
  // procedure B {
  // 6: while (x == 2) {
  // 7:   y = y + 1;
  // 8:   call C; } }
  // procedure C {
  // 9: y = 2; }

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS;

  // Test read stmt
  SECTION("Affects(1, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(2, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  // Test call stmt
  SECTION("Affects(7, 7)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(7, 8)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::INTEGER_LITERAL, "8"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  // Test all Affects generated
  SECTION("Affects(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE(results.empty());
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects*, No Nested If/While") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 12; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // example procedure
  // 1: x = 0;
  // 2: i = 5;
  // 3: while (i != 0) {
  // 4:   x = x + 2 * y;
  // 5:   i = i - 1; }
  // 6: if (x == 1) then {
  // 7:   x = x + 1; }
  // 8: else { x = 2;
  // 9:   z = 1; }
  // 10: z = z + x + i;
  // 11: x = 3;
  // 12: x = x * y + z;

  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 8);
  pkb->addRs(RelationshipType::NEXT, 8, 9);
  pkb->addRs(RelationshipType::NEXT, 7, 10);
  pkb->addRs(RelationshipType::NEXT, 9, 10);
  pkb->addRs(RelationshipType::NEXT, 10, 11);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addStmt(DesignEntity::WHILE, 3);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addNextStmtForIfStmt(6, 10);
  for (int i = 1; i <= 12; i++) {
    if (i != 3 && i != 6) {
      pkb->addStmt(DesignEntity::ASSIGN, i);
    }
  }
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 8, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 10, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 12, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "z");

  AffectsOnDemandEvaluator ae(pkb);

  /* Boolean results ---------------------------------------- */
  SECTION("Affects*(1, 7)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(1, 5)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(3, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "3"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects*(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ae.evaluatePairAffectsT(left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11})));
  }

  SECTION("Affects*(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffectsT(left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({12})));
  }

  SECTION("Affects*(s1, 4)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    auto results = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 4}));
  }

  SECTION("Affects*(s1, 10)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "10"};
    auto results = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 2, 4, 5, 7, 8, 9}));
  }

  SECTION("Affects*(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({4, 7, 10, 12}));
  }

  SECTION("Affects*(5, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({5, 10, 12}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects*(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffectsT(left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({8, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 12})));
    REQUIRE_THAT(results, VectorContains(vector<int>({11, 12})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 8})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 9})));
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects*, Nested If/While Separately") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 19; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }

  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 7, 8);
  pkb->addRs(RelationshipType::NEXT, 8, 12);
  pkb->addRs(RelationshipType::NEXT, 6, 9);
  pkb->addRs(RelationshipType::NEXT, 9, 10);
  pkb->addRs(RelationshipType::NEXT, 10, 12);
  pkb->addRs(RelationshipType::NEXT, 3, 11);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addRs(RelationshipType::NEXT, 12, 13);
  pkb->addRs(RelationshipType::NEXT, 12, 19);
  pkb->addRs(RelationshipType::NEXT, 13, 14);
  pkb->addRs(RelationshipType::NEXT, 14, 15);
  pkb->addRs(RelationshipType::NEXT, 15, 16);
  pkb->addRs(RelationshipType::NEXT, 15, 18);
  pkb->addRs(RelationshipType::NEXT, 16, 17);
  pkb->addRs(RelationshipType::NEXT, 17, 15);
  pkb->addRs(RelationshipType::NEXT, 18, 12);
  pkb->addStmt(DesignEntity::IF, 3);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addStmt(DesignEntity::WHILE, 12);
  pkb->addStmt(DesignEntity::WHILE, 15);
  pkb->addNextStmtForIfStmt(3, 12);
  pkb->addNextStmtForIfStmt(6, 12);
  vector<STMT_NO> assign{1, 2, 4, 5, 7, 8, 9, 10, 11, 13, 14, 16, 17, 18, 19};
  for (STMT_NO assignStmt : assign) {
    pkb->addStmt(DesignEntity::ASSIGN, assignStmt);
  }
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "c");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "d");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "f");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "g");
  pkb->addRs(RelationshipType::MODIFIES_S, 8, TableType::VAR_TABLE, "h");
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "i");
  pkb->addRs(RelationshipType::MODIFIES_S, 10, TableType::VAR_TABLE, "j");
  pkb->addRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE, "k");
  pkb->addRs(RelationshipType::MODIFIES_S, 13, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::MODIFIES_S, 14, TableType::VAR_TABLE, "f");
  pkb->addRs(RelationshipType::MODIFIES_S, 16, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::MODIFIES_S, 17, TableType::VAR_TABLE, "f");
  pkb->addRs(RelationshipType::MODIFIES_S, 18, TableType::VAR_TABLE, "l");
  pkb->addRs(RelationshipType::MODIFIES_S, 19, TableType::VAR_TABLE, "m");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "c");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "d");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::USES_S, 8, TableType::VAR_TABLE, "f");
  pkb->addRs(RelationshipType::USES_S, 9, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "f");
  pkb->addRs(RelationshipType::USES_S, 11, TableType::VAR_TABLE, "c");
  pkb->addRs(RelationshipType::USES_S, 13, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::USES_S, 14, TableType::VAR_TABLE, "d");
  pkb->addRs(RelationshipType::USES_S, 16, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::USES_S, 17, TableType::VAR_TABLE, "e");
  pkb->addRs(RelationshipType::USES_S, 19, TableType::VAR_TABLE, "l");
  /* procedure A {
  1    c = 3;
  2    d = 3;
  3    if (blah) then {
  4      e = c + 3;
  5      f = d + 3;
  6      if (blah2) then {
  7        g = e + 3;
  8        h = f + 3;
         } else {
  9        i = e + 3;
  10       j = f + 3;
         }
       } else {
  11     k = c + 3;
       }
  12   while (blah3) {
  13     e = e + 3;
  14     f = d + 3;
  15     while (blah4) {
  16       e = e + 3;
  17       f = e + 3;
         }
  18     l = 3;
       }
  19 m = l + 3;
     }*/

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS_T;

  SECTION("Affects*(2, 10)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "10"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(1, 19)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "19"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(5, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(19, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "19"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, 14)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "14"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(_, 2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{4, 7, 9, 11, 13, 16, 17});
  }

  SECTION("Affects*(11, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "11"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{});
  }

  SECTION("Affects*(s1, 16)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "16"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{1, 4, 13, 16});
  }

  SECTION("Affects*(s1,s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffectsT(left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 9})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 11})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 13})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 16})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 17})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 8})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 10})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 14})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 9})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 11})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({9, 11})));
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects*, Nested If/While Together") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 16; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }

  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 2, 16);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 6);
  pkb->addRs(RelationshipType::NEXT, 5, 14);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 7, 8);
  pkb->addRs(RelationshipType::NEXT, 8, 9);
  pkb->addRs(RelationshipType::NEXT, 8, 15);
  pkb->addRs(RelationshipType::NEXT, 9, 10);
  pkb->addRs(RelationshipType::NEXT, 10, 11);
  pkb->addRs(RelationshipType::NEXT, 10, 13);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addRs(RelationshipType::NEXT, 12, 8);
  pkb->addRs(RelationshipType::NEXT, 13, 8);
  pkb->addRs(RelationshipType::NEXT, 14, 15);
  pkb->addRs(RelationshipType::NEXT, 15, 2);
  pkb->addStmt(DesignEntity::IF, 5);
  pkb->addStmt(DesignEntity::IF, 10);
  pkb->addStmt(DesignEntity::WHILE, 2);
  pkb->addStmt(DesignEntity::WHILE, 8);
  pkb->addNextStmtForIfStmt(5, 15);
  pkb->addNextStmtForIfStmt(10, 8);
  vector<STMT_NO> assign{1, 3, 4, 6, 7, 9, 11, 12, 13, 14, 15, 16};
  for (STMT_NO assignStmt : assign) {
    pkb->addStmt(DesignEntity::ASSIGN, assignStmt);
  }
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 6, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 12, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 13, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 14, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 15, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 16, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 6, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 11, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 12, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 13, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 14, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 15, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 15, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 16, TableType::VAR_TABLE, "z");
  /* procedure A {
  1    x = 3;
  2    while (blah) {
  3      x = x + 3;
  4      y = 3;
  5      if (blah1) then {
  6        x = x + 3;
  7        y = y;
  8        while (blah 2) {
  9          z = 1;
  10         if (blah3) then {
  11           y = y;
  12           z = z;
             } else {
  13           x = x;
             }
           }
         } else {
  14       x = y;
         }
  15     z = x + y;
       }
  16   x = z;
     }*/

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS_T;

  SECTION("Affects*(1, 16)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "16"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(15, 9)") {
    Param left = {ParamType::INTEGER_LITERAL, "15"};
    Param right = {ParamType::INTEGER_LITERAL, "9"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(4, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "4"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(16, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "16"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, 13)") {
    Param right = {ParamType::WILDCARD, "_"};
    Param left = {ParamType::INTEGER_LITERAL, "13"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(_, 4)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{3, 6, 13, 15, 16});
  }

  SECTION("Affects*(9, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "9"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{12});
  }

  SECTION("Affects*(s1, 6)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{1, 3, 4, 6, 13, 14});
  }

  SECTION("Affects*(s1, 15)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "15"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{1, 3, 4, 6, 7, 11, 13, 14});
  }

  SECTION("Affects*(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffectsT(left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 6})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 13})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 15})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 16})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 11})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 14})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 15})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 6})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 13})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 16})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7, 14})));
  }

  /* CacheT testing ------------------------------------------*/

  SECTION("Affects*(4, 16) with complete cacheT - Hit") {
    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::SYNONYM, "s2"};
    ae.evaluatePairAffectsT(left1, right1);

    Param left = {ParamType::INTEGER_LITERAL, "4"};
    Param right = {ParamType::INTEGER_LITERAL, "16"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(15, 12) with complete cacheT - Hit") {
    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::SYNONYM, "s2"};
    ae.evaluatePairAffectsT(left1, right1);

    Param left = {ParamType::INTEGER_LITERAL, "15"};
    Param right = {ParamType::INTEGER_LITERAL, "12"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(7, s2) with complete cacheT - Hit") {
    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::SYNONYM, "s2"};
    ae.evaluatePairAffectsT(left1, right1);

    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{11, 15, 16});
  }

  SECTION("Affects*(s1, 15) with complete cacheT - Hit") {
    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::SYNONYM, "s2"};
    ae.evaluatePairAffectsT(left1, right1);

    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "15"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{1, 3, 4, 6, 7, 11, 13, 14});
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects*, Multiple Procedures") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 7);
  for (int i = 1; i <= 11; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 4);
  pkb->addRs(RelationshipType::NEXT, 7, 8);
  pkb->addRs(RelationshipType::NEXT, 8, 9);
  pkb->addRs(RelationshipType::NEXT, 8, 10);
  pkb->addRs(RelationshipType::NEXT, 9, 11);
  pkb->addRs(RelationshipType::NEXT, 10, 11);
  pkb->addStmt(DesignEntity::IF, 8);
  pkb->addStmt(DesignEntity::WHILE, 4);
  pkb->addNextStmtForIfStmt(8, 11);
  vector<STMT_NO> assign{1, 2, 3, 5, 6, 7, 9, 10, 11};
  for (STMT_NO assignStmt : assign) {
    pkb->addStmt(DesignEntity::ASSIGN, assignStmt);
  }
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 6, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 10, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 6, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 6, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 9, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 11, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 11, TableType::VAR_TABLE, "z");
  /* procedure A {
  1    x = 3;
  2    y = x + 3;
  3    z = 3;
  4    while (blah) {
  5      z = z + x;
  6      z = z + y;
       }
     }
     procedure B {
  7    x = z;
  8    if (blah1) then {
  9      y = x;
       } else {
  10     z = y + 2;
       }
  11   x = y + z;
     }*/

  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS_T;

  SECTION("Affects*(1, 6)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(3, 7)") {
    Param left = {ParamType::INTEGER_LITERAL, "3"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(2, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(11, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "11"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, 9)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "9"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(_, 3)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(6, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "6"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{5, 6});
  }

  SECTION("Affects*(s1, 10)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "10"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{});
  }

  SECTION("Affects*(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffectsT(left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 2})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 6})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 6})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 11})));
    REQUIRE_THAT(results, VectorContains(vector<int>({10, 11})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({1, 9})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({2, 10})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({6, 7})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({9, 10})));
  }
}

TEST_CASE("AffectsOnDemandEvaluator: Affects*, Test Call & Read Stmts") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 9);
  pkb->insertAt(TableType::PROC_TABLE, "C");
  pkb->addFirstStmtOfProc("C", 13);
  for (int i = 1; i <= 13; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 3, 4);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 6);
  pkb->addRs(RelationshipType::NEXT, 6, 7);
  pkb->addRs(RelationshipType::NEXT, 7, 8);
  pkb->addRs(RelationshipType::NEXT, 9, 10);
  pkb->addRs(RelationshipType::NEXT, 11, 12);
  pkb->addRs(RelationshipType::NEXT, 12, 13);

  vector<int> assignStmts = {1, 2, 4, 6, 8, 9, 11, 13};
  for (auto stmt : assignStmts) {
    pkb->addStmt(DesignEntity::ASSIGN, stmt);
  }
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 2, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 4, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 6, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 8, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 13, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 6, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 6, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 8, TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::USES_S, 8, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 9, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 11, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 11, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::USES_S, 13, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 13, TableType::VAR_TABLE, "z");

  pkb->addStmt(DesignEntity::CALL, 5);
  pkb->addStmt(DesignEntity::CALL, 7);
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "A",
             TableType::PROC_TABLE, "B");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "A",
             TableType::PROC_TABLE, "C");
  pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "B",
             TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "C",
             TableType::VAR_TABLE, "z");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "z");
  pkb->addStmt(DesignEntity::READ, 3);
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addStmt(DesignEntity::READ, 10);
  pkb->addRs(RelationshipType::MODIFIES_S, 10, TableType::VAR_TABLE, "x");
  pkb->addStmt(DesignEntity::READ, 12);
  pkb->addRs(RelationshipType::MODIFIES_S, 12, TableType::VAR_TABLE, "y");
  /* procedure A {
  1    x = 3;
  2    y = 3;
  3    read x;
  4    z = x + y;
  5    call B;
  6    z = z + x;
  7    call C;
  8    z = z + y;
     }
     procedure B {  // modifies x only
  9    x = x + 3;
  10   read x;
     }
     procedure C {  // modifies y and z
  11   z = x + y;
  12   read y;
  13   z = z + y;
     }
  */
  AffectsOnDemandEvaluator ae(pkb);

  RelationshipType rsType = RelationshipType::AFFECTS_T;

  SECTION("Affects*(2, 6)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(2, 11)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "11"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  /* Tests read Affects ---------------------------------*/
  SECTION("Affects*(2, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(1, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(2, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, 6)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  /* Tests call Affects ---------------------------------*/
  SECTION("Affects*(_, 8)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "9"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects*(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffectsT(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects*(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{});
  }

  SECTION("Affects*(2, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{4, 6});
  }

  SECTION("Affects*(s1, 6)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{2, 4});
  }

  /* Tests call and read Affects ---------------------------------*/
  SECTION("Affects*(s1, 8)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "8"};
    auto result = ae.evaluateStmtAffectsT(left, right);
    REQUIRE(result == unordered_set<STMT_NO>{});
  }

  SECTION("Affects*(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffectsT(left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({2, 6})));
    REQUIRE_THAT(results, VectorContains(vector<int>({4, 6})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({2, 8})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({6, 8})));
  }
}

TEST_CASE("AffectsBip - Truthy Values") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 8);
  pkb->insertAt(TableType::PROC_TABLE, "C");
  pkb->addFirstStmtOfProc("B", 9);
  for (int i = 1; i <= 9; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // procedure A {
  // 1: x = 0;
  // 2: while (i != 0) {
  // 3:   x = x + 2 * y;
  // 4:   if (i != 0) then {
  // 5:     x = x + 2; }
  // NA:  else {
  // 6:     call B; } }
  // 7: y = x + 1;
  // }
  // procedure B {
  // 8:  call C; }
  // procedure C {
  // 9:  x = x + 1; }
  RelationshipType cfgRsType = RelationshipType::NEXT_BIP;
  pkb->addRs(cfgRsType, 1, 2);
  pkb->addRs(cfgRsType, 2, 3);
  pkb->addRs(cfgRsType, 3, 4);
  pkb->addRs(cfgRsType, 4, 5);
  pkb->addRs(cfgRsType, 4, 6);
  pkb->addRs(cfgRsType, 6, 8);
  pkb->addRs(cfgRsType, 8, 9);
  pkb->addRs(cfgRsType, 9, 2);
  pkb->addRs(cfgRsType, 2, 7);

  pkb->addStmt(DesignEntity::WHILE, 2);
  pkb->addStmt(DesignEntity::IF, 4);
  pkb->addStmt(DesignEntity::CALL, 6);
  pkb->addStmt(DesignEntity::CALL, 8);
  unordered_set<int> nonAssignments = {2, 4, 6, 8};
  pkb->addNextStmtForIfStmt(4, 2);
  for (int i = 1; i <= 9; i++) {
    if (nonAssignments.find(i) == nonAssignments.end()) {
      pkb->addStmt(DesignEntity::ASSIGN, i);
    }
  }
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "A",
             TableType::PROC_TABLE, "B");
  pkb->addRs(RelationshipType::CALLS_S, 6, TableType::PROC_TABLE, "B");
  pkb->addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "B",
             TableType::PROC_TABLE, "C");
  pkb->addRs(RelationshipType::CALLS_S, 8, TableType::PROC_TABLE, "C");
  pkb->addRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "y");
  pkb->addRs(RelationshipType::MODIFIES_S, 5, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "x");
  pkb->addRs(RelationshipType::USES_S, 9, TableType::VAR_TABLE, "x");

  unordered_map<string, DesignEntity> synonyms = {
      {"s1", DesignEntity::STATEMENT},
      {"s2", DesignEntity::STATEMENT},
      {"a1", DesignEntity::ASSIGN},
      {"a2", DesignEntity::ASSIGN}};
  Synonym s1 = {DesignEntity::STATEMENT, "s1"};
  Synonym s2 = {DesignEntity::STATEMENT, "s2"};
  Synonym a1 = {DesignEntity::ASSIGN, "a1"};
  Synonym a2 = {DesignEntity::ASSIGN, "a2"};
  vector<ConditionClause> conditionClauses = {};

  AffectsOnDemandEvaluator ae(pkb);
  RelationshipType rsType = RelationshipType::AFFECTS_BIP;

  SECTION("AffectsBip(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("AffectsBip(3, 9)") {
    Param left = {ParamType::INTEGER_LITERAL, "3"};
    Param right = {ParamType::INTEGER_LITERAL, "9"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("AffectsBip(9, 3)") {
    Param left = {ParamType::INTEGER_LITERAL, "9"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("AffectsBip(5, 9)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::INTEGER_LITERAL, "9"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("AffectsBip(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("AffectsBip(9, s)") {
    Param left = {ParamType::INTEGER_LITERAL, "9"};
    Param right = {ParamType::SYNONYM, "s"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<int>({3, 7}));
  }

  SECTION("AffectsBip(s, 9)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::INTEGER_LITERAL, "9"};
    auto results = ae.evaluateStmtAffects(rsType, left, right);
    REQUIRE(results == unordered_set<int>({3}));
  }

  SECTION("AffectsBip(s, _)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({7})));
  }

  SECTION("AffectsBip(_, s)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9})));
  }

  SECTION("AffectsBip(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ae.evaluatePairAffects(rsType, left, right);
    vector<vector<int>> results(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 5})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({3, 9})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({5, 7})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({9, 7})));
  }
}
