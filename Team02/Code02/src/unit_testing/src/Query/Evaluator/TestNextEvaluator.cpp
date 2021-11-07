#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Evaluator/NextEvaluator.h>

#include <iostream>

#include "catch.hpp"

using namespace std;
using namespace query;
using Catch::Matchers::VectorContains;

TEST_CASE("NextEvaluator: Next") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 7; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // example procedure
  // 1: x = 1;
  // 2: if (z == 4) then {
  // 3:   z = 5; }
  // 4: else { z = 6; }
  // 5: while (z == 2) {
  // 6:  z = 3 }
  // 7: x = 5;
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 2, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 6);
  pkb->addRs(RelationshipType::NEXT, 5, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 5);

  RelationshipType rsType = RelationshipType::NEXT;

  NextEvaluator ne(pkb);

  SECTION("Next(1, 2)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = ne.evaluateBoolNextNextBip(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Next(1, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextNextBip(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("Next(s, 5)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    unordered_set<int> result = ne.evaluateNextNextBip(rsType, left, right);
    REQUIRE(result == unordered_set<int>({3, 4, 6}));
  }

  SECTION("Next(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ne.evaluatePairNextNextBip(rsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(result, VectorContains(vector<int>({1, 2})));
    REQUIRE_THAT(result, VectorContains(vector<int>({2, 3})));
    REQUIRE_THAT(result, VectorContains(vector<int>({2, 4})));
    REQUIRE_THAT(result, VectorContains(vector<int>({3, 5})));
    REQUIRE_THAT(result, VectorContains(vector<int>({4, 5})));
    REQUIRE_THAT(result, VectorContains(vector<int>({5, 6})));
    REQUIRE_THAT(result, VectorContains(vector<int>({5, 7})));
    REQUIRE_THAT(result, VectorContains(vector<int>({6, 5})));
  }
}

TEST_CASE("NextEvaluator: NextT - No Nested If/While") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 7; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // example procedure
  // 1: x = 1;
  // 2: if (z == 4) then {
  // 3:   z = 5; }
  // 4: else { z = 6; }
  // 5: while (z == 2) {
  // 6:  z = 3 }
  // 7: x = 5;
  pkb->addRs(RelationshipType::NEXT, 1, 2);
  pkb->addRs(RelationshipType::NEXT, 2, 3);
  pkb->addRs(RelationshipType::NEXT, 2, 4);
  pkb->addRs(RelationshipType::NEXT, 3, 5);
  pkb->addRs(RelationshipType::NEXT, 4, 5);
  pkb->addRs(RelationshipType::NEXT, 5, 6);
  pkb->addRs(RelationshipType::NEXT, 5, 7);
  pkb->addRs(RelationshipType::NEXT, 6, 5);

  RelationshipType rsType = RelationshipType::NEXT_T;

  NextEvaluator ne(pkb);

  /* Integer and Integer --------------------------- */
  SECTION("NextT(1, 3)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(3, 5)") {
    Param left = {ParamType::INTEGER_LITERAL, "3"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(5, 5)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(6, 7)") {
    Param left = {ParamType::INTEGER_LITERAL, "6"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(3, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "3"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == false);
  }

  /* Integer and Wildcard -------------------------- */
  SECTION("NextT(7, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("NextT(_, 7)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(_, 1)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == false);
  }

  /* Wildcard and Wildcard ------------------------- */
  SECTION("NextT(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  /* Integer and Synonym --------------------------- */
  SECTION("NextT(5, s)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::SYNONYM, "s"};
    unordered_set<int> result = ne.evaluateNextTNextBipT(rsType, left, right);
    REQUIRE(result == unordered_set<int>({5, 6, 7}));
  }

  SECTION("NextT(s, 7)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::INTEGER_LITERAL, "7"};
    unordered_set<int> result = ne.evaluateNextTNextBipT(rsType, left, right);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4, 5, 6}));
  }

  /* Wildcard and Synonym -------------------------- */
  SECTION("NextT(s, _)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ne.evaluatePairNextTNextBipT(rsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    for (int i = 1; i < 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({i})));
    }
    REQUIRE_THAT(result, !VectorContains(vector<int>({7})));
  }

  SECTION("NextT(_, s)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s"};
    auto setOfResults = ne.evaluatePairNextTNextBipT(rsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    for (int i = 2; i <= 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({i})));
    }
    REQUIRE_THAT(result, !VectorContains(vector<int>({1})));
  }

  /* Synonym and Synonym --------------------------- */
  SECTION("NextT(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ne.evaluatePairNextTNextBipT(rsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    // check NextT(1, _)
    for (int i = 2; i <= 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({1, i})));
    }
    REQUIRE_THAT(result, !VectorContains(vector<int>({1, 1})));
    // check NextT(2, _)
    for (int i = 3; i <= 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({2, i})));
    }
    // check NextT(3, _)
    for (int i = 5; i <= 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({3, i})));
    }
    // check NextT(4, _)
    for (int i = 5; i <= 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({4, i})));
    }
    // check NextT(5, _)
    for (int i = 5; i <= 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({5, i})));
    }
    // check NextT(6, _)
    for (int i = 5; i <= 7; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({6, i})));
    }
  }
}

