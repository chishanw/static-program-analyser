#include "PKB/ConstTable.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("CONSTTABLE") {
  ConstTable test;

  // insert
  REQUIRE(test.insertConst("1") == 0);
  REQUIRE(test.insertConst("2") == 1);
  // duplicate insert
  REQUIRE(test.insertConst("1") == 0);

  // retrieve with valid index
  REQUIRE(test.getConst(0) == "1");
  // retrieve with invalid index
  REQUIRE(test.getConst(-1) == "");
  REQUIRE(test.getConst(2) == "");

  // retrieve with valid constant name
  REQUIRE(test.getConstIndex("1") == 0);
  // retrieve with invalid index
  REQUIRE(test.getConstIndex("3") == -1);

  // test new method
  unordered_set<CONST_IDX> answer({ 0, 1 });
  REQUIRE(test.getAllConstants() == answer);
}
