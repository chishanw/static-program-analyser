#pragma once

#include <optional>
#include <string>
#include <vector>

#include "QueryToken.h"

typedef std::basic_string<char>::const_iterator sIterator;
class QueryLexer {
 public:
  static std::vector<qpp::QueryToken> Tokenize(const std::string&);

 private:
  static bool isLetter(char);
  static bool isDigit(char);
  static bool isAllowedSymbol(char);
  static bool isKeywordSymbol(char);
  static char consumeChar(sIterator&, sIterator&);
  static std::optional<char> peekChar(sIterator&, sIterator&);
  static void consumeWhitespace(sIterator&, sIterator&);
  static qpp::QueryToken getNameOrKeyword(sIterator&, sIterator&);
  static qpp::QueryToken getInteger(sIterator&, sIterator&);
  static qpp::QueryToken getSymbol(sIterator&, sIterator&);
};
