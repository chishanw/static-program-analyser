#pragma once

#include <optional>
#include <set>
#include <string>
#include <vector>

#include "QueryToken.h"

typedef std::basic_string<char>::const_iterator sIterator;
class QueryLexer {
 public:
  inline static const std::string INVALID_TOKEN_MSG =
      "QueryLexer found an invalid token";
  inline static const std::string INVALID_INTEGER_START_ZERO_MSG =
      "QueryLexer expected a multiple digit integer to start with"
      " a non zero digit";

  QueryLexer();
  std::vector<qpp::QueryToken> Tokenize(const std::string&);

 private:
  sIterator it;
  sIterator endIt;

  const std::set<char> ALLOWED_SYMBOL_SET = {'+', '-', '*', '/', '%', '_',
                                             '"', ',', ';', '(', ')'};

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
