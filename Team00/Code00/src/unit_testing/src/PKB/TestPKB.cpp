#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("GET_ALL_STATEMENTS_TEST") {
  // Init
  PKB db = PKB();
  UNO_SET_OF_STMT_NO expected({});
  for (int i = 0; i < 1000; i++) {
    db.addStatement(i);
    expected.insert(i);
  }

  REQUIRE(db.getAllStatements() == expected);
}

TEST_CASE("GET_ALL_STATEMENTS_EMPTY_TEST") {
  // Init
  PKB db = PKB();
  REQUIRE(db.getAllStatements() == UNO_SET_OF_STMT_NO({}));
}

TEST_CASE("PKB_FOLLOW_API_TEST") {
  // Init
  PKB db = PKB();
  REQUIRE(db.getAllStatements() == UNO_SET_OF_STMT_NO({}));

  for (int s1 = 1; s1 < 6; s1++) {
    db.addStatement(s1);
  }

  for (int s1 = 1; s1 < 5; s1++) {
    db.setFollows(s1, s1 + 1);
    for (int s2 = s1; s2 < 5; s2++) {
      db.addFollowsT(s1, s2 + 1);
    }
  }

  // Check relationship
  REQUIRE(db.getAllStatements() == UNO_SET_OF_STMT_NO({1, 2, 3, 4, 5}));
  for (int s1 = 1; s1 < 5; s1++) {
    REQUIRE(db.getFollows(s1) == s1 + 1);
    UNO_SET_OF_STMT_NO expected({});
    for (int s2 = s1; s2 < 5; s2++) {
      expected.insert(s2 + 1);
      REQUIRE(db.isFollowsT(s1, s1 + 1) == true);
    }
    REQUIRE(db.getFollowsT(s1) == expected);
  }
}
