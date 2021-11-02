#include <set>
#include <vector>

#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("StmtName_Relationship") {
  PKB db = PKB();
  RelationshipType rsList[] = {RelationshipType::MODIFIES_S,
                               RelationshipType::USES_S};
  TableType tableType = TableType::VAR_TABLE;

  // invalid queries when ModifiesKB and VarTable are empty
  unordered_set<int> emptySet({});
  unordered_set<std::vector<int>, VectorHash> emptyAllPairs({});

  for (auto rs : rsList) {
    REQUIRE(db.getMappings(rs, ParamPosition::BOTH) == emptyAllPairs);
    REQUIRE(db.getRight(rs, 1) == emptySet);
    REQUIRE(db.getLeft(rs, tableType, "a") == emptySet);
    REQUIRE(db.isRs(rs, 2, tableType, "b") == false);
  }

  // valid insertions
  // x has varIdx 0, y 1, z 2
  for (auto rs : rsList) {
    db.addRs(rs, 1, tableType, "x");
    db.addRs(rs, 2, tableType, "x");
    db.addRs(rs, 2, tableType, "y");
    db.addRs(rs, 3, tableType, "x");
    db.addRs(rs, 3, tableType, "y");
    db.addRs(rs, 3, tableType, "z");
  }

  // valid queries
  for (auto rs : rsList) {
    REQUIRE(db.isRs(rs, 3, tableType, "z"));

    unordered_set<int> answer1({0});
    unordered_set<int> answer2({0, 1});
    unordered_set<int> answer3({0, 1, 2});
    REQUIRE(db.getRight(rs, 1) == answer1);
    REQUIRE(db.getRight(rs, 2) == answer2);
    REQUIRE(db.getRight(rs, 3) == answer3);

    unordered_set<int> answerX({1, 2, 3});
    unordered_set<int> answerY({2, 3});
    unordered_set<int> answerZ({3});
    REQUIRE(db.getLeft(rs, tableType, "x") == answerX);
    REQUIRE(db.getLeft(rs, tableType, "y") == answerY);
    REQUIRE(db.getLeft(rs, tableType, "z") == answerZ);

    unordered_set<vector<int>, VectorHash> answerAllPairs(
        {vector<int>({1, 0}), vector<int>({2, 0}), vector<int>({2, 1}),
         vector<int>({3, 0}), vector<int>({3, 1}), vector<int>({3, 2})});

    auto output = db.getMappings(rs, ParamPosition::BOTH);
    REQUIRE(output == answerAllPairs);

    // invalid queries
    REQUIRE(db.isRs(rs, 1, tableType, "a") == false);
    REQUIRE(db.getLeft(rs, tableType, "b") == emptySet);
    REQUIRE(db.getRight(rs, 30) == emptySet);
  }
}
