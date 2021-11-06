
#include <Common/Tokenizer.h>
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>

#include "catch.hpp"

using namespace std;

TEST_CASE("[DE][Next R/S] source from quiz") {
  string program =
      "procedure a {            "
      "  while (x == 0) {       "  // 1
      "    stmt2 = 0;           "
      "    while (x == 0) {     "  // 3
      "      stmt4 = 0;         "
      "      stmt5 = 0;         "
      "      stmt6 = 0;         "
      "    }                    "
      "    if (x == 0) then {   "  // 7
      "      stmt8 = 0;         "
      "      stmt9 = 0;         "
      "    } else {             "
      "      stmt10 = 0;        "
      "    }                    "
      "  }                      "
      "}                        ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  CHECK(pkb->getRight(RelationshipType::NEXT, 0) ==
        unordered_set<int>{});  // out of bound
  CHECK(pkb->getRight(RelationshipType::NEXT, 1) == unordered_set<int>{2});
  CHECK(pkb->getRight(RelationshipType::NEXT, 2) == unordered_set<int>{3});
  CHECK(pkb->getRight(RelationshipType::NEXT, 3) == unordered_set<int>{4, 7});
  CHECK(pkb->getRight(RelationshipType::NEXT, 4) == unordered_set<int>{5});
  CHECK(pkb->getRight(RelationshipType::NEXT, 5) == unordered_set<int>{6});
  CHECK(pkb->getRight(RelationshipType::NEXT, 6) == unordered_set<int>{3});
  CHECK(pkb->getRight(RelationshipType::NEXT, 7) == unordered_set<int>{8, 10});
  CHECK(pkb->getRight(RelationshipType::NEXT, 8) == unordered_set<int>{9});
  CHECK(pkb->getRight(RelationshipType::NEXT, 9) == unordered_set<int>{1});
  CHECK(pkb->getRight(RelationshipType::NEXT, 10) == unordered_set<int>{1});
  CHECK(pkb->getRight(RelationshipType::NEXT, 11) ==
        unordered_set<int>{});  // out of bound
}

TEST_CASE("[DE][Next R/S] nested while") {
  string program =
      "procedure a {                "
      "  while (x == 0) {           "  // 1
      "    while (x == 0) {         "  // 2
      "      while (x == 0) {       "  // 3
      "        while (x == 0) {     "  // 4
      "          while (x == 0) {   "  // 5
      "            x = 0;           "  // 6
      "          }                  "
      "        }                    "
      "      }                      "
      "    }                        "
      "  }                          "
      "}                            ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  CHECK(pkb->getRight(RelationshipType::NEXT, 0) ==
        unordered_set<int>{});  // out of bound
  CHECK(pkb->getRight(RelationshipType::NEXT, 1) == unordered_set<int>{2});
  CHECK(pkb->getRight(RelationshipType::NEXT, 2) == unordered_set<int>{1, 3});
  CHECK(pkb->getRight(RelationshipType::NEXT, 3) == unordered_set<int>{2, 4});
  CHECK(pkb->getRight(RelationshipType::NEXT, 4) == unordered_set<int>{3, 5});
  CHECK(pkb->getRight(RelationshipType::NEXT, 5) == unordered_set<int>{4, 6});
  CHECK(pkb->getRight(RelationshipType::NEXT, 6) == unordered_set<int>{5});
  CHECK(pkb->getRight(RelationshipType::NEXT, 7) ==
        unordered_set<int>{});  // out of bound
}

