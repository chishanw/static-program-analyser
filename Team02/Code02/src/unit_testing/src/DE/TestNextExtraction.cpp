
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

  CHECK(pkb->getNextStmts(0) == unordered_set<int>{});  // out of bound
  CHECK(pkb->getNextStmts(1) == unordered_set<int>{2});
  CHECK(pkb->getNextStmts(2) == unordered_set<int>{3});
  CHECK(pkb->getNextStmts(3) == unordered_set<int>{4, 7});
  CHECK(pkb->getNextStmts(4) == unordered_set<int>{5});
  CHECK(pkb->getNextStmts(5) == unordered_set<int>{6});
  CHECK(pkb->getNextStmts(6) == unordered_set<int>{3});
  CHECK(pkb->getNextStmts(7) == unordered_set<int>{8, 10});
  CHECK(pkb->getNextStmts(8) == unordered_set<int>{9});
  CHECK(pkb->getNextStmts(9) == unordered_set<int>{1});
  CHECK(pkb->getNextStmts(10) == unordered_set<int>{1});
  CHECK(pkb->getNextStmts(11) == unordered_set<int>{});  // out of bound
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

  CHECK(pkb->getNextStmts(0) == unordered_set<int>{});  // out of bound
  CHECK(pkb->getNextStmts(1) == unordered_set<int>{2});
  CHECK(pkb->getNextStmts(2) == unordered_set<int>{1, 3});
  CHECK(pkb->getNextStmts(3) == unordered_set<int>{2, 4});
  CHECK(pkb->getNextStmts(4) == unordered_set<int>{3, 5});
  CHECK(pkb->getNextStmts(5) == unordered_set<int>{4, 6});
  CHECK(pkb->getNextStmts(6) == unordered_set<int>{5});
  CHECK(pkb->getNextStmts(7) == unordered_set<int>{});  // out of bound
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

  CHECK(pkb->getNextStmts(0) == unordered_set<int>{});  // out of bound
  CHECK(pkb->getNextStmts(1) == unordered_set<int>{2});
  CHECK(pkb->getNextStmts(2) == unordered_set<int>{3, 5});
  CHECK(pkb->getNextStmts(3) == unordered_set<int>{4, 1});
  CHECK(pkb->getNextStmts(4) == unordered_set<int>{3});
  CHECK(pkb->getNextStmts(5) == unordered_set<int>{6, 1});
  CHECK(pkb->getNextStmts(6) == unordered_set<int>{5});
  CHECK(pkb->getNextStmts(7) == unordered_set<int>{});  // out of bound
}

TEST_CASE("[DE][NextBip R/S] sample source") {
  string program =
      "procedure Example {        "
      "  x = 2;                   "
      "  z = 3;                   "
      "  i = 5;                   "
      "  while (i!=0) {           "
      "    x = x - 1;             "
      "    if (x==1) then {       "
      "      z = x + 1; }         "
      "    else {                 "
      "      y = z + x; }         "
      "    z = z + x + i;         "
      "    call q;                "  // 10
      "    i = i - 1; }           "
      "  call p; }                "  // 12
      "                           "
      "procedure p {              "
      "  if (x<0) then {          "  // 13
      "    while (i>0) {          "
      "      x = z * 3 + 2 * y;   "
      "      call q;              "  // 16
      "      i = i - 1; }         "
      "    x = x + 1;             "
      "    z = x + z; }           "
      "  else {                   "
      "    z = 1; }               "
      "  z = z + x + i; }         "
      "                           "
      "procedure q {              "
      "  if (x==1) then {         "  // 22
      "    z = x + 1; }           "
      "  else {                   "
      "    x = z + x; } }         ";  // 24

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(program));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  // TODO(gf): when pkb is ready
  // CHECK(pkb->getNextBip(0) == unordered_set<int>{});  // out of bound
  // CHECK(pkb->getNextBip(10).count(22) > 0);
  // CHECK(pkb->getNextBip(12).count(13) > 0);
  // CHECK(pkb->getNextBip(16).count(22) > 0);
  // CHECK(pkb->getNext(10).count(22) == 0);
  // CHECK(pkb->getNext(12).count(13) == 0);
  // CHECK(pkb->getNext(16).count(22) == 0);
  // CHECK(pkb->getNextBip(25) == unordered_set<int>{});  // out of bound
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