TEST_CASE("NextEvaluator: NextT - Nested If/While") {
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

  RelationshipType rsType = RelationshipType::NEXT_T;

  NextEvaluator ne(pkb);

  /* Integer and Integer --------------------------- */
  SECTION("NextT(2, 2)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "2"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(4, 5)") {
    Param left = {ParamType::INTEGER_LITERAL, "4"};
    Param right = {ParamType::INTEGER_LITERAL, "5"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(5, 4)") {
    Param left = {ParamType::INTEGER_LITERAL, "5"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(4, 1)") {
    Param left = {ParamType::INTEGER_LITERAL, "4"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("NextT(4, 6)") {
    Param left = {ParamType::INTEGER_LITERAL, "4"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(6, 8)") {
    Param left = {ParamType::INTEGER_LITERAL, "6"};
    Param right = {ParamType::INTEGER_LITERAL, "8"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(7, 10)") {
    Param left = {ParamType::INTEGER_LITERAL, "7"};
    Param right = {ParamType::INTEGER_LITERAL, "10"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == false);
  }

  SECTION("NextT(11, 11)") {
    Param left = {ParamType::INTEGER_LITERAL, "11"};
    Param right = {ParamType::INTEGER_LITERAL, "11"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(13, 11)") {
    Param left = {ParamType::INTEGER_LITERAL, "13"};
    Param right = {ParamType::INTEGER_LITERAL, "11"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  /* Integer and Wildcard -------------------------- */
  SECTION("NextT(2, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextT(14, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "14"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == false);
  }

  /* Wildcard and Wildcard ------------------------- */
  SECTION("NextT(_, _)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextTNextBipT(rsType, left, right);
    REQUIRE(result == true);
  }

  /* Integer and Synonym --------------------------- */
  SECTION("NextT(1, s)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    unordered_set<int> result = ne.evaluateNextTNextBipT(rsType, left, right);
    REQUIRE(result ==
            unordered_set<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
  }

  SECTION("NextT(2, s)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    unordered_set<int> result = ne.evaluateNextTNextBipT(rsType, left, right);
    REQUIRE(result ==
            unordered_set<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
  }

  SECTION("NextT(4, s)") {
    Param left = {ParamType::INTEGER_LITERAL, "4"};
    Param right = {ParamType::SYNONYM, "s"};
    unordered_set<int> result = ne.evaluateNextTNextBipT(rsType, left, right);
    REQUIRE(result ==
            unordered_set<int>({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
  }

  SECTION("NextT(s, 4)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::INTEGER_LITERAL, "4"};
    unordered_set<int> result = ne.evaluateNextTNextBipT(rsType, left, right);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4, 5}));
  }

  SECTION("NextT(s, 1)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::INTEGER_LITERAL, "1"};
    unordered_set<int> result = ne.evaluateNextTNextBipT(rsType, left, right);
    REQUIRE(result.empty());
  }

  /* Wildcard and Synonym -------------------------- */
  SECTION("NextT(s, _)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::WILDCARD, "_"};
    auto setOfResults = ne.evaluatePairNextTNextBipT(rsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    for (int i = 1; i < 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({i})));
    }
    REQUIRE_THAT(result, !VectorContains(vector<int>({14})));
  }

  SECTION("NextT(_, s)") {
    Param left = {ParamType::WILDCARD, "_"};
    Param right = {ParamType::SYNONYM, "s"};
    auto setOfResults = ne.evaluatePairNextTNextBipT(rsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({i})));
    }
    REQUIRE_THAT(result, !VectorContains(vector<int>({1})));
  }

  /* Synonym and Synonym --------------------------- */
  SECTION("NextT(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ne.evaluatePairNextTNextBipT(rsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());

    // check NextT(1, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({1, i})));
    }
    REQUIRE_THAT(result, !VectorContains(vector<int>({1, 1})));
    // check NextT(2, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({2, i})));
    }
    // check NextT(3, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({3, i})));
    }
    // check NextT(4, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({4, i})));
    }
    // check NextT(5, _)
    for (int i = 2; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({5, i})));
    }
    // check NextT(6, _)
    for (int i = 7; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({6, i})));
    }
    // check NextT(7, _)
    for (int i = 8; i <= 14; i++) {
      if (i == 10) {
        REQUIRE_THAT(result, !VectorContains(vector<int>({7, i})));
      } else {
        REQUIRE_THAT(result, VectorContains(vector<int>({7, i})));
      }
    }
    // check NextT(8, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({8, i})));
    }
    // check NextT(9, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({9, i})));
    }
    // check NextT(10, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({10, i})));
    }
    // check NextT(11, _)
    for (int i = 11; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({11, i})));
    }
    // check NextT(12, _)
    for (int i = 12; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({12, i})));
    }
    // check NextT(13, _)
    for (int i = 13; i <= 14; i++) {
      REQUIRE_THAT(result, VectorContains(vector<int>({13, i})));
    }
  }
}

