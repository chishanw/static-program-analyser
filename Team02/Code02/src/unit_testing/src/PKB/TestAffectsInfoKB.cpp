#include <set>

#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("AFFECTS_INFO_TABLE") {
  PKB db = PKB();

  // invalid queries when table is empty
  REQUIRE(db.getNextStmtForIfStmt(1) == -1);
  REQUIRE(db.getFirstStmtOfAllProcs().empty());

  /* valid insertions with valid queries
   * source code looks like:
   * procedure a { // procIdx 0
   *   if (x == 2) then { // stmt# 1
   *     x = 3; }
   *   else { y = 2; }
   * }
   *
   * procedure b { // procIdx 1
   *   if (x == 3) then { // stmt# 4
   *     read x; }
   *   else { x = 2; }
   *   print y; // stmt# 7
   * }
   */
  db.insertAt(TableType::PROC_TABLE, "a");
  db.insertAt(TableType::PROC_TABLE, "b");
  db.addFirstStmtOfProc("a", 1);
  db.addFirstStmtOfProc("b", 4);
  db.addNextStmtForIfStmt(4, 7);

  REQUIRE(db.getNextStmtForIfStmt(1) == -1);
  REQUIRE(db.getNextStmtForIfStmt(4) == 7);
  auto allFirstStmts = db.getFirstStmtOfAllProcs();
  REQUIRE(set(allFirstStmts.begin(), allFirstStmts.end()) == set<int>({1, 4}));

  // invalid queries
  REQUIRE(db.getNextStmtForIfStmt(2) == -1);
}
