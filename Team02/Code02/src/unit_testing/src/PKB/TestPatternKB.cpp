#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;
using namespace Catch;

TEST_CASE("WHILE_PATTERN") {
  // Init PKB
  PKB db = PKB();
  RelationshipType rs = RelationshipType::PTT_WHILE;

  // Empty Results
  REQUIRE(db.getStmtsForVar(rs, 5) == unordered_set<int>());

  db.addPatternRs(rs, 1, "a");
  db.addPatternRs(rs, 2, "a");
  db.addPatternRs(rs, 3, "b");
  db.addPatternRs(rs, 4, "b");
  db.addPatternRs(rs, 5, "c");
  db.addPatternRs(rs, 5, "d");

  VarIdx aIndex = db.getIndexOf(TableType::VAR_TABLE, "a");
  VarIdx bIndex = db.getIndexOf(TableType::VAR_TABLE, "b");
  VarIdx cIndex = db.getIndexOf(TableType::VAR_TABLE, "c");
  VarIdx dIndex = db.getIndexOf(TableType::VAR_TABLE, "d");

  REQUIRE(db.isPatternRs(rs, 1, aIndex));
  REQUIRE(db.isPatternRs(rs, 2, aIndex));
  REQUIRE(db.isPatternRs(rs, 3, bIndex));
  REQUIRE(db.isPatternRs(rs, 4, bIndex));
  REQUIRE(db.isPatternRs(rs, 5, cIndex));
  REQUIRE(db.isPatternRs(rs, 5, dIndex));
  REQUIRE(!db.isPatternRs(rs, 1, bIndex));
  REQUIRE(!db.isPatternRs(rs, 5, bIndex));
  REQUIRE(!db.isPatternRs(rs, 16, aIndex));
  REQUIRE(!db.isPatternRs(RelationshipType::PTT_WHILE, 1, bIndex));

  // w("a", _) = {1, 2}
  REQUIRE(db.getStmtsForVar(rs, aIndex) == unordered_set({1, 2}));
  REQUIRE(db.getStmtsForVar(rs, bIndex) == unordered_set({3, 4}));
  REQUIRE(db.getStmtsForVar(rs, cIndex) == unordered_set({5}));
  REQUIRE(db.getStmtsForVar(rs, dIndex) == unordered_set({5}));
}

TEST_CASE("ASSIGNMENT PATTERN") {
  // Init PKB
  PKB db = PKB();
  RelationshipType rs = RelationshipType::PTT_ASSIGN_FULL_EXPR;
  REQUIRE(db.getStmtsForVarAndExpr(rs, 0, "bellow").empty());
  REQUIRE(db.getStmtsForVar(rs, 1).empty());
  REQUIRE(db.getVarsForExpr(rs, "").empty());

  db.addPatternRs(rs, 1, "y", "x+1");
  db.addPatternRs(rs, 2, "y", "x+1");
  db.addPatternRs(rs, 3, "v", "x+1");
  db.addPatternRs(rs, 4, "v", "x+2");

  VarIdx yIndex = db.getIndexOf(TableType::VAR_TABLE, "y");
  VarIdx vIndex = db.getIndexOf(TableType::VAR_TABLE, "v");
  VarIdx aIndex = db.getIndexOf(TableType::VAR_TABLE, "a");

  REQUIRE(db.isPatternRs(rs, 1, yIndex, "x+1"));
  REQUIRE(db.isPatternRs(rs, 2, yIndex, "x+1"));
  REQUIRE(db.isPatternRs(rs, 3, vIndex, "x+1"));
  REQUIRE(db.isPatternRs(rs, 4, vIndex, "x+2"));

  REQUIRE(!db.isPatternRs(rs, 5, vIndex, "x+2"));
  REQUIRE(!db.isPatternRs(rs, 2, vIndex, "x"));
  REQUIRE(!db.isPatternRs(rs, 3, aIndex, "x+1"));
  REQUIRE(!db.isPatternRs(RelationshipType::PTT_IF, 4, vIndex, "x+2"));

  // a (_, "x+1") = {1, 2, 3}
  REQUIRE(db.getVarsForExpr(rs, "x+1") == unordered_set({yIndex, vIndex}));

  // a (yIndex, "x+1") = {1, 2}
  REQUIRE(db.getStmtsForVarAndExpr(rs, yIndex, "x+1") == unordered_set({1, 2}));

  // a (v, "x+1") = { {1, index(y)}, {2, index(y)}, {3, index(v)} }
  int yIdx = db.getIndexOf(TableType::VAR_TABLE, "y");
  int vIdx = db.getIndexOf(TableType::VAR_TABLE, "v");

  // a ("y",_) = {1, 2}, a("v",_) = {3, 4}
  REQUIRE(db.getStmtsForVar(rs, yIndex) == unordered_set({1, 2}));
  REQUIRE(db.getStmtsForVar(rs, vIndex) == unordered_set({3, 4}));

  // Invalid Queries

  REQUIRE(db.getStmtsForVarAndExpr(rs, yIndex, "bellow").empty());
  REQUIRE(db.getStmtsForVar(rs, 20).empty());
  REQUIRE(db.getVarsForExpr(rs, "bad").empty());
}
