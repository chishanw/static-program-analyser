#include <set>

#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("MODIFIES_TABLE_STATEMENTS") {
  PKB db = PKB();

  // invalid queries when ModifiesKB and VarTable are empty
  unordered_set<int> emptySet({});
  vector<pair<int, vector<int>>> emptyAllPairs({});
  REQUIRE(db.getAllModifiesSPairs() == emptyAllPairs);
  REQUIRE(db.getVarsModifiedS(1) == emptySet);
  REQUIRE(db.getModifiesS("a") == emptySet);
  REQUIRE(db.isModifiesS(2, "b") == false);

  // valid insertions
  // x has varIdx 0, y 1, z 2
  db.addModifiesS(1, "x");
  db.addModifiesS(2, "x");
  db.addModifiesS(2, "y");
  db.addModifiesS(3, "x");
  db.addModifiesS(3, "y");
  db.addModifiesS(3, "z");

  // valid queries
  REQUIRE(db.isModifiesS(3, "z"));

  unordered_set<int> answer1({ 0 });
  unordered_set<int> answer2({ 0, 1 });
  unordered_set<int> answer3({ 0, 1, 2 });
  REQUIRE(db.getVarsModifiedS(1) == answer1);
  REQUIRE(db.getVarsModifiedS(2) == answer2);
  REQUIRE(db.getVarsModifiedS(3) == answer3);

  unordered_set<int> answerX({ 1, 2, 3 });
  unordered_set<int> answerY({ 2, 3 });
  unordered_set<int> answerZ({ 3 });
  REQUIRE(db.getModifiesS("x") == answerX);
  REQUIRE(db.getModifiesS("y") == answerY);
  REQUIRE(db.getModifiesS("z") == answerZ);

  vector<int> s1({ 0 });
  pair<int, vector<int>> firstPair(1, s1);
  vector<int> s2({ 0, 1 });
  pair<int, vector<int>> secondPair(2, s2);
  vector<int> s3({ 0, 1, 2 });
  pair<int, vector<int>> thirdPair(3, s3);
  vector<pair<int, vector<int>>> answerAllPairs({ firstPair, secondPair,
    thirdPair });
  vector<pair<int, vector<int>>> output = db
    .getAllModifiesSPairs();
  REQUIRE(set<pair<STMT_NO, vector<int>>>(output.begin(), output.end())
    == set<pair<STMT_NO, vector<int>>>(answerAllPairs.begin(),
    answerAllPairs.end()));

  // invalid queries
  REQUIRE(db.isModifiesS(1, "a") == false);
  REQUIRE(db.getModifiesS("b") == emptySet);
  REQUIRE(db.getVarsModifiedS(30) == emptySet);
}


TEST_CASE("MODIFIES_TABLE_PROCEDURE") {
  PKB db = PKB();
  PROC_NAME a = "a";
  PROC_NAME b = "b";
  PROC_NAME c = "c";

  // invalid queries when ModifiesKB and VarTable are empty
  unordered_set<int> emptySet({});
  vector<pair<int, vector<int>>> emptyAllPairs({});
  REQUIRE(db.getAllModifiesPPairs() == emptyAllPairs);
  REQUIRE(db.getVarsModifiedP("a") == emptySet);
  REQUIRE(db.getModifiesP("a") == emptySet);
  REQUIRE(db.isModifiesP("a", "b") == false);

  // valid insertions
  // x has varIdx 0, y 1, z 2
  db.addModifiesP(a, "x");
  db.addModifiesP(b, "x");
  db.addModifiesP(b, "y");
  db.addModifiesP(c, "x");
  db.addModifiesP(c, "y");
  db.addModifiesP(c, "z");

  // valid queries
  REQUIRE(db.isModifiesP(c, "z"));

  unordered_set<int> answer1({0});
  unordered_set<int> answer2({0, 1});
  unordered_set<int> answer3({0, 1, 2});
  REQUIRE(db.getVarsModifiedP(a) == answer1);
  REQUIRE(db.getVarsModifiedP(b) == answer2);
  REQUIRE(db.getVarsModifiedP(c) == answer3);

  unordered_set<int> answerX({0, 1, 2});
  unordered_set<int> answerY({1, 2});
  unordered_set<int> answerZ({2});
  REQUIRE(db.getModifiesP("x") == answerX);
  REQUIRE(db.getModifiesP("y") == answerY);
  REQUIRE(db.getModifiesP("z") == answerZ);

  vector<int> s1({0});
  pair<int, vector<int>> firstPair(0, s1);
  vector<int> s2({0, 1});
  pair<int, vector<int>> secondPair(1, s2);
  vector<int> s3({0, 1, 2});
  pair<int, vector<int>> thirdPair(2, s3);
  vector<pair<int, vector<int>>> answerAllPairs(
      {firstPair, secondPair, thirdPair});
  vector<pair<int, vector<int>>> output = db.getAllModifiesPPairs();
  REQUIRE(set<pair<PROC_IDX, vector<int>>>(output.begin(), output.end()) ==
          set<pair<PROC_IDX, vector<int>>>(answerAllPairs.begin(),
                                          answerAllPairs.end()));

  // invalid queries
  REQUIRE(db.isModifiesP(b, "a") == false);
  REQUIRE(db.getModifiesP("f") == emptySet);
  REQUIRE(db.getVarsModifiedP("f") == emptySet);
}
