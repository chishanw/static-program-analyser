#include <set>

#include "PKB/PKB.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("USES_TABLE_STATEMENT") {
  PKB db = PKB();

  // invalid queries when UsesKB and VarTable are empty
  unordered_set<int> emptySet({});
  vector<pair<int, vector<int>>> emptyAllPairs({});
  REQUIRE(db.getAllUsesSPairs() == emptyAllPairs);
  REQUIRE(db.getVarsUsedS(1) == emptySet);
  REQUIRE(db.getUsesS("a") == emptySet);
  REQUIRE(db.isUsesS(2, "b") == false);

  // valid insertions
  // x has varIdx 0, y 1, z 2
  db.addUsesS(1, "x");
  db.addUsesS(2, "x");
  db.addUsesS(2, "y");
  db.addUsesS(3, "x");
  db.addUsesS(3, "y");
  db.addUsesS(3, "z");

  // valid queries
  REQUIRE(db.isUsesS(3, "z"));

  unordered_set<int> answer1({0});
  unordered_set<int> answer2({0, 1});
  unordered_set<int> answer3({0, 1, 2});
  REQUIRE(db.getVarsUsedS(1) == answer1);
  REQUIRE(db.getVarsUsedS(2) == answer2);
  REQUIRE(db.getVarsUsedS(3) == answer3);

  unordered_set<int> answerX({ 1, 2, 3 });
  unordered_set<int> answerY({ 2, 3 });
  unordered_set<int> answerZ({ 3 });
  REQUIRE(db.getUsesS("x") == answerX);
  REQUIRE(db.getUsesS("y") == answerY);
  REQUIRE(db.getUsesS("z") == answerZ);

  vector<int> s1({ 0 });
  pair<int, vector<int>> firstPair(1, s1);
  vector<int> s2({ 0, 1 });
  pair<int, vector<int>> secondPair(2, s2);
  vector<int> s3({ 0, 1, 2 });
  pair<int, vector<int>> thirdPair(3, s3);
  vector<pair<int, vector<int>>> answerAllPairs({ firstPair, secondPair,
    thirdPair });
  vector<pair<int, vector<int>>> output = db.getAllUsesSPairs();
  REQUIRE(set<pair<STMT_NO, vector<int>>>(output.begin(), output.end())
    == set<pair<STMT_NO, vector<int>>>(answerAllPairs.begin(),
    answerAllPairs.end()));

  // invalid queries
  REQUIRE(db.isUsesS(1, "a") == false);
  REQUIRE(db.getUsesS("b") == emptySet);
  REQUIRE(db.getVarsUsedS(30) == emptySet);
}

TEST_CASE("USES_TABLE_PROCEDURE") {
  PKB db = PKB();
  PROC_NAME a = "a";
  PROC_NAME b = "b";
  PROC_NAME c = "c";

  // invalid queries when UsesKB and VarTable are empty
  unordered_set<int> emptySet({});
  vector<pair<int, vector<int>>> emptyAllPairs({});
  REQUIRE(db.getAllUsesPPairs() == emptyAllPairs);
  REQUIRE(db.getVarsUsedP("a") == emptySet);
  REQUIRE(db.getUsesP("a") == emptySet);
  REQUIRE(db.isUsesP("c", "b") == false);

  // valid insertions
  // x has varIdx 0, y 1, z 2
  db.addUsesP(a, "x");
  db.addUsesP(b, "x");
  db.addUsesP(b, "y");
  db.addUsesP(c, "x");
  db.addUsesP(c, "y");
  db.addUsesP(c, "z");

  // valid queries
  REQUIRE(db.isUsesP(c, "z"));

  unordered_set<int> answer1({0});
  unordered_set<int> answer2({0, 1});
  unordered_set<int> answer3({0, 1, 2});
  REQUIRE(db.getVarsUsedP(a) == answer1);
  REQUIRE(db.getVarsUsedP(b) == answer2);
  REQUIRE(db.getVarsUsedP(c) == answer3);

  unordered_set<int> answerX({0, 1, 2});
  unordered_set<int> answerY({1, 2});
  unordered_set<int> answerZ({2});
  REQUIRE(db.getUsesP("x") == answerX);
  REQUIRE(db.getUsesP("y") == answerY);
  REQUIRE(db.getUsesP("z") == answerZ);

  vector<int> s1({0});
  pair<int, vector<int>> firstPair(0, s1);
  vector<int> s2({0, 1});
  pair<int, vector<int>> secondPair(1, s2);
  vector<int> s3({0, 1, 2});
  pair<int, vector<int>> thirdPair(2, s3);
  vector<pair<int, vector<int>>> answerAllPairs(
      {firstPair, secondPair, thirdPair});
  vector<pair<int, vector<int>>> output = db.getAllUsesPPairs();
  REQUIRE(set<pair<PROC_IDX, vector<int>>>(output.begin(), output.end()) ==
          set<pair<PROC_IDX, vector<int>>>(answerAllPairs.begin(),
                                          answerAllPairs.end()));

  // invalid queries
  REQUIRE(db.isUsesP(b, "a") == false);
  REQUIRE(db.getUsesP("f") == emptySet);
  REQUIRE(db.getVarsUsedP("f") == emptySet);
}
