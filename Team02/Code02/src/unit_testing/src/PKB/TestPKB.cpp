#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("GET_ALL_STATEMENTS_TEST") {
  // Init
  PKB db = PKB();

  db.addStmt(DesignEntity::READ, 1);
  db.addStmt(DesignEntity::PRINT, 2);
  db.addCalls(3, "test1", "test2");
  db.addStmt(DesignEntity::WHILE, 4);
  db.addStmt(DesignEntity::IF, 5);
  db.addStmt(DesignEntity::ASSIGN, 6);

  // Boolean positive
  REQUIRE(db.isStmt(DesignEntity::READ, 1));
  REQUIRE(db.isStmt(DesignEntity::PRINT, 2));
  REQUIRE(db.isCallStmt(3));
  REQUIRE(db.isStmt(DesignEntity::WHILE, 4));
  REQUIRE(db.isStmt(DesignEntity::IF, 5));
  REQUIRE(db.isStmt(DesignEntity::ASSIGN, 6));

  // Boolean false
  REQUIRE(!db.isStmt(DesignEntity::READ, 2));
  REQUIRE(!db.isStmt(DesignEntity::PRINT, 10));
  REQUIRE(!db.isCallStmt(7));
  REQUIRE(!db.isStmt(DesignEntity::WHILE, 5));
  REQUIRE(!db.isStmt(DesignEntity::IF, 4));
  REQUIRE(!db.isStmt(DesignEntity::ASSIGN, 3));

  // Check if set is correct
  REQUIRE(db.getAllStmts(DesignEntity::STATEMENT) ==
          SetOfStmts({1, 2, 3, 4, 5, 6}));
  REQUIRE(db.getAllStmts(DesignEntity::READ) == SetOfStmts({1}));
  REQUIRE(db.getAllStmts(DesignEntity::PRINT) == SetOfStmts({2}));
  REQUIRE(db.getAllCallStmts() == SetOfStmts({3}));
  REQUIRE(db.getAllStmts(DesignEntity::WHILE) == SetOfStmts({4}));
  REQUIRE(db.getAllStmts(DesignEntity::IF) == SetOfStmts({5}));
  REQUIRE(db.getAllStmts(DesignEntity::ASSIGN) == SetOfStmts({6}));
}

TEST_CASE("GET_ALL_STATEMENTS_EMPTY_TEST") {
  // Init
  PKB db = PKB();
  REQUIRE(db.getAllStmts(DesignEntity::STATEMENT) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::READ) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::PRINT) == SetOfStmts({}));
  REQUIRE(db.getAllCallStmts() == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::WHILE) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::IF) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::ASSIGN) == SetOfStmts({}));
}

TEST_CASE("PKB_CONSTANTS_TEST") {
  PKB db = PKB();
  REQUIRE(db.getAllElementsAt(TableType::CONST_TABLE) == unordered_set<int>());

  db.insertAt(TableType::CONST_TABLE, "1");
  db.insertAt(TableType::CONST_TABLE, "2");
  db.insertAt(TableType::CONST_TABLE, "3");
  REQUIRE(db.getAllElementsAt(TableType::CONST_TABLE) ==
          unordered_set<ConstIdx>({0, 1, 2}));

  // duplicate insert
  db.insertAt(TableType::CONST_TABLE, "2");
  REQUIRE(db.getAllElementsAt(TableType::CONST_TABLE) ==
          unordered_set<ConstIdx>({0, 1, 2}));
}

TEST_CASE("PKB_TABLES_TEST") {
  PKB db = PKB();

  // invalid queries when empty
  REQUIRE(db.getElementAt(TableType::PROC_TABLE, 1) == "");
  REQUIRE(db.getAllElementsAt(TableType::PROC_TABLE) == unordered_set<int>());
  REQUIRE(db.getElementAt(TableType::VAR_TABLE, 1) == "");
  REQUIRE(db.getAllElementsAt(TableType::VAR_TABLE) == unordered_set<int>());

  db.insertAt(TableType::PROC_TABLE, "ComputeCentroid");
  db.insertAt(TableType::PROC_TABLE, "main");
  db.addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "a");
  db.addRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "b");

  // valid queries
  unordered_set<int> answer({0, 1});
  REQUIRE(db.getElementAt(TableType::PROC_TABLE, 0) == "ComputeCentroid");
  REQUIRE(db.getAllElementsAt(TableType::PROC_TABLE) == answer);
  REQUIRE(db.getElementAt(TableType::VAR_TABLE, 1) == "b");
  REQUIRE(db.getAllElementsAt(TableType::VAR_TABLE) == answer);

  // duplicate entries
  db.insertAt(TableType::PROC_TABLE, "main");
  db.addRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "a");

  REQUIRE(db.getElementAt(TableType::PROC_TABLE, 1) == "main");
  REQUIRE(db.getAllElementsAt(TableType::PROC_TABLE) == answer);
  REQUIRE(db.getElementAt(TableType::VAR_TABLE, 0) == "a");
  REQUIRE(db.getAllElementsAt(TableType::VAR_TABLE) == answer);
}