TEST_CASE("[DE][Next R/S] nested while and if") {
  string program =
      "procedure a {            "
      "  while (x == 0) {       "  // 1
      "    if (x == 0) then {   "  // 2
      "      while (x == 0) {   "  // 3
      "        x = 0;           "  // 4
      "      }                  "
      "    } else {             "
      "      while (x == 0) {   "  // 5
      "        x = 0;           "  // 6
      "      }                  "
      "    }                    "
      "  }                      "
      "}                        ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  CHECK(pkb->getRight(RelationshipType::NEXT, 0) ==
        unordered_set<int>{});  // out of bound
  CHECK(pkb->getRight(RelationshipType::NEXT, 1) == unordered_set<int>{2});
  CHECK(pkb->getRight(RelationshipType::NEXT, 2) == unordered_set<int>{3, 5});
  CHECK(pkb->getRight(RelationshipType::NEXT, 3) == unordered_set<int>{4, 1});
  CHECK(pkb->getRight(RelationshipType::NEXT, 4) == unordered_set<int>{3});
  CHECK(pkb->getRight(RelationshipType::NEXT, 5) == unordered_set<int>{6, 1});
  CHECK(pkb->getRight(RelationshipType::NEXT, 6) == unordered_set<int>{5});
  CHECK(pkb->getRight(RelationshipType::NEXT, 7) ==
        unordered_set<int>{});  // out of bound
}

TEST_CASE("[DE][NextBip R/S] sample source") {
  string program =
      "procedure Example {        "
      "  x = 2;                   "  // 1
      "  z = 3;                   "  // 2
      "  i = 5;                   "  // 3
      "  while (i!=0) {           "  // 4
      "    x = x - 1;             "  // 5
      "    if (x==1) then {       "  // 6
      "      z = x + 1; }         "  // 7
      "    else {                 "
      "      y = z + x; }         "  // 8
      "    z = z + x + i;         "  // 9
      "    call q;                "  // 10
      "    i = i - 1; }           "  // 11
      "  call p; }                "  // 12
      "                           "
      "procedure p {              "
      "  if (x<0) then {          "  // 13
      "    while (i>0) {          "  // 14
      "      x = z * 3 + 2 * y;   "  // 15
      "      call q;              "  // 16
      "      i = i - 1; }         "  // 17
      "    x = x + 1;             "  // 18
      "    z = x + z; }           "  // 19
      "  else {                   "
      "    z = 1; }               "  // 20
      "  z = z + x + i; }         "  // 21
      "                           "
      "procedure q {              "
      "  if (x==1) then {         "  // 22
      "    z = x + 1; }           "  // 23
      "  else {                   "
      "    x = z + x; }           "  // 24
      "}                          ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  // NextBip for non call stmts in Example
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 0) ==
        unordered_set<int>{});  // out of bound
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 1) == unordered_set<int>{2});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 2) == unordered_set<int>{3});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 3) == unordered_set<int>{4});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 4) ==
        unordered_set<int>{5, 12});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 5) == unordered_set<int>{6});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 6) ==
        unordered_set<int>{7, 8});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 7) == unordered_set<int>{9});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 8) == unordered_set<int>{9});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 9) == unordered_set<int>{10});

  // check NextBip between Example and q
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 10) ==
        unordered_set<int>{22});

  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 11) == unordered_set<int>{4});

  // check NextBip between Example and p
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 12) ==
        unordered_set<int>{13});

  // check NextBip in p
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 13) ==
        unordered_set<int>{14, 20});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 14) ==
        unordered_set<int>{15, 18});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 15) ==
        unordered_set<int>{16});

  // check NextBip between p and q
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 16) ==
        unordered_set<int>{22});

  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 17) ==
        unordered_set<int>{14});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 18) ==
        unordered_set<int>{19});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 19) ==
        unordered_set<int>{21});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 20) ==
        unordered_set<int>{21});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 21) ==
        unordered_set<int>{});  // end of p

  // check NextBip in q
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 22) ==
        unordered_set<int>{23, 24});

  // end of q, return to respective callers
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 23) ==
        unordered_set<int>{11, 17});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 24) ==
        unordered_set<int>{11, 17});

  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 25) ==
        unordered_set<int>{});  // out of bound

  // next neg tests
  REQUIRE(pkb->getRight(RelationshipType::NEXT, 10).count(22) == 0);
  REQUIRE(pkb->getRight(RelationshipType::NEXT, 12).count(13) == 0);
  REQUIRE(pkb->getRight(RelationshipType::NEXT, 16).count(22) == 0);
}

