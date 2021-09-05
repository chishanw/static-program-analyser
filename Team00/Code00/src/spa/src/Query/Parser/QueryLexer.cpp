#include "QueryLexer.h"

#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace qpp;

bool QueryLexer::isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool QueryLexer::isDigit(char c) { return c >= '0' && c <= '9'; }

bool QueryLexer::isAllowedSymbol(char c) {
  const set<char> ALLOWED_SYMBOL_SET = {'+', '-', '*', '/', '%', '_',
                                        '"', ',', ';', '(', ')'};
  return ALLOWED_SYMBOL_SET.find(c) != ALLOWED_SYMBOL_SET.end();
}

// Symbols used in keywords, like * in Follows*
bool QueryLexer::isKeywordSymbol(char c) { return c == '*'; }

void QueryLexer::consumeWhitespace(sIterator& it, sIterator& endIt) {
  while (isspace(peekChar(it, endIt).value())) {
    consumeChar(it, endIt);
  }
}

char QueryLexer::consumeChar(sIterator& it, sIterator& endIt) {
  if (it == endIt) {
    throw runtime_error("QueryLexer expected another char but received None.");
  }
  char current = *it;
  ++it;
  return current;
}

optional<char> QueryLexer::peekChar(sIterator& it, sIterator& endIt) {
  if (it == endIt) {
    return nullopt;
  }
  return {*it};
}

QueryToken QueryLexer::getNameOrKeyword(sIterator& it, sIterator& endIt) {
  string nameOrKeyword;
  bool hasLetter = true;
  while (hasLetter) {
    char letter = consumeChar(it, endIt);
    nameOrKeyword.push_back(letter);

    optional<char> nextChar = peekChar(it, endIt);
    hasLetter = nextChar.has_value() &&
                (isLetter(nextChar.value()) || isDigit(nextChar.value()));
  }

  // Check if there is a keyword symbol like * at the end of a string
  optional<char> nextChar = peekChar(it, endIt);
  if (nextChar.has_value() && isKeywordSymbol(nextChar.value())) {
    char symbol = consumeChar(it, endIt);
    nameOrKeyword.push_back(symbol);
    return {TokenType::KEYWORD, nameOrKeyword };
  }

  return {TokenType::NAME, nameOrKeyword };
}

QueryToken QueryLexer::getInteger(sIterator& it, sIterator& endIt) {
  string integer;
  bool hasDigit = true;
  while (hasDigit) {
    char digit = consumeChar(it, endIt);
    integer.push_back(digit);

    optional<char> nextChar = peekChar(it, endIt);
    hasDigit = nextChar.has_value() && isDigit(nextChar.value());
  }
  return {TokenType::INTEGER, integer};
}

QueryToken QueryLexer::getSymbol(sIterator& it, sIterator& endIt) {
  string symbol = {consumeChar(it, endIt)};
  return {TokenType::CHAR_SYMBOL, symbol};
}

vector<QueryToken> QueryLexer::Tokenize(const string& in) {
  vector<QueryToken> tokens;

  auto it = in.begin();
  auto endIt = in.end();

  bool hasNextChar = peekChar(it, endIt).has_value();
  while (hasNextChar) {
    consumeWhitespace(it, endIt);
    char nextChar = peekChar(it, endIt).value();

    if (isAllowedSymbol(nextChar)) {
      tokens.push_back(getSymbol(it, endIt));

    } else if (isLetter(nextChar)) {
      tokens.push_back(getNameOrKeyword(it, endIt));

    } else if (isDigit(nextChar)) {
      tokens.push_back(getInteger(it, endIt));

    } else {
      std::cerr << "INVALID: " << nextChar
                << endl;  // TODO(Beatrice): remove for submission
      throw std::runtime_error("QueryLexer found an Invalid token");
    }
    hasNextChar = peekChar(it, endIt).has_value();
  }
  return tokens;
}
