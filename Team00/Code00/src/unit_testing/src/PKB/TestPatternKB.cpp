#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("FULL_PATTERN") {
  /*
  Stmt_No 1: y = x + 1;
  Stmt_No 2: y = x + 1;
  Stmt_No 3: v = x + 1;
  Stmt_No 4: v = y + 2;

  */

  // Init PKB
  PKB db = PKB();
  db.addAssignPttFullExpr(1, "y", "x+1");
  db.addAssignPttFullExpr(2, "y", "x+1");
  db.addAssignPttFullExpr(3, "v", "x+1");
  db.addAssignPttFullExpr(4, "v", "x+2");

  // a (_, "x+1") = {1, 2, 3}
  REQUIRE(db.getAssignForFullExpr("x+1") == unordered_set({1, 2, 3}));

  // a ("y", "x+1") = {1, 2}
  REQUIRE(db.getAssignForVarAndFullExpr("y", "x+1") == unordered_set({1, 2}));

  // a (v, "x+1") = { {1, index(y)}, {2, index(y)}, {3, index(v)} }
  int yIdx = db.varTable.getVarIndex("y");
  int vIdx = db.varTable.getVarIndex("v");
  REQUIRE(db.getAssignVarPairsForFullExpr("x+1") ==
          vector({vector<int>({1, yIdx}), vector<int>({2, yIdx}),
                  vector<int>({3, vIdx})}));

  // a ("y",_) = {1, 2}, a("v",_) = {3, 4}
  REQUIRE(db.getAssignForVar("y") == unordered_set({1, 2}));
  REQUIRE(db.getAssignForVar("v") == unordered_set({3, 4}));

  // a(v, _)  = return all pairs for filtering (s, varidx), etc etc
  REQUIRE_THAT(db.getAssignVarPairs(),
               Catch::Matchers::UnorderedEquals(vector<vector<int>>(
                   {vector<int>({1, yIdx}), vector<int>({2, yIdx}),
                    vector<int>({3, vIdx}), vector<int>({4, vIdx})})));
}

TEST_CASE("SUB_PATTERN") {
  /*
  Stmt_No 1: y = x + 1;
  Stmt_No 2: y = x + 1;
  Stmt_No 3: v = x + 1;
  Stmt_No 4: v = y + 2;
  */

  // Init PKB
  PKB db = PKB();
  db.addAssignPttSubExpr(1, "y", "x+1");
  db.addAssignPttSubExpr(1, "y", "x");
  db.addAssignPttSubExpr(1, "y", "1");
  db.addAssignPttSubExpr(2, "y", "x+1");
  db.addAssignPttSubExpr(2, "y", "x");
  db.addAssignPttSubExpr(2, "y", "1");
  db.addAssignPttSubExpr(3, "v", "x+1");
  db.addAssignPttSubExpr(3, "v", "x");
  db.addAssignPttSubExpr(3, "v", "1");
  db.addAssignPttSubExpr(4, "v", "y+2");
  db.addAssignPttSubExpr(4, "v", "y");
  db.addAssignPttSubExpr(4, "v", "2");

  // a (_, "_x+1_") = {1, 2, 3}
  REQUIRE(db.getAssignForSubExpr("x+1") == unordered_set({1, 2, 3}));

  // a (_, "_x_") = {1, 2, 3, 4}
  REQUIRE(db.getAssignForSubExpr("x") == unordered_set({1, 2, 3}));

  // a (_, "_1_") = {1, 2, 3}
  REQUIRE(db.getAssignForSubExpr("1") == unordered_set({1, 2, 3}));

  // a (_, "_2_") = {4}
  REQUIRE(db.getAssignForSubExpr("2") == unordered_set({4}));

  // a ("y", "_x+1_") = a ("y", "_x_") = {1, 2},
  REQUIRE(db.getAssignForVarAndSubExpr("y", "x+1") == unordered_set({1, 2}));
  REQUIRE(db.getAssignForVarAndSubExpr("y", "x") == unordered_set({1, 2}));

  // a (v, "x+1") = { {1, index(y)}, {2, index(y)}, {3, index(v)} }
  int yIdx = db.varTable.getVarIndex("y");
  int vIdx = db.varTable.getVarIndex("v");
  REQUIRE(db.getAssignVarPairsForSubExpr("x+1") ==
          vector({vector<int>({1, yIdx}), vector<int>({2, yIdx}),
                  vector<int>({3, vIdx})}));
  REQUIRE(db.getAssignForVar("y") == unordered_set({1, 2}));
  REQUIRE(db.getAssignForVar("v") == unordered_set({3, 4}));
}
