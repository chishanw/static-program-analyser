#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;
using namespace Catch;

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
  int yIdx = db.getIndexOf(TableType::VAR_TABLE, "y");
  int vIdx = db.getIndexOf(TableType::VAR_TABLE, "v");
  REQUIRE(db.getAssignVarPairsForFullExpr("x+1") ==
          vector({vector<int>({1, yIdx}), vector<int>({2, yIdx}),
                  vector<int>({3, vIdx})}));

  // a ("y",_) = {1, 2}, a("v",_) = {3, 4}
  REQUIRE(db.getAssignForVar("y") == unordered_set({1, 2}));
  REQUIRE(db.getAssignForVar("v") == unordered_set({3, 4}));

  // a(v, _)  = return all pairs for filtering (s, varidx), etc etc
  REQUIRE_THAT(db.getAssignVarPairs(), VectorContains(vector<int>({1, yIdx})));
  REQUIRE_THAT(db.getAssignVarPairs(), VectorContains(vector<int>({2, yIdx})));
  REQUIRE_THAT(db.getAssignVarPairs(), VectorContains(vector<int>({3, vIdx})));
  REQUIRE_THAT(db.getAssignVarPairs(), VectorContains(vector<int>({4, vIdx})));
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
  int yIdx = db.getIndexOf(TableType::VAR_TABLE, "y");
  int vIdx = db.getIndexOf(TableType::VAR_TABLE, "v");
  REQUIRE(db.getAssignVarPairsForSubExpr("x+1") ==
          vector({vector<int>({1, yIdx}), vector<int>({2, yIdx}),
                  vector<int>({3, vIdx})}));
  REQUIRE(db.getAssignForVar("y") == unordered_set({1, 2}));
  REQUIRE(db.getAssignForVar("v") == unordered_set({3, 4}));
}

TEST_CASE("IF_PATTERN") {
  // Init PKB
  PKB db = PKB();

  // Empty Results
  REQUIRE(db.getIfStmtVarPairs() == vector<vector<int>>());
  REQUIRE(db.getIfStmtForVar("f") == unordered_set<int>());

  db.addIfPtt(1, "a");
  db.addIfPtt(2, "a");
  db.addIfPtt(3, "b");
  db.addIfPtt(4, "b");
  db.addIfPtt(5, "c");
  db.addIfPtt(5, "d");

  // ifs("a", _, _) = {1, 2}
  REQUIRE(db.getIfStmtForVar("a") == unordered_set({1, 2}));
  REQUIRE(db.getIfStmtForVar("b") == unordered_set({3, 4}));
  REQUIRE(db.getIfStmtForVar("c") == unordered_set({5}));
  REQUIRE(db.getIfStmtForVar("d") == unordered_set({5}));

  // ifs("_", _, _) = All possible pairs of <s, index(var_name)>
  int aIndex = db.getIndexOf(TableType::VAR_TABLE, "a");
  int bIndex = db.getIndexOf(TableType::VAR_TABLE, "b");
  int cIndex = db.getIndexOf(TableType::VAR_TABLE, "c");
  int dIndex = db.getIndexOf(TableType::VAR_TABLE, "d");
  REQUIRE(db.getIfStmtVarPairs() == vector({
    vector({1, aIndex}),
    vector({2, aIndex}),
    vector({3, bIndex}),
    vector({4, bIndex}),
    vector({5, cIndex}),
    vector({5, dIndex})}));
}

TEST_CASE("WHILE_PATTERN") {
  // Init PKB
  PKB db = PKB();

  // Empty Results
  REQUIRE(db.getWhileStmtVarPairs() == vector<vector<int>>());
  REQUIRE(db.getWhileStmtForVar("d") == unordered_set<int>());

  db.addWhilePtt(1, "a");
  db.addWhilePtt(2, "a");
  db.addWhilePtt(3, "b");
  db.addWhilePtt(4, "b");
  db.addWhilePtt(5, "c");
  db.addWhilePtt(5, "d");

  // w("a", _) = {1, 2}
  REQUIRE(db.getWhileStmtForVar("a") == unordered_set({1, 2}));
  REQUIRE(db.getWhileStmtForVar("b") == unordered_set({3, 4}));
  REQUIRE(db.getWhileStmtForVar("c") == unordered_set({5}));
  REQUIRE(db.getWhileStmtForVar("d") == unordered_set({5}));

  // w("_", _) = All possible pairs of <s, index(var_name)>
  int aIndex = db.getIndexOf(TableType::VAR_TABLE, "a");
  int bIndex = db.getIndexOf(TableType::VAR_TABLE, "b");
  int cIndex = db.getIndexOf(TableType::VAR_TABLE, "c");
  int dIndex = db.getIndexOf(TableType::VAR_TABLE, "d");
  REQUIRE(db.getWhileStmtVarPairs() == vector({
    vector({1, aIndex}),
    vector({2, aIndex}),
    vector({3, bIndex}),
    vector({4, bIndex}),
    vector({5, cIndex}),
    vector({5, dIndex}),
    }));
}
