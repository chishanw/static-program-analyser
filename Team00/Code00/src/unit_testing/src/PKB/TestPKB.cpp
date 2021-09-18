#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("GET_ALL_STATEMENTS_TEST") {
  // Init
  PKB db = PKB();

  db.addReadStmt(1);
  db.addPrintStmt(2);
  db.addCallStmt(3);
  db.addWhileStmt(4);
  db.addIfStmt(5);
  db.addAssignStmt(6);

  // Boolean positive
  REQUIRE(db.isReadStmt(1));
  REQUIRE(db.isPrintStmt(2));
  REQUIRE(db.isCallStmt(3));
  REQUIRE(db.isWhileStmt(4));
  REQUIRE(db.isIfStmt(5));
  REQUIRE(db.isAssignStmt(6));

  // Boolean false
  REQUIRE(!db.isReadStmt(2));
  REQUIRE(!db.isPrintStmt(10));
  REQUIRE(!db.isCallStmt(7));
  REQUIRE(!db.isWhileStmt(5));
  REQUIRE(!db.isIfStmt(4));
  REQUIRE(!db.isAssignStmt(3));

  // Check if set is correct
  REQUIRE(db.getAllStmts() == UNO_SET_OF_STMT_NO({1, 2, 3, 4, 5, 6}));
  REQUIRE(db.getAllReadStmts() == UNO_SET_OF_STMT_NO({1}));
  REQUIRE(db.getAllPrintStmts() == UNO_SET_OF_STMT_NO({2}));
  REQUIRE(db.getAllCallStmts() == UNO_SET_OF_STMT_NO({3}));
  REQUIRE(db.getAllWhileStmts() == UNO_SET_OF_STMT_NO({4}));
  REQUIRE(db.getAllIfStmts() == UNO_SET_OF_STMT_NO({5}));
  REQUIRE(db.getAllAssignStmts() == UNO_SET_OF_STMT_NO({6}));
}

TEST_CASE("GET_ALL_STATEMENTS_EMPTY_TEST") {
  // Init
  PKB db = PKB();
  REQUIRE(db.getAllStmts() == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getAllReadStmts() == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getAllPrintStmts() == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getAllCallStmts() == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getAllWhileStmts() == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getAllIfStmts() == UNO_SET_OF_STMT_NO({}));
  REQUIRE(db.getAllAssignStmts() == UNO_SET_OF_STMT_NO({}));
}

TEST_CASE("PKB_FOLLOW_API_TEST") {
  // Init
  PKB db = PKB();
  REQUIRE(db.getAllStmts() == UNO_SET_OF_STMT_NO({}));

  for (int s1 = 1; s1 < 6; s1++) {
    db.addStmt(s1);
  }

  for (int s1 = 1; s1 < 5; s1++) {
    db.setFollows(s1, s1 + 1);
    for (int s2 = s1; s2 < 5; s2++) {
      db.addFollowsT(s1, s2 + 1);
    }
  }

  // Check relationship
  REQUIRE(db.getAllStmts() == UNO_SET_OF_STMT_NO({1, 2, 3, 4, 5}));
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

TEST_CASE("PKB_CONSTANTS_TEST") {
  PKB db = PKB();
  REQUIRE(db.getAllConstants() == unordered_set<int>());

  db.insertConst("1");
  db.insertConst("2");
  db.insertConst("3");
  REQUIRE(db.getAllConstants() == unordered_set<CONST_IDX>({ 0, 1, 2 }));

  // duplicate insert
  db.insertConst("2");
  REQUIRE(db.getAllConstants() == unordered_set<CONST_IDX>({ 0, 1, 2 }));
}

TEST_CASE("PKB_TABLES_TEST") {
  PKB db = PKB();

  // invalid queries when empty
  REQUIRE(db.getProcName(1) == "");
  REQUIRE(db.getAllProcedures() == unordered_set<int>());
  REQUIRE(db.getVarName(1) == "");
  REQUIRE(db.getAllVariables() == unordered_set<int>());

  db.insertProc("ComputeCentroid");
  db.insertProc("main");
  db.addUsesS(1, "a");
  db.addUsesS(2, "b");

  // valid queries
  unordered_set<int> answer({ 0, 1 });
  REQUIRE(db.getProcName(0) == "ComputeCentroid");
  REQUIRE(db.getAllProcedures() == answer);
  REQUIRE(db.getVarName(1) == "b");
  REQUIRE(db.getAllVariables() == answer);

  // duplicate entries
  db.insertProc("main");
  db.addUsesS(1, "a");

  REQUIRE(db.getProcName(1) == "main");
  REQUIRE(db.getAllProcedures() == answer);
  REQUIRE(db.getVarName(0) == "a");
  REQUIRE(db.getAllVariables() == answer);
}
