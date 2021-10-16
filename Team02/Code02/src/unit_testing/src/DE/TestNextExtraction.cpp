
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
