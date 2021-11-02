#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/AffectsEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace std;
using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("AffectsEvaluator: Affects, No Nested If/While") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 12; i++) {
    pkb->addStmt(i);
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
  pkb->addNext(1, 2);
  pkb->addNext(2, 3);
  pkb->addNext(3, 4);
  pkb->addNext(4, 5);
  pkb->addNext(5, 3);
  pkb->addNext(3, 6);
  pkb->addNext(6, 7);
  pkb->addNext(6, 8);
  pkb->addNext(8, 9);
  pkb->addNext(7, 10);
  pkb->addNext(9, 10);
  pkb->addNext(10, 11);
  pkb->addNext(11, 12);
  pkb->addWhileStmt(3);
  pkb->addIfStmt(6);
  pkb->addNextStmtForIfStmt(6, 10);
  for (int i = 1; i <= 12; i++) {
    if (i != 3 && i != 6) {
      pkb->addAssignStmt(i);
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

  AffectsEvaluator ae(pkb);

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(1, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(1, 1)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(3, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "3"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({1, 2, 4, 5, 7, 8, 9, 10, 11}));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({4, 5, 7, 10, 12}));
  }

  SECTION("Affects(s1, 4)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 4}));
  }

  SECTION("Affects(s1, 10)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "10"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({2, 5, 7, 8, 9}));
  }

  SECTION("Affects(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({4, 7}));
  }

  SECTION("Affects(5, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({5, 10}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    auto results = ae.evaluatePairAffects();
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
    ae.evaluatePairAffects();

    auto results = ae.evaluatePairAffects();
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
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);

    auto results = ae.evaluatePairAffects();
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
    ae.evaluatePairAffects();

    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::WILDCARD, "_"};
    auto results1 = ae.evaluateStmtAffects(left1, right1);
    REQUIRE(results1 == unordered_set<int>({1, 2, 4, 5, 7, 8, 9, 10, 11}));

    Param left2 = {ParamType::WILDCARD, "_"};
    Param right2 = {ParamType::SYNONYM, "s2"};
    auto results2 = ae.evaluateStmtAffects(left2, right2);
    REQUIRE(results2 == unordered_set<int>({4, 5, 7, 10, 12}));
  }

  SECTION("Affects(s1, 10) & Affects(1, s2) - Complete Cache, Hit") {
    Param left0 = {ParamType::SYNONYM, "s1"};
    Param right0 = {ParamType::WILDCARD, "_"};
    ae.evaluateStmtAffects(left0, right0);

    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::INTEGER_LITERAL, "10"};
    auto results1 = ae.evaluateStmtAffects(left1, right1);
    REQUIRE(results1 == unordered_set<STMT_NO>({2, 5, 7, 8, 9}));

    Param left2 = {ParamType::INTEGER_LITERAL, "1"};
    Param right2 = {ParamType::SYNONYM, "s2"};
    auto results2 = ae.evaluateStmtAffects(left2, right2);
    REQUIRE(results2 == unordered_set<STMT_NO>({4, 7}));
  }

  SECTION("Affects(s1, 10) & Affects(1, s2) - Incomplete Cache, Miss") {
    Param left1 = {ParamType::SYNONYM, "s1"};
    Param right1 = {ParamType::INTEGER_LITERAL, "10"};
    auto results1 = ae.evaluateStmtAffects(left1, right1);
    REQUIRE(results1 == unordered_set<STMT_NO>({2, 5, 7, 8, 9}));

    Param left2 = {ParamType::INTEGER_LITERAL, "1"};
    Param right2 = {ParamType::SYNONYM, "s2"};
    auto results2 = ae.evaluateStmtAffects(left2, right2);
    REQUIRE(results2 == unordered_set<STMT_NO>({4, 7}));
  }

  SECTION("Affects(1, 4) & Affects(_, _) - Incomplete Cache, Hit") {
    Param left1 = {ParamType::INTEGER_LITERAL, "1"};
    Param right1 = {ParamType::INTEGER_LITERAL, "4"};
    bool result1 = ae.evaluateBoolAffects(left1, right1);
    REQUIRE(result1 == true);

    Param left2 = {ParamType::WILDCARD, "_"};
    Param right2 = {ParamType::WILDCARD, "_"};
    bool result2 = ae.evaluateBoolAffects(left2, right2);
    REQUIRE(result2 == true);
  }

  SECTION("Affects(_, _) & Affects(1, 4) - Incomplete Cache, Hit") {
    Param left1 = {ParamType::WILDCARD, "_"};
    Param right1 = {ParamType::WILDCARD, "_"};
    bool result1 = ae.evaluateBoolAffects(left1, right1);
    REQUIRE(result1 == true);

    Param left2 = {ParamType::INTEGER_LITERAL, "1"};
    Param right2 = {ParamType::INTEGER_LITERAL, "4"};
    bool result2 = ae.evaluateBoolAffects(left2, right2);
    REQUIRE(result2 == true);
  }

  SECTION("Affects(_, _) & Affects(8, 10) - Incomplete Cache, Miss") {
    Param left1 = {ParamType::WILDCARD, "_"};
    Param right1 = {ParamType::WILDCARD, "_"};
    bool result1 = ae.evaluateBoolAffects(left1, right1);
    REQUIRE(result1 == true);

    Param left2 = {ParamType::INTEGER_LITERAL, "8"};
    Param right2 = {ParamType::INTEGER_LITERAL, "10"};
    bool result2 = ae.evaluateBoolAffects(left2, right2);
    REQUIRE(result2 == true);
  }
}

