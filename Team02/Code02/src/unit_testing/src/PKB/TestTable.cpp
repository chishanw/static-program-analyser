#include "PKB/Table.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("TABLE AS CONSTTABLE") {
  Table test;

  // insert
  REQUIRE(test.insert("1") == 0);
  REQUIRE(test.insert("2") == 1);
  // duplicate insert
  REQUIRE(test.insert("1") == 0);

  // retrieve with valid index
  REQUIRE(test.getElement(0) == "1");
  // retrieve with invalid index
  REQUIRE(test.getElement(-1) == "");
  REQUIRE(test.getElement(2) == "");

  // retrieve with valid constant name
  REQUIRE(test.getIndex("1") == 0);
  // retrieve with invalid index
  REQUIRE(test.getIndex("3") == -1);

  // test new method
  unordered_set<int> answer({ 0, 1 });
  REQUIRE(test.getAllElements() == answer);
}

TEST_CASE("TABLE AS PROCTABLE") {
  Table test;

  // insert
  REQUIRE(test.insert("a") == 0);
  REQUIRE(test.insert("b") == 1);
  // duplicate insert
  REQUIRE(test.insert("a") == 0);

  // retrieve with valid index
  REQUIRE(test.getElement(0) == "a");
  // retrieve with invalid index
  REQUIRE(test.getElement(-1) == "");
  REQUIRE(test.getElement(2) == "");

  // retrieve with valid Procedure name
  REQUIRE(test.getIndex("a") == 0);
  // retrieve with invalid index
  REQUIRE(test.getIndex("c") == -1);

  // test new method
  unordered_set<int> answer({ 0, 1 });
  REQUIRE(test.getAllElements() == answer);
}

TEST_CASE("TABLE AS VARTABLE") {
  Table test;

  // insert
  REQUIRE(test.insert("a") == 0);
  REQUIRE(test.insert("b") == 1);
  // duplicate insert
  REQUIRE(test.insert("a") == 0);

  // retrieve with valid index
  REQUIRE(test.getElement(0) == "a");
  // retrieve with invalid index
  REQUIRE(test.getElement(-1) == "");
  REQUIRE(test.getElement(2) == "");

  // retrieve with valid variable name
  REQUIRE(test.getIndex("a") == 0);
  // retrieve with invalid index
  REQUIRE(test.getIndex("c") == -1);

  // test new method
  unordered_set<int> answer({ 0, 1 });
  REQUIRE(test.getAllElements() == answer);
}
