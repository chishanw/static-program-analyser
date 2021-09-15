#include "PKB/ProcTable.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("PROCTABLE") {
  ProcTable test;

  // insert
  REQUIRE(test.insertProc("a") == 0);
  REQUIRE(test.insertProc("b") == 1);
  // duplicate insert
  REQUIRE(test.insertProc("a") == 0);

  // retrieve with valid index
  REQUIRE(test.getProcName(0) == "a");
  // retrieve with invalid index
  REQUIRE(test.getProcName(-1) == "");
  REQUIRE(test.getProcName(2) == "");

  // retrieve with valid Procedure name
  REQUIRE(test.getProcIndex("a") == 0);
  // retrieve with invalid index
  REQUIRE(test.getProcIndex("c") == -1);

  // test new method
  unordered_set<PROC_IDX> answer({ 0, 1 });
  REQUIRE(test.getAllProcedures() == answer);
}
