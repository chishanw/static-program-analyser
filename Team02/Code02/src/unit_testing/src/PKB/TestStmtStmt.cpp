#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("PKB_STMT_STMT_RELATIONSHIP") {
  // Init
  PKB db = PKB();
  RelationshipType rsNormal[3] = {RelationshipType::FOLLOWS,
                                  RelationshipType::PARENT,
                                  RelationshipType::NEXT};

  RelationshipType rsTransitive[2] = {RelationshipType::FOLLOWS_T,
                                      RelationshipType::PARENT_T};

  for (RelationshipType rs : rsNormal) {
    db.addRs(rs, 1, 2);
    db.addRs(rs, 2, 3);
    db.addRs(rs, 4, 5);
  }

  for (RelationshipType rs : rsTransitive) {
    db.addRs(rs, 1, 2);
    db.addRs(rs, 1, 3);
    db.addRs(rs, 4, 5);
    db.addRs(rs, 2, 5);
    // Insert Duplicates
    db.addRs(rs, 1, 3);
    db.addRs(rs, 4, 5);
  }

  SECTION("Normal Relationships") {
    for (RelationshipType rs : rsNormal) {
      REQUIRE(db.isRs(rs, 1, 2) == true);
      REQUIRE(db.isRs(rs, 2, 3) == true);
      REQUIRE(db.isRs(rs, 4, 5) == true);
      REQUIRE(db.isRs(rs, 2, 1) == false);
      REQUIRE(db.isRs(rs, 3, 1) == false);

      REQUIRE(db.getRight(rs, 1) == unordered_set<StmtNo>({2}));
      REQUIRE(db.getRight(rs, 2) == unordered_set<StmtNo>({3}));
      REQUIRE(db.getRight(rs, 4) == unordered_set<StmtNo>({5}));
      REQUIRE(db.getRight(rs, 10) == unordered_set<StmtNo>({}));
      REQUIRE(db.getRight(rs, 9) == unordered_set<StmtNo>({}));

      REQUIRE(db.getLeft(rs, 2) == unordered_set<StmtNo>({1}));
      REQUIRE(db.getLeft(rs, 3) == unordered_set<StmtNo>({2}));
      REQUIRE(db.getLeft(rs, 5) == unordered_set<StmtNo>({4}));
      REQUIRE(db.getLeft(rs, 1) == unordered_set<StmtNo>({}));

      SECTION("Check left param") {
        auto expected = unordered_set<vector<int>, VectorHash>(
            {vector<int>({1}), vector<int>({2}), vector<int>({4})});

        auto actual = db.getMappings(rs, ParamPosition::LEFT);
        REQUIRE(expected == actual);
      }
      SECTION("Check right param") {
        auto expected = unordered_set<vector<int>, VectorHash>(
            {vector<int>({2}), vector<int>({3}), vector<int>({5})});

        auto actual = db.getMappings(rs, ParamPosition::RIGHT);
        REQUIRE(expected == actual);
      }
      SECTION("Check both param") {
        auto expected = unordered_set<vector<int>, VectorHash>({
            vector<int>({1, 2}),
            vector<int>({2, 3}),
            vector<int>({4, 5}),
        });

        auto actual = db.getMappings(rs, ParamPosition::BOTH);
        REQUIRE(expected == actual);
      }
    }
  }

  SECTION("Transitive Relationships") {
    for (RelationshipType rs : rsTransitive) {
      REQUIRE(db.isRs(rs, 1, 2) == true);
      REQUIRE(db.isRs(rs, 1, 3) == true);
      REQUIRE(db.isRs(rs, 4, 5) == true);
      REQUIRE(db.isRs(rs, 2, 1) == false);
      REQUIRE(db.isRs(rs, 3, 1) == false);

      REQUIRE(db.getRight(rs, 1) == unordered_set<StmtNo>({2, 3}));
      REQUIRE(db.getRight(rs, 4) == unordered_set<StmtNo>({5}));
      REQUIRE(db.getRight(rs, 10) == unordered_set<StmtNo>({}));

      REQUIRE(db.getLeft(rs, 2) == unordered_set<StmtNo>({1}));
      REQUIRE(db.getLeft(rs, 3) == unordered_set<StmtNo>({1}));
      REQUIRE(db.getLeft(rs, 5) == unordered_set<StmtNo>({4, 2}));

      // No Key, return empty set
      REQUIRE(db.getLeft(rs, 1) == unordered_set<StmtNo>({}));
      REQUIRE(db.getLeft(rs, 9) == unordered_set<StmtNo>({}));

      SECTION("Check left param") {
        auto expected = unordered_set<vector<int>, VectorHash>(
            {vector<int>({1}), vector<int>({2}), vector<int>({4})});

        auto actual = db.getMappings(rs, ParamPosition::LEFT);
        REQUIRE(expected == actual);
      }
      SECTION("Check right param") {
        auto expected = unordered_set<vector<int>, VectorHash>(
            {vector<int>({2}), vector<int>({3}), vector<int>({5})});

        auto actual = db.getMappings(rs, ParamPosition::RIGHT);
        REQUIRE(expected == actual);
      }
      SECTION("Check both param") {
        auto expected = unordered_set<vector<int>, VectorHash>({
            vector<int>({1, 2}),
            vector<int>({1, 3}),
            vector<int>({4, 5}),
            vector<int>({2, 5}),
        });

        auto actual = db.getMappings(rs, ParamPosition::BOTH);
        REQUIRE(expected == actual);
      }
    }
  }
}
