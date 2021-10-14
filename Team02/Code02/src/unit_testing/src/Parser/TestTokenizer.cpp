#include <Common/Tokenizer.h>

#include <vector>

#include "catch.hpp"

using namespace std;
using namespace Catch;

TEST_CASE("[Tokenizer] Whole Program Test") {
  string program =
      "procedure Example {\n"
      "  x = 2;\n"
      "  z = 3;\n"
      "  i = 5;\n"
      "  while (i!=0) {\n"
      "    x = x - 1;\n"
      "    if (x==1) then {\n"
      "      z = x + 1; }\n"
      "    else {\n"
      "      y = z + x; }\n"
      "    z = z + x + i;\n"
      "    call q;\n"
      "    i = i - 1; }\n"
      "  call p; }\n"
      "\n"
      "procedure p {\n"
      "  if (x<0) then {\n"
      "    while (i>0) {\n"
      "      x = z * 3 + 2 * y;\n"
      "      call q;\n"
      "      i = i - 1; }\n"
      "    x = x + 1;\n"
      "    z = x + z; }\n"
      "  else {\n"
      "    z = 1; }\n"
      "  z = z + x + i; }\n"
      "\n"
      "procedure q {\n"
      "  if (x==1) then {\n"
      "    z = x + 1; }\n"
      "  else {\n"
      "    x = z + x; } }\n"
      "\n";

  vector<string> expectedTokens = {
      "procedure", "Example",   "{", "x",    "=",         "2",    ";",
      "z",         "=",         "3", ";",    "i",         "=",    "5",
      ";",         "while",     "(", "i",    "!=",        "0",    ")",
      "{",         "x",         "=", "x",    "-",         "1",    ";",
      "if",        "(",         "x", "==",   "1",         ")",    "then",
      "{",         "z",         "=", "x",    "+",         "1",    ";",
      "}",         "else",      "{", "y",    "=",         "z",    "+",
      "x",         ";",         "}", "z",    "=",         "z",    "+",
      "x",         "+",         "i", ";",    "call",      "q",    ";",
      "i",         "=",         "i", "-",    "1",         ";",    "}",
      "call",      "p",         ";", "}",    "procedure", "p",    "{",
      "if",        "(",         "x", "<",    "0",         ")",    "then",
      "{",         "while",     "(", "i",    ">",         "0",    ")",
      "{",         "x",         "=", "z",    "*",         "3",    "+",
      "2",         "*",         "y", ";",    "call",      "q",    ";",
      "i",         "=",         "i", "-",    "1",         ";",    "}",
      "x",         "=",         "x", "+",    "1",         ";",    "z",
      "=",         "x",         "+", "z",    ";",         "}",    "else",
      "{",         "z",         "=", "1",    ";",         "}",    "z",
      "=",         "z",         "+", "x",    "+",         "i",    ";",
      "}",         "procedure", "q", "{",    "if",        "(",    "x",
      "==",        "1",         ")", "then", "{",         "z",    "=",
      "x",         "+",         "1", ";",    "}",         "else", "{",
      "x",         "=",         "z", "+",    "x",         ";",    "}",
      "}"};

  REQUIRE(Tokenizer::TokenizeProgramString(program) == expectedTokens);
}

TEST_CASE("[Tokenizer] Single Width Symbol Test") {
  string program = "{}()+-*/%!;";
  vector<string> expectedTokens = {
      "{", "}", "(", ")",       // brackets
      "+", "-", "*", "/", "%",  // arith
      "!",                      // cond_expr
      ";",                      // semi colon
  };
  REQUIRE(Tokenizer::TokenizeProgramString(program) == expectedTokens);
}

TEST_CASE("[Tokenizer] Double Width Symbol Test") {
  string program = "&&||>=<===!== ===";
  vector<string> expectedTokens = {
      "&&", "||", ">=", "<=", "==", "!=", "=", "==", "="};
  REQUIRE(Tokenizer::TokenizeProgramString(program) == expectedTokens);
}

TEST_CASE("[Tokenizer] All kinds of whitespaces test") {
  string program = "\f \n \r \t \t \v a";
  vector<string> expectedTokens = {"a"};
  REQUIRE(Tokenizer::TokenizeProgramString(program) == expectedTokens);
}

TEST_CASE("[Tokenizer] Exception Test") {
  SECTION("Invalid character") {
    string program = "`";

    REQUIRE_THROWS_WITH(Tokenizer::TokenizeProgramString(program),
                        StartsWith("[Tokenizer] Unrecognized token:"));
  }

  SECTION("Invalid DoubleWidthSymbol") {
    string program =
        "procedure Example {\n"
        "  if (x |& 1) then {\n"
        "  z = 3; } else {\n"
        "  i = 5;}\n"
        "  }\n";

    REQUIRE_THROWS_WITH(Tokenizer::TokenizeProgramString(program),
                        StartsWith("[Tokenizer] Expected next char to be"));
  }
}