TEST_CASE("NextEvaluator: NextBip & NextBipT") {
  PKB* pkb = new PKB();
  for (int i = 1; i <= 6; i++) {
    pkb->addStmt(DesignEntity::STATEMENT, i);
  }
  // proc A {
  // 1: x = 1;
  // 2: x = 5; }
  // proc B {
  // 3: if (x == 2) then {
  // 4:   read x; }
  // 5: else { x = 3; } }
  // proc C {
  // 6: y = 2; }

  RelationshipType nextBipRsType = RelationshipType::NEXT_BIP;
  RelationshipType nextBipTRsType = RelationshipType::NEXT_BIP_T;

  pkb->addRs(nextBipRsType, 1, 2);
  pkb->addRs(nextBipRsType, 2, 3);
  pkb->addRs(nextBipRsType, 3, 4);
  pkb->addRs(nextBipRsType, 3, 5);
  pkb->addRs(nextBipRsType, 4, 6);
  pkb->addRs(nextBipRsType, 5, 6);

  NextEvaluator ne(pkb);

  /* NextBip --------------------------------------------------------- */
  SECTION("NextBip(2, 3))") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    bool result = ne.evaluateBoolNextNextBip(nextBipRsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextBip(2, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextNextBip(nextBipRsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextBip(s, 6)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    unordered_set<int> result =
        ne.evaluateNextNextBip(nextBipRsType, left, right);
    REQUIRE(result == unordered_set<int>({4, 5}));
  }

  SECTION("NextBip(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults = ne.evaluatePairNextNextBip(nextBipRsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    REQUIRE_THAT(result, VectorContains(vector<int>({1, 2})));
    REQUIRE_THAT(result, VectorContains(vector<int>({2, 3})));
    REQUIRE_THAT(result, VectorContains(vector<int>({3, 4})));
    REQUIRE_THAT(result, VectorContains(vector<int>({4, 6})));
    REQUIRE_THAT(result, VectorContains(vector<int>({5, 6})));
  }

  /* NextBip* -------------------------------------------------------- */
  SECTION("NextBipT(1, 3))") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::INTEGER_LITERAL, "3"};
    bool result = ne.evaluateBoolNextTNextBipT(nextBipTRsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextBipT(2, _)") {
    Param left = {ParamType::INTEGER_LITERAL, "2"};
    Param right = {ParamType::WILDCARD, "_"};
    bool result = ne.evaluateBoolNextTNextBipT(nextBipTRsType, left, right);
    REQUIRE(result == true);
  }

  SECTION("NextBipT(s, 6)") {
    Param left = {ParamType::SYNONYM, "s"};
    Param right = {ParamType::INTEGER_LITERAL, "6"};
    unordered_set<int> result =
        ne.evaluateNextTNextBipT(nextBipTRsType, left, right);
    REQUIRE(result == unordered_set<int>({1, 2, 3, 4, 5}));
  }

  SECTION("NextBipT(1, s)") {
    Param left = {ParamType::INTEGER_LITERAL, "1"};
    Param right = {ParamType::SYNONYM, "s"};
    unordered_set<int> result =
        ne.evaluateNextTNextBipT(nextBipTRsType, left, right);
    REQUIRE(result == unordered_set<int>({2, 3, 4, 5, 6}));
  }

  SECTION("NextBipT(s1, s2)") {
    Param left = {ParamType::SYNONYM, "s1"};
    Param right = {ParamType::SYNONYM, "s2"};
    auto setOfResults =
        ne.evaluatePairNextTNextBipT(nextBipTRsType, left, right);
    vector<vector<int>> result(setOfResults.begin(), setOfResults.end());
    for (int i = 1; i < 5; i++) {
      for (int j = i + 1; j <= 6; j++) {
        if (!(i == 4 && j == 5)) {
          REQUIRE_THAT(result, VectorContains(vector<int>({i, j})));
        }
      }
    }
    REQUIRE_THAT(result, !VectorContains(vector<int>({4, 5})));
  }
}
