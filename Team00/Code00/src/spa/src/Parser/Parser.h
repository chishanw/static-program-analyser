#pragma once
#include <string>
#include <utility>
#include <vector>

#include "AST.h"

class Parser {
 public:
  Parser();
  explicit Parser(bool);

  ProgramAST* Parse(std::vector<std::string>);

 private:
  const bool enableIter1restriction;

  std::vector<std::string> tokens;
  std::vector<std::string>::iterator tokenIterator;
  std::string token;

  int prevStmtNo;

  ProgramAST* program();
  ProcedureAST* procedure();
  std::vector<StmtAST*> stmtLst();
  StmtAST* stmt();
  ReadStmtAST* readStmt();
  PrintStmtAST* printStmt();
  CallStmtAST* callStmt();
  WhileStmtAST* whileStmt();
  IfStmtAST* ifStmt();
  AssignStmtAST* assignStmt();

  //  expr
  ArithAST* buildExprAST(ArithAST* leftNode,
                         std::vector<std::pair<std::string, ArithAST*>>&) const;
  ArithAST* expr();
  std::vector<std::pair<std::string, ArithAST*>> exprPrime();
  ArithAST* term();
  std::vector<std::pair<std::string, ArithAST*>> termPrime();
  FactorAST* factor();

  //  cond expr
  CondExprAST* condExpr();
  RelExprAST* relExpr();

  //  name & number methods
  NAME name();
  int number();

  // utility methods
  bool expectToken(std::string);
  void consumeToken(std::string);
  void nextToken();
  bool noMoreToken();
  void incrementStmtNo();
  bool isName();
  bool isNumber();
  bool isNumber(std::string);
  int stringToInt(std::string);
  void errorExpected(std::string);
};
