#pragma once
#include <string>
#include <utility>
#include <vector>

#include "AST.h"

class ExprParser {
 public:
  ExprParser();

  ArithAST* Parse(std::vector<std::string>::iterator*,
                  std::vector<std::string>::iterator);

 private:
  std::vector<std::string>::iterator* tokenIterator;
  std::vector<std::string>::iterator tokenIteratorEnd;
  std::string token;

  //  expr
  ArithAST* buildExprAST(ArithAST* leftNode,
                         std::vector<std::pair<std::string, ArithAST*>>&) const;
  ArithAST* expr();
  std::vector<std::pair<std::string, ArithAST*>> exprPrime();
  ArithAST* term();
  std::vector<std::pair<std::string, ArithAST*>> termPrime();
  FactorAST* factor();

  //  name & number methods
  Name name();
  std::string number();

  // utility methods
  bool expectToken(std::string);
  void consumeToken(std::string);
  void nextToken();
  bool noMoreToken();
  bool isName();
  bool isNumber();
  void errorExpected(std::string);
};