TEST_CASE("AffectsEvaluator: Affects, Nested If/While Separately") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 13; i++) {
    pkb->addStmt(i);
  }
  pkb->addNext(1, 2);
  pkb->addNext(2, 3);
  pkb->addNext(3, 4);
  pkb->addNext(4, 5);
  pkb->addNext(5, 4);
  pkb->addNext(4, 2);
  pkb->addNext(2, 6);
  pkb->addNext(6, 7);
  pkb->addNext(6, 11);
  pkb->addNext(7, 8);
  pkb->addNext(8, 9);
  pkb->addNext(8, 10);
  pkb->addNext(9, 12);
  pkb->addNext(10, 12);
  pkb->addNext(11, 12);
  pkb->addNext(12, 13);

  unordered_set<int> whileStmts = {2, 4};
  for (int w : whileStmts) {
    pkb->addWhileStmt(w);
  }
  unordered_set<int> ifStmts = {6, 8};
  for (int ifs : ifStmts) {
    pkb->addIfStmt(ifs);
  }
  pkb->addNextStmtForIfStmt(6, 12);
  pkb->addNextStmtForIfStmt(8, 12);
  for (int i = 1; i <= 13; i++) {
    if (whileStmts.find(i) == whileStmts.end() &&
        ifStmts.find(i) == ifStmts.end()) {
      pkb->addAssignStmt(i);
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

  AffectsEvaluator ae(pkb);

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(1, 7)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(1, 5))") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(7, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, 11)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "11"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({1, 3, 5, 7, 9, 10, 11, 12}));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({3, 5, 7, 10, 12, 13}));
  }

  SECTION("Affects(s1, 7)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 3, 5}));
  }

  SECTION("Affects(s1, 13)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "13"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({7, 9, 10, 11, 12}));
  }

  SECTION("Affects(9, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "9"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({12, 13}));
  }

  SECTION("Affects(7, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({10, 12, 13}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    auto results = ae.evaluatePairAffects();
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

TEST_CASE("AffectsEvaluator: Affects, Nested If/While Together") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  for (int i = 1; i <= 8; i++) {
    pkb->addStmt(i);
  }
  pkb->addNext(1, 2);
  pkb->addNext(2, 3);
  pkb->addNext(3, 4);
  pkb->addNext(4, 5);
  pkb->addNext(4, 6);
  pkb->addNext(6, 7);
  pkb->addNext(7, 6);
  pkb->addNext(6, 2);
  pkb->addNext(5, 2);
  pkb->addNext(2, 8);

  unordered_set<int> whileStmts = {2, 6};
  for (int w : whileStmts) {
    pkb->addWhileStmt(w);
  }
  unordered_set<int> ifStmts = {4};
  for (int ifs : ifStmts) {
    pkb->addIfStmt(ifs);
  }
  pkb->addNextStmtForIfStmt(4, 2);
  for (int i = 1; i <= 8; i++) {
    if (whileStmts.find(i) == whileStmts.end() &&
        ifStmts.find(i) == ifStmts.end()) {
      pkb->addAssignStmt(i);
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

  AffectsEvaluator ae(pkb);

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(5, 3)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(7, 5))") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(7, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, 6)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({1, 3, 5, 7}));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({3, 5, 7, 8}));
  }

  SECTION("Affects(s1, 8)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "8"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 3, 5, 7}));
  }

  SECTION("Affects(s1, 3)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1, 3, 5, 7}));
  }

  SECTION("Affects(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({3, 8}));
  }

  SECTION("Affects(7, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({3, 7, 8}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    auto results = ae.evaluatePairAffects();
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

TEST_CASE("AffectsEvaluator: Affects, Multiple Procedures") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 5);
  for (int i = 1; i <= 7; i++) {
    pkb->addStmt(i);
  }

  pkb->addNext(1, 2);
  pkb->addNext(2, 3);
  pkb->addNext(2, 4);
  pkb->addNext(5, 6);
  pkb->addNext(6, 5);
  pkb->addNext(5, 7);

  unordered_set<int> whileStmts = {5};
  for (int w : whileStmts) {
    pkb->addWhileStmt(w);
  }
  unordered_set<int> ifStmts = {2};
  for (int ifs : ifStmts) {
    pkb->addIfStmt(ifs);
  }
  for (int i = 1; i <= 7; i++) {
    if (whileStmts.find(i) == whileStmts.end() &&
        ifStmts.find(i) == ifStmts.end()) {
      pkb->addAssignStmt(i);
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

  AffectsEvaluator ae(pkb);

  /* Boolean results ---------------------------------------- */
  SECTION("Affects(1, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(1, 1))") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(6, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "6"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, 7)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  SECTION("Affects(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == true);
  }

  /* Stmt results --------------------------------- */
  SECTION("Affects(s1, _)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::WILDCARD, "_"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({1, 6}));
  }

  SECTION("Affects(_, s2)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<int>({3, 4, 7}));
  }

  SECTION("Affects(s1, 7)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({6}));
  }

  SECTION("Affects(s1, 4)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({1}));
  }

  SECTION("Affects(1, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({3, 4}));
  }

  SECTION("Affects(6, s2)") {
    Param left = {ParamType::INTEGER_LITERAL, "6"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto results = ae.evaluateStmtAffects(left, right);
    REQUIRE(results == unordered_set<STMT_NO>({7}));
  }

  /* Pair results ------------------------------------------- */
  SECTION("Affects(s1, s2)") {
    auto results = ae.evaluatePairAffects();
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 3})));
    REQUIRE_THAT(results, VectorContains(vector<int>({1, 4})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({3, 4})));
    REQUIRE_THAT(results, VectorContains(vector<int>({6, 7})));
    REQUIRE_THAT(results, !VectorContains(vector<int>({6, 6})));
  }
}

