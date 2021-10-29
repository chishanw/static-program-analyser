#include <set>

#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("CALLS_TABLE") {
  PKB db = PKB();

  // invalid queries when table is empty
  unordered_set<PROC_IDX> emptySet({});
  vector<pair<PROC_IDX, vector<PROC_IDX>>> emptyRes({});

  REQUIRE(!db.isCallStmt(1));
  REQUIRE(!db.isCalls("a", "b"));
  REQUIRE(db.getProcsCalledBy("a") == emptySet);
  REQUIRE(db.getCallerProcs("b") == emptySet);
  REQUIRE(db.getProcCalledByCallStmt(2) == -1);
  REQUIRE(db.getAllCallsPairs() == emptyRes);

  /* valid insertions with valid queries
  * source code looks like:
  * procedure a { // procIdx 0
  *   call b;
  *   call c;
  * }
  * 
  * procedure b { // procIdx 1
  *   call c;
  * }
  * 
  * procedure c { // procIdx 2
  *   // whatever
  * }
  */ 
  db.addCalls(1, "a", "b");
  db.addCalls(2, "a", "c");
  db.addCalls(2, "b", "c");

  REQUIRE(db.isCallStmt(1));
  REQUIRE(db.isCalls("a", "b"));
  REQUIRE(db.getProcsCalledBy("a") == unordered_set({ 1, 2 }));
  REQUIRE(db.getCallerProcs("b") == unordered_set({ 0 }));
  REQUIRE(db.getCallerProcs("c") == unordered_set({ 0, 1 }));
  REQUIRE(db.getProcCalledByCallStmt(2) == 2);
  vector<PROC_IDX> calledByA({ 1, 2 });
  pair<PROC_IDX, vector<PROC_IDX>> aRes(0, calledByA);
  vector<PROC_IDX> calledByB({ 2 });
  pair<PROC_IDX, vector<PROC_IDX>> bRes(1, calledByB);
  vector<pair<PROC_IDX, vector<PROC_IDX>>> result({ aRes, bRes });
  vector<pair<PROC_IDX, vector<PROC_IDX>>> output = db.getAllCallsPairs();
  REQUIRE(set<pair<PROC_IDX, vector<PROC_IDX>>>(output.begin(), output.end())
    == set<pair<PROC_IDX, vector<PROC_IDX>>>(result.begin(),
    result.end()));

  // invalid queries
  REQUIRE(!db.isCallStmt(4));
  REQUIRE(!db.isCalls("c", "a"));
}


TEST_CASE("CALLS_TABLE_TRANS") {
  PKB db = PKB();

  // invalid queries when table is empty
  unordered_set<PROC_IDX> emptySet({});
  vector<pair<PROC_IDX, vector<PROC_IDX>>> emptyRes({});

  REQUIRE(!db.isCallsT("a", "b"));
  REQUIRE(db.getProcsCalledTBy("a") == emptySet);
  REQUIRE(db.getCallerTProcs("b") == emptySet);
  REQUIRE(db.getAllCallsTPairs() == emptyRes);

  /* valid insertions with valid queries
  * source code looks like:
  * procedure a { // procIdx 0
  *   call b;
  * }
  *
  * procedure b { // procIdx 1
  *   call c;
  * }
  *
  * procedure c { // procIdx 2
  *   // whatever
  * }
  */
  db.insertAt(TABLE_ENUM::PROC_TABLE, "a");
  db.insertAt(TABLE_ENUM::PROC_TABLE, "b");
  db.insertAt(TABLE_ENUM::PROC_TABLE, "c");

  db.addCallsT("a", "b");
  db.addCallsT("a", "c");
  db.addCallsT("b", "c");

  REQUIRE(db.isCallsT("a", "b"));
  REQUIRE(db.isCallsT("a", "c"));
  REQUIRE(db.getProcsCalledTBy("a") == unordered_set({ 1, 2 }));
  REQUIRE(db.getCallerTProcs("c") == unordered_set({ 0, 1 }));
  vector<PROC_IDX> calledByA({ 1, 2 });
  pair<PROC_IDX, vector<PROC_IDX>> aRes(0, calledByA);
  vector<PROC_IDX> calledByB({ 2 });
  pair<PROC_IDX, vector<PROC_IDX>> bRes(1, calledByB);
  vector<pair<PROC_IDX, vector<PROC_IDX>>> result({ aRes, bRes });
  vector<pair<PROC_IDX, vector<PROC_IDX>>> output = db.getAllCallsTPairs();
  REQUIRE(set<pair<PROC_IDX, vector<PROC_IDX>>>(output.begin(), output.end())
    == set<pair<PROC_IDX, vector<PROC_IDX>>>(result.begin(),
    result.end()));

  // invalid query
  REQUIRE(!db.isCallsT("c", "a"));
}
