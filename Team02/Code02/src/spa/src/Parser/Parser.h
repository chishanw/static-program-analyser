#pragma once
#include <string>
#include <utility>
#include <vector>

#include "Common/AST.h"

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
  ArithAST* expr();

  //  cond expr
  CondExprAST* condExpr();
  RelExprAST* relExpr();
  FactorAST* relFactor();

  //  name & number methods
  NAME name();

  // utility methods
  bool expectToken(std::string);
  void consumeToken(std::string);
  void nextToken();
  bool noMoreToken();
  void incrementStmtNo();
  bool isName();
  bool isRelExprInParens();
  bool isNextTokenEqualSign();
  void errorExpected(std::string);
};
