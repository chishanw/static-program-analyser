#include "ExprParser.h"

#include <Common/Global.h>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

ExprParser::ExprParser() {}

ArithAST* ExprParser::Parse(vector<string>::iterator* tokenIterator,
                            vector<string>::iterator tokenIteratorEnd) {
  if (*tokenIterator == tokenIteratorEnd) {
    throw runtime_error(
        "[ExprParser] an expression must have at least 1 token.");

    return nullptr;
  }

  // set up instance variables
  this->tokenIterator = tokenIterator;
  this->tokenIteratorEnd = tokenIteratorEnd;
  this->token = *(*tokenIterator);

  return expr();
}

// =======================================
//  expr
// =======================================

ArithAST* ExprParser::buildExprAST(
    ArithAST* leftNode,
    vector<pair<string, ArithAST*>>& listSignAndTerm) const {
  if (listSignAndTerm.empty()) {
    return leftNode;
  }

  for (vector<pair<string, ArithAST*>>::iterator it = listSignAndTerm.begin();
       it != listSignAndTerm.end(); ++it) {
    string sign = it->first;
    ArithAST* rightNode = it->second;
    ArithAST* newNode = new ArithAST(sign, leftNode, rightNode);
    leftNode = newNode;
  }

  return leftNode;
}

ArithAST* ExprParser::expr() {
  ArithAST* leftNode = term();
  vector<pair<string, ArithAST*>> listSignAndTerm = exprPrime();
  return buildExprAST(leftNode, listSignAndTerm);
}

vector<pair<string, ArithAST*>> ExprParser::exprPrime() {
  if (expectToken("+")) {
    consumeToken("+");
    ArithAST* exprAST = term();
    vector<pair<string, ArithAST*>> rest = exprPrime();
    rest.insert(rest.begin(), make_pair("+", exprAST));
    return rest;
  } else if (expectToken("-")) {
    consumeToken("-");
    ArithAST* exprAST = term();
    vector<pair<string, ArithAST*>> rest = exprPrime();
    rest.insert(rest.begin(), make_pair("-", exprAST));
    return rest;
  } else {
    vector<pair<string, ArithAST*>> res;
    return res;
  }
}

ArithAST* ExprParser::term() {
  ArithAST* leftNode = factor();
  vector<pair<string, ArithAST*>> listSignAndFactor = termPrime();
  return buildExprAST(leftNode, listSignAndFactor);
}

vector<pair<string, ArithAST*>> ExprParser::termPrime() {
  if (expectToken("*")) {
    consumeToken("*");
    ArithAST* factorAST = factor();
    vector<pair<string, ArithAST*>> rest = termPrime();
    rest.insert(rest.begin(), make_pair("*", factorAST));
    return rest;
  } else if (expectToken("/")) {
    consumeToken("/");
    ArithAST* factorAST = factor();
    vector<pair<string, ArithAST*>> rest = termPrime();
    rest.insert(rest.begin(), make_pair("/", factorAST));
    return rest;
  } else if (expectToken("%")) {
    consumeToken("%");
    ArithAST* factorAST = factor();
    vector<pair<string, ArithAST*>> rest = termPrime();
    rest.insert(rest.begin(), make_pair("%", factorAST));
    return rest;
  } else {
    vector<pair<string, ArithAST*>> res;
    return res;
  }
}

FactorAST* ExprParser::factor() {
  if (expectToken("(")) {
    consumeToken("(");
    ArithAST* exprAST = expr();
    consumeToken(")");
    return new FactorAST(exprAST);
  } else if (isName()) {
    Name varName = name();
    return new FactorAST(varName);
  } else if (isNumber()) {
    string constValue = number();
    return new FactorAST(constValue, true);
  } else {
    errorExpected("Left_Paren or Name or Number");
    return nullptr;  // won't reach this line
  }
}

// =======================================
//  name & number methods
// =======================================

Name ExprParser::name() {
  if (!isName()) {
    errorExpected("name");
    return "";
  }
  Name ret = token;
  nextToken();
  return ret;
}

string ExprParser::number() {
  if (!isNumber()) {
    errorExpected("number");
    return 0;
  }
  string currToken = token;
  nextToken();
  return currToken;
}

// =======================================
//  utility methods
// =======================================

bool ExprParser::expectToken(string expected) { return token == expected; }

void ExprParser::consumeToken(string toConsume) {
  if (!expectToken(toConsume)) {
    errorExpected(toConsume);
    return;
  }
  nextToken();
}

void ExprParser::nextToken() {
  if (noMoreToken()) {
    DMOprintErrMsgAndExit(
        "[ExprParser] no more token to parse but nextToken() was called");
    return;
  }

  *tokenIterator = (*tokenIterator) + 1;

  if (noMoreToken()) {
    // special value to indicate there's no more token
    // speical value should consist of invalid lexical tokens
    // to safely differentiate it from valid ones
    // i.e. mainly to prevent speical value to be interpreted as a Name
    token = "_END_OF_PROGRAM_";
  } else {
    token = *(*tokenIterator);
  }
}

bool ExprParser::noMoreToken() { return *tokenIterator == tokenIteratorEnd; }

bool ExprParser::isName() {
  if (token.size() == 0) {
    return false;
  }
  char c = token[0];
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool ExprParser::isNumber() {
  // assumes that tokenizer works properly, then only a Number can start with a
  // digit, and has only digits
  switch (token.size()) {
    case 0:
      return false;
    case 1:
      return '0' <= token[0] &&
             token[0] <= '9';  // '0' in ascii is 48 and '9' is 57
    default:
      // if constant has more than 1 digit, the 1st digit cannot be 0
      return '1' <= token[0] &&
             token[0] <= '9';  // '1' in ascii is 49 and '9' is 57
  }
}

void ExprParser::errorExpected(string expected) {
  stringstream exMsg;
  exMsg << "[ExprParser] Expected token '" << expected
        << "' but encoutered token: '" << token << "'";
  throw runtime_error(exMsg.str());
}
