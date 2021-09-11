#include "QueryLexer.h"

#include <iostream>
#include <set>
#include <string>
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

bool QueryLexer::isStarSymbol(char c) { return c == '*'; }

void QueryLexer::consumeWhitespace() {
  optional<char> maybeNextChar = peekChar();
  while (maybeNextChar.has_value() && isspace(maybeNextChar.value())) {
    consumeChar();
    maybeNextChar = peekChar();
  }
}

char QueryLexer::consumeChar() {
  if (it == endIt) {
    throw runtime_error("QueryLexer expected another char but received None.");
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
  bool hasLetter = true;
  while (hasLetter) {
    char letter = consumeChar();
    nameOrKeyword.push_back(letter);

    optional<char> maybeNextChar = peekChar();
    hasLetter = maybeNextChar.has_value() && (isLetter(maybeNextChar.value()) ||
                                              isDigit(maybeNextChar.value()));
  }

  // Check if there is a keyword symbol like * at the end of a string
  optional<char> nextChar = peekChar();
  if (nextChar.has_value() && isStarSymbol(nextChar.value())) {
    char symbol = consumeChar();
    nameOrKeyword.push_back(symbol);
    return {TokenType::KEYWORD, nameOrKeyword};
  }

  return {TokenType::NAME_OR_KEYWORD, nameOrKeyword};
}

QueryToken QueryLexer::getInteger() {
  string integer;
  bool hasDigit = true;
  while (hasDigit) {
    char digit = consumeChar();
    integer.push_back(digit);

    optional<char> maybeNextChar = peekChar();
    hasDigit = maybeNextChar.has_value() && isDigit(maybeNextChar.value());
  }
  return {TokenType::INTEGER, integer};
}

QueryToken QueryLexer::getSymbol() {
  string symbol = {consumeChar()};
  return {TokenType::CHAR_SYMBOL, symbol};
}

vector<QueryToken> QueryLexer::Tokenize(const string& in) {
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
      std::cerr << "INVALID: " << nextChar
                << endl;  // TODO(Beatrice): remove for submission
      throw std::runtime_error(INVALID_TOKEN_MSG);
    }
    hasNextChar = peekChar().has_value();
  }
  return tokens;
}
