
#include <Parser/Parser.h>
#include <Common/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("[Parser] semantic errors") {
  SECTION("<1 procedure") {
    string program = "    ";
    REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
  }

  SECTION("<1 stmt in procedure stmtLst") {
    string program =
        "procedure Exmaple {"
        ""
        "}";

    REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
  }

  SECTION("<1 stmt in whileStmt stmtLst") {
    string program =
        "procedure Exmaple {"
        "  while (a>1) {"
        "               "
        "  }"
        "}";

    REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
  }

  SECTION("<1 stmt in ifStmt stmtLst") {
    SECTION("<1 stmt in ifStmt stmtLst then block") {
      string program =
          "procedure Exmaple {    "
          "  if (a>1) then {      "
          "                       "
          "  } else {             "
          "    a = 1;             "
          "  }                    "
          "}                      ";
      REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }

    SECTION("<1 stmt in ifStmt stmtLst else block") {
      string program =
          "procedure Exmaple {    "
          "  if (a>1) then {      "
          "    a = 1;             "
          "  } else {             "
          "                       "
          "  }                    "
          "}                      ";
      REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }

    SECTION("<1 stmt in ifStmt stmtLst both block") {
      string program =
          "procedure Exmaple {    "
          "  if (a>1) then {      "
          "                       "
          "  } else {             "
          "                       "
          "  }                    "
          "}                      ";
      REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }
  }
}

TEST_CASE("[Parser] using keywords as ProcName and VarName") {
  string program =
      "procedure if {                                             "
      "  if ((!(if == else)) || (else == (then * print))) then {  "
      "    read = 1;                                              "
      "  } else {                                                 "
      "    print = 2;                                             "
      "  }                                                        "
      "  call = 3;                                                "
      "  while = 4;                                               "
      "  if = 5;                                                  "
      "}                                                          ";
  REQUIRE_NOTHROW(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
}

TEST_CASE("[Parser] stmt() parser look ahead out of bound error") {
  string program =
      "procedure if {                                           "
      "  read = 1;                                              "
      "  if                                                     ";
  REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
}

TEST_CASE("[Parser] parsing INTEGER") {
  SECTION("big number") {
    string program =
        "procedure p {"
        "    p = 1234567890123456789012345678901234567890123456789012345678; "
        "}";
    REQUIRE_NOTHROW(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
  }

  SECTION("number has > 1 digit and it starts with 0") {
    string program =
        "procedure p {"
        "    p = 01234; "
        "}";
    REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
  }

  SECTION("number has only 1 digit and it starts with 0") {
    string program =
        "procedure p {"
        "    p = 0; "
        "}";
    REQUIRE_NOTHROW(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
  }
}

TEST_CASE("[Parser] cond_expr tests") {
  // =============================
  // Cases that should throw
  // =============================

  SECTION("Cases that should throw") {
    SECTION("baseline case: missing token") {
      string program =
          "procedure Example {\n"
          "  if ((a > a) && ) then { \n"
          "    print b; \n"
          "  } else { \n"
          "    print b;  \n"
          "  }  \n"
          "}";
      REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }

    SECTION("empty cond_expr") {
      string program =
          "procedure Example {\n"
          "  if () then { \n"
          "    print b; \n"
          "  } else { \n"
          "    print b;  \n"
          "  }  \n"
          "}";
      REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }

    SECTION(
        "only 1 rel_expr in if stmt but has redundant parens around the whole "
        "rel_expr") {
      string program =
          "procedure Example {\n"
          "  if ((x>1)) then { \n"
          "    print b; \n"
          "  } else { \n"
          "    print b;  \n"
          "  }  \n"
          "}";
      REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }

    SECTION(
        "complex if stmt that has 3 cond_expr and 2 '&&' or '||' operator in "
        "between, basically not enough parens") {
      string program =
          "procedure ComplexIfStmt {   \n"
          "    if ((((20) + ((a - b) * ((c / 30 % d)))) || (10 == a) || (!(20 "
          "!= b))) && ((c < 10) && ((a <= 10) || (b > 20))) || (c >= 20)) then "
          "{"
          "        x = 20;   \n"
          "    } else {   \n"
          "        x = 0;   \n"
          "    }   \n"
          "}   \n";
      REQUIRE_THROWS(Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }
  }

  // =============================
  // Cases that should NOT throw
  // =============================

  SECTION("Cases that should NOT throw") {
    SECTION("only 1 rel_expr in paren") {
      string program =
          "procedure Example {\n"
          "  if ((x + z + i) > a) then { \n"
          "    print b; \n"
          "  } else { \n"
          "    print b;  \n"
          "  }  \n"
          "}";
      REQUIRE_NOTHROW(
          Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }

    SECTION(
        "only 1 rel_expr in if stmt but has redundant parens inside the "
        "rel_expr") {
      string program =
          "procedure Example {\n"
          "  if (((((x + ((z)) + i)))) > (((a)))) then { \n"
          "    print b; \n"
          "  } else { \n"
          "    print b;  \n"
          "  }  \n"
          "}";
      REQUIRE_NOTHROW(
          Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }

    SECTION("complex if stmt") {
      string program =
          "procedure ComplexIfStmt {   \n"
          "    if ((b >= 0) && ((y < 0) || (x != 10))) then {   \n"
          "        x = 20;   \n"
          "    } else {   \n"
          "        x = 0;   \n"
          "    }   \n"
          "    if ((!(p == 4)) || (a == (p * 2))) then {   \n"
          "        p = a * 2;   \n"
          "    } else {   \n"
          "        a = p * 3;   \n"
          "    }   \n"
          "}   \n";
      REQUIRE_NOTHROW(
          Parser().Parse(Tokenizer::TokenizeProgramString(program)));
    }
  }
}
