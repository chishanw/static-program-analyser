#include <set>

#include "PKB/ModifiesKB.h"
#include "PKB/VarTable.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("MODIFIES_TABLE") {
  VarTable* testVarTable = new VarTable();
  ModifiesKB testModifiesKB(testVarTable);

  // invalid queries when ModifiesKB and VarTable are empty
  unordered_set<int> emptySet({});
  vector<pair<int, vector<int>>> emptyAllPairs({});
  REQUIRE(testModifiesKB.getAllModifiesSPairs() == emptyAllPairs);
  REQUIRE(testModifiesKB.getVarsModifiedS(1) == emptySet);
  REQUIRE(testModifiesKB.getModifiesS("a") == emptySet);
  REQUIRE(testModifiesKB.isModifiesS(2, "b") == false);

  // valid insertions
  // x has varIdx 0, y 1, z 2
  testModifiesKB.addModifiesS(1, "x");
  testModifiesKB.addModifiesS(2, "x");
  testModifiesKB.addModifiesS(2, "y");
  testModifiesKB.addModifiesS(3, "x");
  testModifiesKB.addModifiesS(3, "y");
  testModifiesKB.addModifiesS(3, "z");

  // valid queries
  REQUIRE(testModifiesKB.isModifiesS(3, "z"));

  unordered_set<int> answer1({ 0 });
  unordered_set<int> answer2({ 0, 1 });
  unordered_set<int> answer3({ 0, 1, 2 });
  REQUIRE(testModifiesKB.getVarsModifiedS(1) == answer1);
  REQUIRE(testModifiesKB.getVarsModifiedS(2) == answer2);
  REQUIRE(testModifiesKB.getVarsModifiedS(3) == answer3);

  unordered_set<int> answerX({ 1, 2, 3 });
  unordered_set<int> answerY({ 2, 3 });
  unordered_set<int> answerZ({ 3 });
  REQUIRE(testModifiesKB.getModifiesS("x") == answerX);
  REQUIRE(testModifiesKB.getModifiesS("y") == answerY);
  REQUIRE(testModifiesKB.getModifiesS("z") == answerZ);

  vector<int> s1({ 0 });
  pair<int, vector<int>> firstPair(1, s1);
  vector<int> s2({ 0, 1 });
  pair<int, vector<int>> secondPair(2, s2);
  vector<int> s3({ 0, 1, 2 });
  pair<int, vector<int>> thirdPair(3, s3);
  vector<pair<int, vector<int>>> answerAllPairs({ firstPair, secondPair,
    thirdPair });
  vector<pair<int, vector<int>>> output = testModifiesKB
    .getAllModifiesSPairs();
  REQUIRE(set<pair<STMT_NO, vector<int>>>(output.begin(), output.end())
    == set<pair<STMT_NO, vector<int>>>(answerAllPairs.begin(),
    answerAllPairs.end()));

  // invalid queries
  REQUIRE(testModifiesKB.isModifiesS(1, "a") == false);
  REQUIRE(testModifiesKB.getModifiesS("b") == emptySet);
  REQUIRE(testModifiesKB.getVarsModifiedS(30) == emptySet);
}
