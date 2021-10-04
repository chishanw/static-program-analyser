#include <Query/Parser/QueryLexer.h>
#include <Query/Parser/QueryToken.h>

#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

// ===================== Testing synonym declarations =====================

TEST_CASE("Query with valid synonym types are tokenized successfully") {
  string validQuery =
      "stmt s, s1, s2; read r; print p; while w; if i; assign a; variable v; "
      "constant c; procedure p; call ca; prog_line n;"
      "Select s such that Follows(1, 2)";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "stmt"},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "s1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "s2"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "read"},
      {qpp::TokenType::NAME_OR_KEYWORD, "r"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "print"},
      {qpp::TokenType::NAME_OR_KEYWORD, "p"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "while"},
      {qpp::TokenType::NAME_OR_KEYWORD, "w"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "if"},
      {qpp::TokenType::NAME_OR_KEYWORD, "i"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "assign"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "variable"},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "constant"},
      {qpp::TokenType::NAME_OR_KEYWORD, "c"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "procedure"},
      {qpp::TokenType::NAME_OR_KEYWORD, "p"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "call"},
      {qpp::TokenType::NAME_OR_KEYWORD, "ca"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::KEYWORD, "prog_line"},
      {qpp::TokenType::NAME_OR_KEYWORD, "n"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

// ============================ Testing such that ===========================

TEST_CASE(
    "Query with valid such that clauses for different relationships are "
    "tokenized successfully") {
  string validQuery =
      "stmt s;"
      "Select s "
      "such that Follows(1, 2)"
      "such that Follows*(1, 2)"
      "such that Parent(1, 2)"
      "such that Parent*(1, 2)"
      "such that Uses(1, 2)"
      "such that Modifies(1, 2)";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "stmt"},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::KEYWORD, "Follows*"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Parent"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::KEYWORD, "Parent*"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Uses"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Modifies"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

TEST_CASE(
    "Query with valid such that clauses for different parameters are tokenized "
    "successfully") {
  string validQuery =
      "stmt s, s1;"
      "Select s "
      "such that Follows(1, 2)"
      "such that Follows(1, s)"
      "such that Follows(1, _)"
      "such that Follows(_, 2)"
      "such that Follows(_, s)"
      "such that Follows(_, _)"
      "such that Follows(s, 2)"
      "such that Follows(s, s1)"
      "such that Follows(s, _)";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "stmt"},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "s1"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "s1"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::NAME_OR_KEYWORD, "s"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

// ============================ Testing pattern ===========================

TEST_CASE("Query with valid pattern clauses are tokenized correctly") {
  string validQuery =
      "assign a; variable v;"
      "Select a "
      "pattern a(\"v\", _\"x\"_)"
      "pattern a(v, _\"x\"_)"
      "pattern a(v, \"x\")"
      "pattern a(v, _)";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "assign"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "variable"},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::NAME_OR_KEYWORD, "pattern"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::NAME_OR_KEYWORD, "x"},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "pattern"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::NAME_OR_KEYWORD, "x"},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "pattern"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::NAME_OR_KEYWORD, "x"},
      {qpp::TokenType::CHAR_SYMBOL, "\""},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "pattern"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

// ============================ Testing multi-clause ===========================

TEST_CASE(
    "Query with valid such that clause and pattern clause are tokenized "
    "correctly") {
  string validQuery =
      "assign a; variable v;"
      "Select a "
      "such that Follows*(1, 2)"
      "pattern a(v, _)";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "assign"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "variable"},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::KEYWORD, "Follows*"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "pattern"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::CHAR_SYMBOL, "_"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

// ============================ Testing whitespace ===========================

TEST_CASE(
    "Whitespaces between different tokens for such that clause are ignored") {
  string validQuery =
      "assign a,a1          ;variable v;    "
      "     Select   a "
      "such       that     Follows*        (   1,2)     ";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "assign"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "a1"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "variable"},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::KEYWORD, "Follows*"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

TEST_CASE("Whitespaces in one keyword is tokenized into separate tokens") {
  string invalidWhitespaceInKeyword =
      "ass     i gn a,a1          ;variab le v;    "
      "     Se lect   a "
      "su ch       t hat     Follows       *        (   1,2)     "
      "such       that     Parent    *        (   1,2)     ";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "ass"},
      {qpp::TokenType::NAME_OR_KEYWORD, "i"},
      {qpp::TokenType::NAME_OR_KEYWORD, "gn"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::NAME_OR_KEYWORD, "a1"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "variab"},
      {qpp::TokenType::NAME_OR_KEYWORD, "le"},
      {qpp::TokenType::NAME_OR_KEYWORD, "v"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Se"},
      {qpp::TokenType::NAME_OR_KEYWORD, "lect"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::NAME_OR_KEYWORD, "su"},
      {qpp::TokenType::NAME_OR_KEYWORD, "ch"},
      {qpp::TokenType::NAME_OR_KEYWORD, "t"},
      {qpp::TokenType::NAME_OR_KEYWORD, "hat"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "*"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Parent"},
      {qpp::TokenType::CHAR_SYMBOL, "*"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens =
      QueryLexer().Tokenize(invalidWhitespaceInKeyword);

  REQUIRE(actualTokens == expectedTokens);
}

// ============================ Testing keywords ===========================

TEST_CASE("Tokenization of Follows * and Follows* should be different") {
  string validFollows = "Follows*";
  string invalidFollows = "Follows *";

  vector<qpp::QueryToken> validFollowsTokens =
      QueryLexer().Tokenize(validFollows);
  vector<qpp::QueryToken> invalidFollowsTokens =
      QueryLexer().Tokenize(invalidFollows);

  REQUIRE(validFollowsTokens != invalidFollowsTokens);
}

TEST_CASE("Query with invalid character throws") {
  string invalidQuery = "ass!gn a; Select a";

  REQUIRE_THROWS_WITH(QueryLexer().Tokenize(invalidQuery),
                      QueryLexer::INVALID_TOKEN_MSG);
}

// ============================ Testing integer ===========================

TEST_CASE("Large integer is parsed successfully") {
  string validQuery =
      "assign a;"
      "Select a such that Follows(1, 2200000000)";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "assign"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "2200000000"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

TEST_CASE("Zero value integer is parsed successfully") {
  string validQuery =
      "assign a;"
      "Select a such that Follows(1, 0)";

  vector<qpp::QueryToken> expectedTokens = {
      {qpp::TokenType::NAME_OR_KEYWORD, "assign"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::CHAR_SYMBOL, ";"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Select"},
      {qpp::TokenType::NAME_OR_KEYWORD, "a"},
      {qpp::TokenType::NAME_OR_KEYWORD, "such"},
      {qpp::TokenType::NAME_OR_KEYWORD, "that"},
      {qpp::TokenType::NAME_OR_KEYWORD, "Follows"},
      {qpp::TokenType::CHAR_SYMBOL, "("},
      {qpp::TokenType::INTEGER, "1"},
      {qpp::TokenType::CHAR_SYMBOL, ","},
      {qpp::TokenType::INTEGER, "0"},
      {qpp::TokenType::CHAR_SYMBOL, ")"}};

  vector<qpp::QueryToken> actualTokens = QueryLexer().Tokenize(validQuery);

  REQUIRE(actualTokens == expectedTokens);
}

TEST_CASE("Query with invalid integer throws") {
  string invalidQuery =
      "assign a;"
      "Select a such that Follows(0123, 4567)";

  REQUIRE_THROWS_WITH(QueryLexer().Tokenize(invalidQuery),
                      QueryLexer::INVALID_INTEGER_START_ZERO_MSG);
}
