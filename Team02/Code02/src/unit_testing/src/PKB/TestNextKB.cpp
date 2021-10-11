#include "PKB/PKB.h"
#include "catch.hpp"
using namespace std;

TEST_CASE("NEXT_TABLE") {
  // Init
  PKB db = PKB();

  // invalid queries when empty
  REQUIRE(!db.isNext(1, 2));
  REQUIRE(db.getNextStmts(1) == UNO_SET_OF_STMT_NO());
  REQUIRE(db.getPreviousStmts(2) == UNO_SET_OF_STMT_NO());
  REQUIRE(db.getAllNextStmtPairs() == vector<pair<STMT_NO, vector<STMT_NO>>>());

  // set Next
  // Successful Insertion, return true
  db.addNext(1, 2);
  db.addNext(2, 3);
  db.addNext(3, 4);  // lets say line 4 is a if stmt
  db.addNext(4, 5);  // line 5 if block
  db.addNext(4, 6);  // line 6 else block
  db.addNext(5, 7);
  db.addNext(6, 7);

  // get NextStmts
  REQUIRE(db.getNextStmts(1) == UNO_SET_OF_STMT_NO({ 2 }));
  REQUIRE(db.getNextStmts(2) == UNO_SET_OF_STMT_NO({ 3 }));
  REQUIRE(db.getNextStmts(3) == UNO_SET_OF_STMT_NO({ 4 }));
  REQUIRE(db.getNextStmts(4) == UNO_SET_OF_STMT_NO({ 5, 6 }));

  // get PreviousStmts
  REQUIRE(db.getPreviousStmts(2) == UNO_SET_OF_STMT_NO({ 1 }));
  REQUIRE(db.getPreviousStmts(3) == UNO_SET_OF_STMT_NO({ 2 }));
  REQUIRE(db.getPreviousStmts(5) == UNO_SET_OF_STMT_NO({ 4 }));
  REQUIRE(db.getPreviousStmts(6) == UNO_SET_OF_STMT_NO({ 4 }));
  REQUIRE(db.getPreviousStmts(7) == UNO_SET_OF_STMT_NO({ 5, 6 }));

  // get all follow statement pairs
  vector<STMT_NO> vec1({ 2 });
  pair<STMT_NO, vector<STMT_NO>> ans1(1, vec1);
  vector<STMT_NO> vec2({ 3 });
  pair<STMT_NO, vector<STMT_NO>> ans2(2, vec2);
  vector<STMT_NO> vec3({ 4 });
  pair<STMT_NO, vector<STMT_NO>> ans3(3, vec3);
  vector<STMT_NO> vec4({ 5, 6 });
  pair<STMT_NO, vector<STMT_NO>> ans4(4, vec4);
  vector<STMT_NO> vec56({ 7 });
  pair<STMT_NO, vector<STMT_NO>> ans5(5, vec56);
  pair<STMT_NO, vector<STMT_NO>> ans6(6, vec56);
  vector<pair<STMT_NO, vector<STMT_NO>>> expected({ ans1, ans2, ans3, ans4,
      ans5, ans6 });

  vector<pair<STMT_NO, vector<STMT_NO>>> output = db.getAllNextStmtPairs();

  REQUIRE(set<pair<STMT_NO, vector<STMT_NO>>>(output.begin(), output.end())
      == set<pair<STMT_NO, vector<STMT_NO>>>(expected.begin(), expected.end()));

  // invalid queries
  REQUIRE(!db.isNext(1, 3));
  REQUIRE(db.getNextStmts(8) == UNO_SET_OF_STMT_NO());
  REQUIRE(db.getPreviousStmts(8) == UNO_SET_OF_STMT_NO());
}
