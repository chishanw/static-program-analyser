#include <set>

#include "PKB/UsesKB.h"
#include "PKB/VarTable.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("USES_TABLE") {
  VarTable* testVarTable = new VarTable();
  UsesKB testUsesKB(testVarTable);

  // invalid queries when UsesKB and VarTable are empty
  unordered_set<int> emptySet({});
  vector<pair<int, vector<int>>> emptyAllPairs({});
  REQUIRE(testUsesKB.getAllUsesSPairs() == emptyAllPairs);
  REQUIRE(testUsesKB.getVarsUsedS(1) == emptySet);
  REQUIRE(testUsesKB.getUsesS("a") == emptySet);
  REQUIRE(testUsesKB.isUsesS(2, "b") == false);

  // valid insertions
  // x has varIdx 0, y 1, z 2
  testUsesKB.addUsesS(1, "x");
  testUsesKB.addUsesS(2, "x");
  testUsesKB.addUsesS(2, "y");
  testUsesKB.addUsesS(3, "x");
  testUsesKB.addUsesS(3, "y");
  testUsesKB.addUsesS(3, "z");

  // valid queries
  REQUIRE(testUsesKB.isUsesS(3, "z"));

  unordered_set<int> answer1({0});
  unordered_set<int> answer2({0, 1});
  unordered_set<int> answer3({0, 1, 2});
  REQUIRE(testUsesKB.getVarsUsedS(1) == answer1);
  REQUIRE(testUsesKB.getVarsUsedS(2) == answer2);
  REQUIRE(testUsesKB.getVarsUsedS(3) == answer3);

  unordered_set<int> answerX({ 1, 2, 3 });
  unordered_set<int> answerY({ 2, 3 });
  unordered_set<int> answerZ({ 3 });
  REQUIRE(testUsesKB.getUsesS("x") == answerX);
  REQUIRE(testUsesKB.getUsesS("y") == answerY);
  REQUIRE(testUsesKB.getUsesS("z") == answerZ);

  vector<int> s1({ 0 });
  pair<int, vector<int>> firstPair(1, s1);
  vector<int> s2({ 0, 1 });
  pair<int, vector<int>> secondPair(2, s2);
  vector<int> s3({ 0, 1, 2 });
  pair<int, vector<int>> thirdPair(3, s3);
  vector<pair<int, vector<int>>> answerAllPairs({ firstPair, secondPair,
    thirdPair });
  vector<pair<int, vector<int>>> output = testUsesKB.getAllUsesSPairs();
  REQUIRE(set<pair<STMT_NO, vector<int>>>(output.begin(), output.end())
    == set<pair<STMT_NO, vector<int>>>(answerAllPairs.begin(),
    answerAllPairs.end()));

  // invalid queries
  REQUIRE(testUsesKB.isUsesS(1, "a") == false);
  REQUIRE(testUsesKB.getUsesS("b") == emptySet);
  REQUIRE(testUsesKB.getVarsUsedS(30) == emptySet);
}
