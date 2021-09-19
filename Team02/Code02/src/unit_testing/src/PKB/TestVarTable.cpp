#include "PKB/VarTable.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("VARTABLE") {
  VarTable test;

  // insert
  REQUIRE(test.insertVar("a") == 0);
  REQUIRE(test.insertVar("b") == 1);
  // duplicate insert
  REQUIRE(test.insertVar("a") == 0);

  // retrieve with valid index
  REQUIRE(test.getVarName(0) == "a");
  // retrieve with invalid index
  REQUIRE(test.getVarName(-1) == "");
  REQUIRE(test.getVarName(2) == "");

  // retrieve with valid variable name
  REQUIRE(test.getVarIndex("a") == 0);
  // retrieve with invalid index
  REQUIRE(test.getVarIndex("c") == -1);

  // test new method
  unordered_set<VAR_IDX> answer({ 0, 1 });
  REQUIRE(test.getAllVariables() == answer);
}
