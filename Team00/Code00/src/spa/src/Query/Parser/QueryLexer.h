#pragma once

#include <optional>
#include <set>
#include <string>
#include <vector>

#include "QueryToken.h"

typedef std::basic_string<char>::const_iterator sIterator;
class QueryLexer {
 public:
  std::vector<qpp::QueryToken> Tokenize(const std::string&);
  QueryLexer();

 private:
  sIterator it;
  sIterator endIt;

  const std::set<char> ALLOWED_SYMBOL_SET = {'+', '-', '*', '/', '%', '_',
                                             '"', ',', ';', '(', ')'};

  bool isLetter(char);
  bool isDigit(char);
  bool isAllowedSymbol(char);
  bool isStarSymbol(char);
  char consumeChar();
  std::optional<char> peekChar();
  void consumeWhitespace();
  qpp::QueryToken getNameOrKeyword();
  qpp::QueryToken getInteger();
  qpp::QueryToken getSymbol();
};
