#pragma once
#include <string>
#include <utility>
#include <vector>

#include "AST.h"

class Parser {
 public:
  ProgramAST* Parse(std::vector<std::string>);

 private:
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
  ExprAST* buildExprAST(ExprAST* leftNode,
                        std::vector<std::pair<std::string, ExprAST*>>&) const;
  ExprAST* expr();
  std::vector<std::pair<std::string, ExprAST*>> exprPrime();
  ExprAST* term();
  std::vector<std::pair<std::string, ExprAST*>> termPrime();
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
