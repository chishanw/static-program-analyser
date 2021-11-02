#include <set>
#include <vector>

#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("Modifies_P/ Uses_P") {
  PKB db = PKB();
  RelationshipType rs = RelationshipType::MODIFIES_P;
  TableType leftType = TableType::PROC_TABLE;
  TableType rightType = TableType::VAR_TABLE;

  PROC_NAME a = "a";
  PROC_NAME b = "b";
  PROC_NAME c = "c";

  // invalid queries when ModifiesKB and VarTable are empty
  unordered_set<int> emptySet({});
  unordered_set<std::vector<int>, VectorHash> emptyAllPairs({});

  REQUIRE(db.getMappings(rs, ParamPosition::BOTH) == emptyAllPairs);
  REQUIRE(db.getRight(rs, leftType, "a") == emptySet);
  REQUIRE(db.getLeft(rs, rightType, "a") == emptySet);
  REQUIRE(db.isRs(rs, leftType, "a", rightType, "b") == false);

  // valid insertions
  // x has varIdx 0, y 1, z 2
  db.addRs(rs, leftType, a, rightType, "x");
  db.addRs(rs, leftType, b, rightType, "x");
  db.addRs(rs, leftType, b, rightType, "y");
  db.addRs(rs, leftType, c, rightType, "x");
  db.addRs(rs, leftType, c, rightType, "y");
  db.addRs(rs, leftType, c, rightType, "z");

  // valid queries
  REQUIRE(db.isRs(rs, leftType, c, rightType, "z"));

  unordered_set<int> answer1({0});
  unordered_set<int> answer2({0, 1});
  unordered_set<int> answer3({0, 1, 2});
  REQUIRE(db.getRight(rs, leftType, a) == answer1);
  REQUIRE(db.getRight(rs, leftType, b) == answer2);
  REQUIRE(db.getRight(rs, leftType, c) == answer3);

  unordered_set<int> answerX({0, 1, 2});
  unordered_set<int> answerY({1, 2});
  unordered_set<int> answerZ({2});
  REQUIRE(db.getLeft(rs, rightType, "x") == answerX);
  REQUIRE(db.getLeft(rs, rightType, "y") == answerY);
  REQUIRE(db.getLeft(rs, rightType, "z") == answerZ);

  SECTION("Left param") {
    unordered_set<vector<int>, VectorHash> answerAllPairs(
        {vector<int>({0}), vector<int>({1}), vector<int>({2})});
    auto output = db.getMappings(rs, ParamPosition::LEFT);
    REQUIRE(output == answerAllPairs);
  }

  SECTION("Right params") {
    unordered_set<vector<int>, VectorHash> answerAllPairs(
        {vector<int>({0}), vector<int>({1}), vector<int>({2})});
    auto output = db.getMappings(rs, ParamPosition::RIGHT);
    REQUIRE(output == answerAllPairs);
  }

  SECTION("Both params") {
    unordered_set<vector<int>, VectorHash> answerAllPairs(
        {vector<int>({0, 0}), vector<int>({1, 0}), vector<int>({1, 1}),
         vector<int>({2, 0}), vector<int>({2, 1}), vector<int>({2, 2})});
    auto output = db.getMappings(rs, ParamPosition::BOTH);
    REQUIRE(output == answerAllPairs);
  }

  // invalid queries
  REQUIRE(db.isRs(rs, leftType, b, rightType, "a") == false);
  REQUIRE(db.getLeft(rs, rightType, "f") == emptySet);
  REQUIRE(db.getRight(rs, leftType, "f") == emptySet);
}
