#include "QueryLexer.h"

#include <set>
#include <vector>

using namespace std;
using namespace qpp;

QueryLexer::QueryLexer() = default;

bool QueryLexer::isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool QueryLexer::isDigit(char c) { return c >= '0' && c <= '9'; }

bool QueryLexer::isAllowedSymbol(char c) {
  return ALLOWED_SYMBOL_SET.find(c) != ALLOWED_SYMBOL_SET.end();
}

bool QueryLexer::isAllowedKeywordSymbol(char c) {
  return c == '*' || c == '_' || c == '#';
}

void QueryLexer::consumeWhitespace() {
  optional<char> maybeNextChar = peekChar();
  while (maybeNextChar.has_value() && isspace(maybeNextChar.value())) {
    consumeChar();
    maybeNextChar = peekChar();
  }
}

char QueryLexer::consumeChar() {
  if (it == endIt) {
    throw qpp::SyntacticErrorException(
        "QueryLexer expected another char but received None.");
  }
  char current = *it;
  ++it;
  return current;
}

optional<char> QueryLexer::peekChar() {
  if (it == endIt) {
    return nullopt;
  }
  return {*it};
}

QueryToken QueryLexer::getNameOrKeyword() {
  string nameOrKeyword;
  // type is NAME_OR_KEYWORD if it passes NAME validation checks
  // type is KEYWORD if it has * or _ symbol
  TokenType type = TokenType::NAME_OR_KEYWORD;

  bool hasNextNameOrKeyword = true;
  while (hasNextNameOrKeyword) {
    char c = consumeChar();
    if (isAllowedKeywordSymbol(c)) {
      // if it is a symbol, it must be a KEYWORD
      type = TokenType::KEYWORD;
    }
    nameOrKeyword.push_back(c);

    // checks if the next character is a continuation of the NAME or KEYWORD
    if (!peekChar().has_value()) {
      hasNextNameOrKeyword = false;
    } else {
      char next = peekChar().value();
      hasNextNameOrKeyword =
          isLetter(next) || isDigit(next) || isAllowedKeywordSymbol(next);
    }
  }
  return {type, nameOrKeyword};
}

QueryToken QueryLexer::getInteger() {
  string integer;
  // get first digit
  char firstDigit = consumeChar();
  integer.push_back(firstDigit);

  // validate that the first digit is not a 0 if there are subsequent digits
  bool isFirstDigitZero = firstDigit == '0';
  bool hasDigit = peekChar().has_value() && isDigit(peekChar().value());
  if (hasDigit && isFirstDigitZero) {
    isSemanticallyValid = false;  // semantic error
    semanticErrorMsg = INVALID_INTEGER_START_ZERO_MSG;
  }

  // otherwise, first digit is not 0
  // subsequent digits are concatenated to the integer
  while (hasDigit) {
    char digit = consumeChar();
    integer.push_back(digit);

    hasDigit = peekChar().has_value() && isDigit(peekChar().value());
  }
  return {TokenType::INTEGER, integer};
}

QueryToken QueryLexer::getSymbol() {
  string symbol = {consumeChar()};
  return {TokenType::CHAR_SYMBOL, symbol};
}

tuple<vector<QueryToken>, bool, string> QueryLexer::Tokenize(const string& in) {
  vector<QueryToken> tokens;

  it = in.begin();
  endIt = in.end();

  bool hasNextChar = peekChar().has_value();
  while (hasNextChar) {
    consumeWhitespace();
    if (!peekChar().has_value()) {
      break;
    }

    char nextChar = peekChar().value();

    if (isAllowedSymbol(nextChar)) {
      tokens.push_back(getSymbol());

    } else if (isLetter(nextChar)) {
      tokens.push_back(getNameOrKeyword());

    } else if (isDigit(nextChar)) {
      tokens.push_back(getInteger());

    } else {
      throw qpp::SyntacticErrorException(INVALID_TOKEN_MSG);
    }
    hasNextChar = peekChar().has_value();
  }
  return {tokens, isSemanticallyValid, semanticErrorMsg};
}
