#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;
using namespace Catch;

TEST_CASE("WHILE_PATTERN") {
  // Init PKB
  PKB db = PKB();

  // Empty Results
  REQUIRE(db.getVarMappings(RelationshipType::PTT_WHILE) == SetOfStmtLists());
  REQUIRE(db.getStmtsForVar(RelationshipType::PTT_WHILE, "d") ==
          unordered_set<int>());

  db.addPatternRs(RelationshipType::PTT_WHILE, 1, "a");
  db.addPatternRs(RelationshipType::PTT_WHILE, 2, "a");
  db.addPatternRs(RelationshipType::PTT_WHILE, 3, "b");
  db.addPatternRs(RelationshipType::PTT_WHILE, 4, "b");
  db.addPatternRs(RelationshipType::PTT_WHILE, 5, "c");
  db.addPatternRs(RelationshipType::PTT_WHILE, 5, "d");

  // w("a", _) = {1, 2}
  REQUIRE(db.getStmtsForVar(RelationshipType::PTT_WHILE, "a") ==
          unordered_set({1, 2}));
  REQUIRE(db.getStmtsForVar(RelationshipType::PTT_WHILE, "b") ==
          unordered_set({3, 4}));
  REQUIRE(db.getStmtsForVar(RelationshipType::PTT_WHILE, "c") ==
          unordered_set({5}));
  REQUIRE(db.getStmtsForVar(RelationshipType::PTT_WHILE, "d") ==
          unordered_set({5}));

  // w("_", _) = All possible pairs of <s, index(var_name)>
  int aIndex = db.getIndexOf(TableType::VAR_TABLE, "a");
  int bIndex = db.getIndexOf(TableType::VAR_TABLE, "b");
  int cIndex = db.getIndexOf(TableType::VAR_TABLE, "c");
  int dIndex = db.getIndexOf(TableType::VAR_TABLE, "d");
  REQUIRE(db.getVarMappings(RelationshipType::PTT_WHILE) ==
          SetOfIntLists({
              vector({1, aIndex}),
              vector({2, aIndex}),
              vector({3, bIndex}),
              vector({4, bIndex}),
              vector({5, cIndex}),
              vector({5, dIndex}),
          }));
}

TEST_CASE("HACK") {
  // Init PKB
  PKB db = PKB();
  RelationshipType rs = RelationshipType::PTT_ASSIGN_FULL_EXPR;
  REQUIRE(db.getStmtsForVarAndExpr(rs, "y", "bellow").empty());
  REQUIRE(db.getStmtsForVar(rs, "").empty());
  REQUIRE(db.getStmtsForExpr(rs, "").empty());
  REQUIRE(db.getVarMappingsForExpr(rs, "x+y") == SetOfStmtLists());
  REQUIRE(db.getVarMappings(rs) == SetOfStmtLists());

  db.addPatternRs(rs, 1, "y", "x+1");
  db.addPatternRs(rs, 2, "y", "x+1");
  db.addPatternRs(rs, 3, "v", "x+1");
  db.addPatternRs(rs, 4, "v", "x+2");

  // a (_, "x+1") = {1, 2, 3}
  REQUIRE(db.getStmtsForExpr(rs, "x+1") == unordered_set({1, 2, 3}));

  // a ("y", "x+1") = {1, 2}
  REQUIRE(db.getStmtsForVarAndExpr(rs, "y", "x+1") == unordered_set({1, 2}));

  // a (v, "x+1") = { {1, index(y)}, {2, index(y)}, {3, index(v)} }
  int yIdx = db.getIndexOf(TableType::VAR_TABLE, "y");
  int vIdx = db.getIndexOf(TableType::VAR_TABLE, "v");
  REQUIRE(db.getVarMappingsForExpr(rs, "x+1") ==
          SetOfStmtLists({vector<int>({1, yIdx}), vector<int>({2, yIdx}),
                          vector<int>({3, vIdx})}));

  //// a ("y",_) = {1, 2}, a("v",_) = {3, 4}
  REQUIRE(db.getStmtsForVar(rs, "y") == unordered_set({1, 2}));
  REQUIRE(db.getStmtsForVar(rs, "v") == unordered_set({3, 4}));

  //// a(v, _)  = return all pairs for filtering (s, varidx), etc etc
  REQUIRE(db.getVarMappings(rs) ==
          SetOfStmtLists({vector<int>({1, yIdx}), vector<int>({2, yIdx}),
                          vector<int>({3, vIdx}), vector<int>({4, vIdx})}));

  // Invalid Queries

  REQUIRE(db.getStmtsForVarAndExpr(rs, "y", "bellow").empty());
  REQUIRE(db.getStmtsForVar(rs, "good").empty());
  REQUIRE(db.getStmtsForExpr(rs, "bad").empty());
  REQUIRE(db.getVarMappingsForExpr(rs, "x+3").empty());

  int s = 1;
  string var = "";
  string expr = "";
  db.addPatternRs(RelationshipType::PTT_ASSIGN_FULL_EXPR, s, var, expr);
}
