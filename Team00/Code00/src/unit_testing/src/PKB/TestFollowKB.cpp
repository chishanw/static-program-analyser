#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("FOLLOWS_TABLE") {
  // Init
  PKB db = PKB();

  // set follows
  // Successful Insertion, return true
  db.setFollows(1, 2);
  db.setFollows(2, 3);
  db.setFollows(4, 5);

  // get follows
  REQUIRE(db.getFollows(1) == 2);
  REQUIRE(db.getFollows(2) == 3);
  REQUIRE(db.getFollows(4) == 5);
  // No Key, return -1
  REQUIRE(db.getFollows(3) == -1);

  // get follows by
  REQUIRE(db.getFollowedBy(2) == 1);
  REQUIRE(db.getFollowedBy(3) == 2);
  REQUIRE(db.getFollowedBy(5) == 4);
  // No Key, return -1
  REQUIRE(db.getFollowedBy(1) == -1);

  // get all follow statement pairs
  auto expected = std::vector({
      LIST_STMT_NO({1, 2}),
      LIST_STMT_NO({2, 3}),
      LIST_STMT_NO({4, 5}),
  });

  auto actual = db.getAllFollowsStmtPairs();

  set<LIST_STMT_NO> actual_set(actual.begin(), actual.end());
  set<LIST_STMT_NO> expected_set(expected.begin(), expected.end());
  REQUIRE(actual_set == expected_set);
}

TEST_CASE("FOLLOWS_TABLE_T") {
  // Init
  PKB db = PKB();
  // set followsT
  // Successful Insertion, return true
  db.addFollowsT(1, 2);
  db.addFollowsT(1, 3);
  db.addFollowsT(4, 5);
  db.addFollowsT(2, 5);
  // Key exists, return true
  db.addFollowsT(1, 3);
  db.addFollowsT(4, 5);

  // Is follows
  REQUIRE(db.isFollowsT(1, 2) == true);
  REQUIRE(db.isFollowsT(1, 3) == true);
  REQUIRE(db.isFollowsT(4, 5) == true);
  REQUIRE(db.isFollowsT(2, 1) == false);
  REQUIRE(db.isFollowsT(3, 1) == false);

  // Get Follows
  // Key Exists, return list
  REQUIRE(db.getFollowsT(1) == UNO_SET_OF_STMT_NO({2, 3}));
  REQUIRE(db.getFollowsT(4) == UNO_SET_OF_STMT_NO({5}));
  // No Key, return empty list
  REQUIRE(db.getFollowsT(10) == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getFollowsT(9) == UNO_SET_OF_STMT_NO({}));

  // Get Follows
  // Key Exists
  REQUIRE(db.getFollowedTBy(2) == UNO_SET_OF_STMT_NO({1}));
  REQUIRE(db.getFollowedTBy(3) == UNO_SET_OF_STMT_NO({1}));
  REQUIRE(db.getFollowedTBy(5) == UNO_SET_OF_STMT_NO({4, 2}));

  // No Key, return empty list
  REQUIRE(db.getFollowedTBy(1) == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getFollowedTBy(9) == UNO_SET_OF_STMT_NO({}));

  // get all followT statement pairs
  auto expected = std::vector({std::pair(1, LIST_STMT_NO({2, 3})),
                               std::pair(4, LIST_STMT_NO({5})),
                               std::pair(2, LIST_STMT_NO({5}))});

  auto actual = db.getAllFollowsTStmtPairs();
  REQUIRE(set<std::pair<STMT_NO, LIST_STMT_NO>>(
              actual.begin(), actual.end()) ==
          set<std::pair<STMT_NO, LIST_STMT_NO>>(
              expected.begin(), expected.end()));
}
