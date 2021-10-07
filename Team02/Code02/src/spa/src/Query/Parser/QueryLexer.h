#pragma once

#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "QueryLexerParserCommon.h"

typedef std::basic_string<char>::const_iterator sIterator;
class QueryLexer {
 public:
  inline static const std::string INVALID_TOKEN_MSG =
      "QueryLexer found an invalid token";
  inline static const std::string INVALID_INTEGER_START_ZERO_MSG =
      "QueryLexer expected a multiple digit integer to start with"
      " a non zero digit";

  QueryLexer();
  std::tuple<std::vector<qpp::QueryToken>, bool, std::string> Tokenize(
      const std::string&);

 private:
  sIterator it;
  sIterator endIt;

  // used to return FALSE for semantically invalid Select BOOLEAN clause
  bool isSemanticallyValid = true;
  std::string semanticErrorMsg;

  const std::set<char> ALLOWED_SYMBOL_SET = {'+', '-', '*', '/', '%', '_', '"',
                                             ',', ';', '(', ')', '<', '>'};

  bool isLetter(char);
  bool isDigit(char);
  bool isAllowedSymbol(char);
  bool isAllowedKeywordSymbol(char);
  char consumeChar();
  std::optional<char> peekChar();
  void consumeWhitespace();
  qpp::QueryToken getNameOrKeyword();
  qpp::QueryToken getInteger();
  qpp::QueryToken getSymbol();
};
