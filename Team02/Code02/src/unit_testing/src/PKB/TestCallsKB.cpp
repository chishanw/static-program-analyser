#include <set>

#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

// TEST_CASE("CALLS_TABLE") {
//   PKB db = PKB();

//   // invalid queries when table is empty
//   unordered_set<PROC_IDX> emptySet({});
//   vector<pair<PROC_IDX, vector<PROC_IDX>>> emptyRes({});

//   REQUIRE(!db.isStmt(DesignEntity::CALL, 1));
//   REQUIRE(!db.isCalls("a", "b"));
//   REQUIRE(db.getRight(RelationshipType::CALLS, TableType::PROC_TABLE, "a") ==
//           emptySet);
//   REQUIRE(db.getLeft(RelationshipType::CALLS, TableType::PROC_TABLE, "b") ==
//           emptySet);
//   REQUIRE(db.getProcCalledByCallStmt(2) == -1);
//   REQUIRE(db.getMappings(RelationshipType::CALLS, ParamPosition::BOTH) ==
//           emptyRes);

//   /* valid insertions with valid queries
//    * source code looks like:
//    * procedure a { // procIdx 0
//    *   call b;
//    *   call c;
//    * }
//    *
//    * procedure b { // procIdx 1
//    *   call c;
//    * }
//    *
//    * procedure c { // procIdx 2
//    *   // whatever
//    * }
//    */
//   db.addRs(RelationshipType::CALLS, TableType::PROC_TABLE, 1, "a", "b");
//   db.addRs(RelationshipType::CALLS, TableType::PROC_TABLE, 2, "a", "c");
//   db.addRs(RelationshipType::CALLS, TableType::PROC_TABLE, 2, "b", "c");

//   REQUIRE(db.isStmt(DesignEntity::CALL, 1));
//   REQUIRE(db.isCalls("a", "b"));
//   REQUIRE(db.getRight(RelationshipType::CALLS, TableType::PROC_TABLE, "a") ==
//           unordered_set({1, 2}));
//   REQUIRE(db.getLeft(RelationshipType::CALLS, TableType::PROC_TABLE, "b") ==
//           unordered_set({0}));
//   REQUIRE(db.getLeft(RelationshipType::CALLS, TableType::PROC_TABLE, "c") ==
//           unordered_set({0, 1}));
//   REQUIRE(db.getProcCalledByCallStmt(2) == 2);
//   vector<PROC_IDX> calledByA({1, 2});
//   pair<PROC_IDX, vector<PROC_IDX>> aRes(0, calledByA);
//   vector<PROC_IDX> calledByB({2});
//   pair<PROC_IDX, vector<PROC_IDX>> bRes(1, calledByB);
//   vector<pair<PROC_IDX, vector<PROC_IDX>>> result({aRes, bRes});
//   vector<pair<PROC_IDX, vector<PROC_IDX>>> output =
//       db.getMappings(RelationshipType::CALLS, ParamPosition::BOTH);
//   REQUIRE(set<pair<PROC_IDX, vector<PROC_IDX>>>(output.begin(), output.end())
//   ==
//           set<pair<PROC_IDX, vector<PROC_IDX>>>(result.begin(),
//           result.end()));

//   // invalid queries
//   REQUIRE(!db.isStmt(DesignEntity::CALL, 4));
//   REQUIRE(!db.isCalls("c", "a"));
// }

// TEST_CASE("CALLS_TABLE_TRANS") {
//   PKB db = PKB();

//   // invalid queries when table is empty
//   unordered_set<PROC_IDX> emptySet({});
//   vector<pair<PROC_IDX, vector<PROC_IDX>>> emptyRes({});

//   REQUIRE(!db.isCallsT("a", "b"));
//   REQUIRE(db.getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a")
//   ==
//           emptySet);
//   REQUIRE(db.getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE, "b")
//   ==
//           emptySet);
//   REQUIRE(db.getMappings(RelationshipType::CALLS_T, ParamPosition::BOTH) ==
//           emptyRes);

//   /* valid insertions with valid queries
//    * source code looks like:
//    * procedure a { // procIdx 0
//    *   call b;
//    * }
//    *
//    * procedure b { // procIdx 1
//    *   call c;
//    * }
//    *
//    * procedure c { // procIdx 2
//    *   // whatever
//    * }
//    */
//   db.insertAt(TableType::PROC_TABLE, "a");
//   db.insertAt(TableType::PROC_TABLE, "b");
//   db.insertAt(TableType::PROC_TABLE, "c");

//   db.addCallsT("a", "b");
//   db.addCallsT("a", "c");
//   db.addCallsT("b", "c");

//   REQUIRE(db.isCallsT("a", "b"));
//   REQUIRE(db.isCallsT("a", "c"));
//   REQUIRE(db.getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a")
//   ==
//           unordered_set({1, 2}));
//   REQUIRE(db.getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE, "c")
//   ==
//           unordered_set({0, 1}));
//   vector<PROC_IDX> calledByA({1, 2});
//   pair<PROC_IDX, vector<PROC_IDX>> aRes(0, calledByA);
//   vector<PROC_IDX> calledByB({2});
//   pair<PROC_IDX, vector<PROC_IDX>> bRes(1, calledByB);
//   vector<pair<PROC_IDX, vector<PROC_IDX>>> result({aRes, bRes});
//   vector<pair<PROC_IDX, vector<PROC_IDX>>> output =
//       db.getMappings(RelationshipType::CALLS_T, ParamPosition::BOTH);
//   REQUIRE(set<pair<PROC_IDX, vector<PROC_IDX>>>(output.begin(), output.end())
//   ==
//           set<pair<PROC_IDX, vector<PROC_IDX>>>(result.begin(),
//           result.end()));

//   // invalid query
//   REQUIRE(!db.isCallsT("c", "a"));
// }