TEST_CASE("[DE][NextBip R/S] call is last stmt in procedure") {
  string program =
      "procedure a {      "
      "  x = 2;           "
      "  call b;          "  // 2
      "}                  "
      "procedure b {      "
      "  x = 2;           "  // 3
      "  x = 2;           "
      "  call c;          "  // 5
      "}                  "
      "procedure c {      "
      "  if (x==1) then { "  // 6
      "    z = x + 1; }   "  // 7
      "  else {           "
      "    x = z + x; } } ";  // 8

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  // out of range
  CHECK(pkb->getRight(RelationshipType::NEXT, -1) == unordered_set<int>{});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, -1) == unordered_set<int>{});
  CHECK(pkb->getRight(RelationshipType::NEXT, 0) == unordered_set<int>{});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 0) == unordered_set<int>{});

  // NextBip for a
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 1) == unordered_set<int>{2});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 2) == unordered_set<int>{3});
  // Next neg test
  CHECK(pkb->getRight(RelationshipType::NEXT, 2) == unordered_set<int>{});

  // NextBip for b
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 3) == unordered_set<int>{4});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 4) == unordered_set<int>{5});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 5) == unordered_set<int>{6});
  // Next neg test
  CHECK(pkb->getRight(RelationshipType::NEXT, 5) == unordered_set<int>{});

  // NextBip for c
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 6) ==
        unordered_set<int>{7, 8});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 7) == unordered_set<int>{});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 8) == unordered_set<int>{});

  // out of range
  CHECK(pkb->getRight(RelationshipType::NEXT, 9) == unordered_set<int>{});
  CHECK(pkb->getRight(RelationshipType::NEXT_BIP, 9) == unordered_set<int>{});
}

TEST_CASE("[DE][AffectsInfo] nested if") {
  string program =
      "procedure a {            "
      "  while (x == 0) {       "  // 1
      "    if (x == 0) then {   "  // 2
      "      if (y == 2) then { "  // 3
      "        x = 2; }         "  // 4
      "      else { y = 3; }    "  // 5
      "    } else {             "
      "      while (x == 0) {   "  // 6
      "        x = 0;           "  // 7
      "      }                  "
      "    }                    "
      "  }                      "
      "}                        ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  REQUIRE(pkb->getNextStmtForIfStmt(2) == 1);
  REQUIRE(pkb->getNextStmtForIfStmt(3) == 1);
  CHECK(pkb->getFirstStmtOfAllProcs() == vector<int>({1}));
}

TEST_CASE("[DE][AffectsInfo] non nested if at top") {
  string program =
      "procedure a {            "
      "    if (x == 0) then {   "  // 1
      "      y = 3;             "  // 2
      "    } else {             "
      "      z = x; }           "  // 3
      "    read y;              "  // 4
      "}                        ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  REQUIRE(pkb->getNextStmtForIfStmt(1) == 4);
  CHECK(pkb->getFirstStmtOfAllProcs() == vector<int>({1}));
}

TEST_CASE("[DE][AffectsInfo] non nested if at bottom") {
  string program =
      "procedure a {            "
      "    if (x == 0) then {   "  // 1
      "      y = 3;             "  // 2
      "    } else {             "
      "      z = x; }           "  // 3
      "}                        ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  REQUIRE(pkb->getNextStmtForIfStmt(1) == -1);
  CHECK(pkb->getFirstStmtOfAllProcs() == vector<int>({1}));
}

TEST_CASE("[DE][AffectsInfo] multiple procedures") {
  string program =
      "procedure a {            "
      "    x = 1;               "  // 1
      "}                        "
      "procedure b {            "
      "    y = 2;               "  // 2
      "    print y;             "  // 3
      "}                        "
      "procedure c {            "
      "    z = 3;               "  // 4
      "}                        ";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  auto allFirstStmts = pkb->getFirstStmtOfAllProcs();
  REQUIRE(set(allFirstStmts.begin(), allFirstStmts.end()) ==
          set<int>({1, 2, 4}));
}
