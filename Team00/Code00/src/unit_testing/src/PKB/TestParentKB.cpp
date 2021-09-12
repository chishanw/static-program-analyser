#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("PARENT_TABLE") {
  // Init
  PKB db = PKB();

  // set Parent
  // Successful Insertion, return true
  db.setParent(1, 2);
  db.setParent(1, 6);
  db.setParent(2, 3);
  db.setParent(4, 5);

  // Is Parent
  REQUIRE(db.isParent(1, 2) == true);
  REQUIRE(db.isParent(1, 6) == true);
  REQUIRE(db.isParent(2, 3) == true);
  REQUIRE(db.isParent(2, 1) == false);
  REQUIRE(db.isParent(3, 1) == false);

  // get Children
  REQUIRE(db.getChildren(1) == UNO_SET_OF_STMT_NO({2, 6}));
  REQUIRE(db.getChildren(2) == UNO_SET_OF_STMT_NO({3}));
  REQUIRE(db.getChildren(4) == UNO_SET_OF_STMT_NO({5}));
  // No Key, return empty Set
  REQUIRE(db.getChildren(3) == UNO_SET_OF_STMT_NO({}));

  // get Parent
  REQUIRE(db.getParent(2) == 1);
  REQUIRE(db.getParent(3) == 2);
  REQUIRE(db.getParent(5) == 4);
  // No Key, return -1
  REQUIRE(db.getParent(1) == -1);

  // get all Parent statement pairs
  auto expected = std::vector({
      std::pair(1, LIST_STMT_NO({2, 6})),
      std::pair(2, LIST_STMT_NO({3})),
      std::pair(4, LIST_STMT_NO({5})),
  });

  auto actual = db.getAllParentsStmtPairs();

  // Placed into set to prevent macOS CI failing
  set<pair<STMT_NO, LIST_STMT_NO>> actual_set(actual.begin(), actual.end());
  set<pair<STMT_NO, LIST_STMT_NO>> expected_set(expected.begin(),
    expected.end());
  REQUIRE(actual_set == expected_set);
}

TEST_CASE("PARENT_TABLE_T") {
  // Init
  PKB db = PKB();
  // set ParentT
  // Successful Insertion, return true
  db.addParentT(1, 2);
  db.addParentT(1, 3);
  db.addParentT(4, 5);
  db.addParentT(2, 5);
  // Key exists, return true
  db.addParentT(1, 3);
  db.addParentT(4, 5);

  // Is Parent
  REQUIRE(db.isParentT(1, 2) == true);
  REQUIRE(db.isParentT(1, 3) == true);
  REQUIRE(db.isParentT(4, 5) == true);
  REQUIRE(db.isParentT(2, 1) == false);
  REQUIRE(db.isParentT(3, 1) == false);

  // Get Parents
  // Key Exists, return list
  REQUIRE(db.getChildrenT(1) == UNO_SET_OF_STMT_NO({2, 3}));
  REQUIRE(db.getChildrenT(4) == UNO_SET_OF_STMT_NO({5}));
  // No Key, return empty list
  REQUIRE(db.getChildrenT(10) == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getChildrenT(9) == UNO_SET_OF_STMT_NO({}));

  // Get Children
  // Key Exists
  REQUIRE(db.getParentsT(2) == UNO_SET_OF_STMT_NO({1}));
  REQUIRE(db.getParentsT(3) == UNO_SET_OF_STMT_NO({1}));
  REQUIRE(db.getParentsT(5) == UNO_SET_OF_STMT_NO({4, 2}));

  // No Key, return empty list
  REQUIRE(db.getParentsT(1) == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getParentsT(9) == UNO_SET_OF_STMT_NO({}));

  // get all ParentT statement pairs
  auto expected = std::vector({pair(1, LIST_STMT_NO({2, 3})),
                               pair(4, LIST_STMT_NO({5})),
                               pair(2, LIST_STMT_NO({5}))});

  auto actual = db.getAllParentsTStmtPairs();

  // Placed in to set to prevent macOS CI failing
  set<pair<STMT_NO, LIST_STMT_NO>> actual_set(actual.begin(), actual.end());
  set<pair<STMT_NO, LIST_STMT_NO>> expected_set(expected.begin(),
    expected.end());

  REQUIRE(actual_set == expected_set);
}