TEST_CASE("AffectsEvaluator: Affects, Test Call & Read Stmts") {
  PKB* pkb = new PKB();
  pkb->insertAt(TableType::PROC_TABLE, "A");
  pkb->addFirstStmtOfProc("A", 1);
  pkb->insertAt(TableType::PROC_TABLE, "B");
  pkb->addFirstStmtOfProc("B", 6);
  pkb->insertAt(TableType::PROC_TABLE, "C");
  pkb->addFirstStmtOfProc("C", 9);
  for (int i = 1; i <= 9; i++) {
    pkb->addStmt(i);
  }

  pkb->addNext(1, 2);
  pkb->addNext(2, 3);
  pkb->addNext(3, 4);
  pkb->addNext(3, 5);
  pkb->addNext(6, 7);
  pkb->addNext(7, 8);
  pkb->addNext(8, 6);

  pkb->addWhileStmt(6);
  pkb->addIfStmt(3);
  pkb->addReadStmt(2);
  pkb->addCalls(8, "B", "C");
  vector<int> assignStmts = {1, 4, 5, 7, 9};
  for (auto stmt : assignStmts) {
    pkb->addAssignStmt(stmt);
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

  AffectsEvaluator ae(pkb);

  // Test read stmt
  SECTION("Affects(1, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(2, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  // Test call stmt
  SECTION("Affects(7, 7)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  SECTION("Affects(7, 8)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::INTEGER_LITERAL, "8"};
    bool result = ae.evaluateBoolAffects(left, right);
    REQUIRE(result == false);
  }

  // Test all Affects generated
  SECTION("Affects(s1, s2)") {
    auto results = ae.evaluatePairAffects();
    REQUIRE(results.empty());
  }
}
