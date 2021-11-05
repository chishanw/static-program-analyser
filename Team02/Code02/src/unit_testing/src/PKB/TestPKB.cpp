#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("GET_ALL_STATEMENTS_TEST") {
  // Init
  PKB db = PKB();

  db.addStmt(DesignEntity::READ, 1);
  db.addStmt(DesignEntity::PRINT, 2);
  db.addStmt(DesignEntity::CALL, 3);
  db.addRs(RelationshipType::CALLS, TableType::PROC_TABLE, "test1",
           TableType::PROC_TABLE, "test2");
  db.addRs(RelationshipType::CALLS_S, 3, TableType::PROC_TABLE, "test2");
  db.addStmt(DesignEntity::WHILE, 4);
  db.addStmt(DesignEntity::IF, 5);
  db.addStmt(DesignEntity::ASSIGN, 6);
  db.insertAt(TableType::VAR_TABLE, "varA");
  db.insertAt(TableType::CONST_TABLE, "1");

  // Boolean positive
  REQUIRE(db.isStmt(DesignEntity::READ, 1));
  REQUIRE(db.isStmt(DesignEntity::PRINT, 2));
  REQUIRE(db.isStmt(DesignEntity::CALL, 3));
  REQUIRE(db.isStmt(DesignEntity::WHILE, 4));
  REQUIRE(db.isStmt(DesignEntity::IF, 5));
  REQUIRE(db.isStmt(DesignEntity::ASSIGN, 6));
  REQUIRE(db.isStmt(DesignEntity::STATEMENT, 1));
  REQUIRE(db.isStmt(DesignEntity::STATEMENT, 2));
  REQUIRE(db.isStmt(DesignEntity::STATEMENT, 3));
  REQUIRE(db.isStmt(DesignEntity::STATEMENT, 4));
  REQUIRE(db.isStmt(DesignEntity::STATEMENT, 5));
  REQUIRE(db.isStmt(DesignEntity::STATEMENT, 6));
  REQUIRE(db.isStmt(DesignEntity::PROG_LINE, 1));
  REQUIRE(db.isStmt(DesignEntity::PROG_LINE, 2));
  REQUIRE(db.isStmt(DesignEntity::PROG_LINE, 3));
  REQUIRE(db.isStmt(DesignEntity::PROG_LINE, 4));
  REQUIRE(db.isStmt(DesignEntity::PROG_LINE, 5));
  REQUIRE(db.isStmt(DesignEntity::PROG_LINE, 6));

  // Boolean false
  REQUIRE(!db.isStmt(DesignEntity::READ, 2));
  REQUIRE(!db.isStmt(DesignEntity::PRINT, 10));
  REQUIRE(!db.isStmt(DesignEntity::CALL, 7));
  REQUIRE(!db.isStmt(DesignEntity::WHILE, 5));
  REQUIRE(!db.isStmt(DesignEntity::IF, 4));
  REQUIRE(!db.isStmt(DesignEntity::ASSIGN, 3));

  // Check if set is correct
  REQUIRE(db.getAllStmts(DesignEntity::STATEMENT) ==
          SetOfStmts({1, 2, 3, 4, 5, 6}));
  REQUIRE(db.getAllStmts(DesignEntity::READ) == SetOfStmts({1}));
  REQUIRE(db.getAllStmts(DesignEntity::PRINT) == SetOfStmts({2}));
  REQUIRE(db.getAllStmts(DesignEntity::CALL) == SetOfStmts({3}));
  REQUIRE(db.getAllStmts(DesignEntity::WHILE) == SetOfStmts({4}));
  REQUIRE(db.getAllStmts(DesignEntity::IF) == SetOfStmts({5}));
  REQUIRE(db.getAllStmts(DesignEntity::ASSIGN) == SetOfStmts({6}));

  // Check sizes
  REQUIRE(db.getNumEntity(DesignEntity::STATEMENT) == 6);
  REQUIRE(db.getNumEntity(DesignEntity::PROG_LINE) == 6);
  REQUIRE(db.getNumEntity(DesignEntity::READ) == 1);
  REQUIRE(db.getNumEntity(DesignEntity::PRINT) == 1);
  REQUIRE(db.getNumEntity(DesignEntity::CALL) == 1);
  REQUIRE(db.getNumEntity(DesignEntity::WHILE) == 1);
  REQUIRE(db.getNumEntity(DesignEntity::IF) == 1);
  REQUIRE(db.getNumEntity(DesignEntity::ASSIGN) == 1);
  REQUIRE(db.getNumEntity(DesignEntity::VARIABLE) == 1);
  REQUIRE(db.getNumEntity(DesignEntity::PROCEDURE) == 2);
  REQUIRE(db.getNumEntity(DesignEntity::CONSTANT) == 1);
}

TEST_CASE("GET_ALL_STATEMENTS_EMPTY_TEST") {
  // Init
  PKB db = PKB();
  REQUIRE(db.getAllStmts(DesignEntity::STATEMENT) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::READ) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::PRINT) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::CALL) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::WHILE) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::IF) == SetOfStmts({}));
  REQUIRE(db.getAllStmts(DesignEntity::ASSIGN) == SetOfStmts({}));

  REQUIRE_FALSE(db.isStmt(DesignEntity::STATEMENT, 1));
  REQUIRE(db.getNumEntity(DesignEntity::STATEMENT) == 0);
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
