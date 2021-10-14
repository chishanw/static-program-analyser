#include "Parser.h"

#include <Common/ExprParser.h>
#include <Common/Global.h>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

Parser::Parser() : enableIter1restriction(false) {}

Parser::Parser(bool enableIter1restriction)
    : enableIter1restriction(enableIter1restriction) {}

ProgramAST* Parser::Parse(std::vector<std::string> tokens) {
  if (tokens.empty()) {
    throw runtime_error(
        "[Parser] a SIMPLE program must have at least 1 procedure.");

    vector<ProcedureAST*> emptyList;
    return new ProgramAST(emptyList);
  }

  // set up instance variables
  this->tokens = tokens;
  tokenIterator = (this->tokens).begin();

  token = *tokenIterator;
  prevStmtNo = 0;

  return program();
}

ProgramAST* Parser::program() {
  vector<ProcedureAST*> procedures;
  while (!noMoreToken()) {
    ProcedureAST* procedureAST = procedure();
    procedures.push_back(procedureAST);
  }

  // TODO(gf): rm this after iter1
  if (enableIter1restriction && procedures.size() > 1) {
    throw runtime_error(
        "[Parser] No more than 1 procedure is allowed in a SIMPLE program in "
        "iteration 1.");
  }

  ProgramAST* temp = new ProgramAST(procedures);
  return temp;
}

ProcedureAST* Parser::procedure() {
  consumeToken("procedure");
  NAME procName = name();
  consumeToken("{");
  vector<StmtAST*> stmtList = stmtLst();
  consumeToken("}");
  return new ProcedureAST(procName, stmtList);
}

vector<StmtAST*> Parser::stmtLst() {
  vector<StmtAST*> stmtList;
  while (!(expectToken("}") || noMoreToken())) {
    StmtAST* stmtAST = stmt();
    stmtList.push_back(stmtAST);
  }

  if (stmtList.size() == 0) {
    throw runtime_error("[Parser] a stmtLst must have at least 1 stmt.");
  }

  return stmtList;
}

StmtAST* Parser::stmt() {
  incrementStmtNo();

  if (isNextTokenEqualSign()) {
    // then it is an AssignStmt
    return assignStmt();
  } else if (expectToken("read")) {
    return readStmt();
  } else if (expectToken("print")) {
    return printStmt();
  } else if (expectToken("call")) {
    return callStmt();
  } else if (expectToken("while")) {
    return whileStmt();
  } else if (expectToken("if")) {
    return ifStmt();
  } else {
    errorExpected("any one of 'read print call while if' or a 'Name'");
    return nullptr;  // won't reach this line
  }
}

ReadStmtAST* Parser::readStmt() {
  consumeToken("read");
  NAME varName = name();
  consumeToken(";");
  return new ReadStmtAST(this->prevStmtNo, varName);
}

PrintStmtAST* Parser::printStmt() {
  consumeToken("print");
  NAME varName = name();
  consumeToken(";");
  return new PrintStmtAST(this->prevStmtNo, varName);
}

CallStmtAST* Parser::callStmt() {
  // TODO(gf): rm this after iter1
  if (enableIter1restriction) {
    throw runtime_error(
        "[Parser] Call Stmt is NOT allowed in a SIMPLE program in "
        "iteration 1.");
  }

  consumeToken("call");
  NAME procName = name();
  consumeToken(";");
  return new CallStmtAST(this->prevStmtNo, procName);
}

WhileStmtAST* Parser::whileStmt() {
  // need to rmb curr stmtNo cos calling stmtLst will increase prevStmtNo
  STMT_NO stmtNo = this->prevStmtNo;

  consumeToken("while");
  consumeToken("(");
  CondExprAST* condExprAST = condExpr();
  consumeToken(")");

  consumeToken("{");
  vector<StmtAST*> stmtList = stmtLst();
  consumeToken("}");

  return new WhileStmtAST(stmtNo, condExprAST, stmtList);
}

IfStmtAST* Parser::ifStmt() {
  STMT_NO stmtNo = this->prevStmtNo;

  consumeToken("if");
  consumeToken("(");
  CondExprAST* condExprAST = condExpr();
  consumeToken(")");

  consumeToken("then");
  consumeToken("{");
  vector<StmtAST*> thenBlock = stmtLst();
  consumeToken("}");

  consumeToken("else");
  consumeToken("{");
  vector<StmtAST*> elseBlock = stmtLst();
  consumeToken("}");

  return new IfStmtAST(stmtNo, condExprAST, thenBlock, elseBlock);
}

AssignStmtAST* Parser::assignStmt() {
  NAME varName = name();
  consumeToken("=");
  ArithAST* exprAST = expr();
  consumeToken(";");
  return new AssignStmtAST(this->prevStmtNo, varName, exprAST);
}

// =======================================
//  expr
// =======================================

ArithAST* Parser::expr() {
  ArithAST* exprAST = ExprParser().Parse(&tokenIterator, tokens.end());
  token = *(tokenIterator);
  return exprAST;
}

// =======================================
//  cond expr
// =======================================

CondExprAST* Parser::condExpr() {
  // cases:
  // ! ( cond_expr )
  // ( cond_expr ) && ( cond_expr )
  // ( cond_expr ) || ( cond_expr )
  // rel_expr
  if (expectToken("!")) {
    consumeToken("!");
    consumeToken("(");
    CondExprAST* left = condExpr();
    consumeToken(")");
    return new CondExprAST("!", left);

  } else if (expectToken("(") && isRelExprInParens()) {
    // open paren could be the starting of either cond_expr or rel_factor
    //
    // if it is a rel_expr until the close paren, then this is either
    // ( (cond_expr) && (cond_expr) )
    // or
    // ( (cond_expr) || (cond_expr) )
    //
    // otherwise it is an expr in parens, and this is the case
    // ( rel_expr )
    // e.g. ((x + z + i) > a)

    consumeToken("(");
    CondExprAST* left = condExpr();
    consumeToken(")");

    string sign;
    if (expectToken("&&")) {
      consumeToken("&&");
      sign = "&&";
    } else if (expectToken("||")) {
      consumeToken("||");
      sign = "||";
    } else {
      errorExpected("'&&' or '||'");
    }

    consumeToken("(");
    CondExprAST* right = condExpr();
    consumeToken(")");

    return new CondExprAST(sign, left, right);

  } else {
    RelExprAST* relExprAST = relExpr();
    return new CondExprAST(relExprAST);
  }
}

RelExprAST* Parser::relExpr() {
  // cases:
  // >= <= == !=
  // > <
  FactorAST* left = relFactor();

  string sign;
  if (expectToken(">=")) {
    sign = ">=";
  } else if (expectToken("<=")) {
    sign = "<=";
  } else if (expectToken("==")) {
    sign = "==";
  } else if (expectToken("!=")) {
    sign = "!=";
  } else if (expectToken(">")) {
    sign = ">";
  } else if (expectToken("<")) {
    sign = "<";
  } else {
    errorExpected("any one of '>= <= == != > <'");
    FactorAST* emptyAST = new FactorAST(nullptr);
    return new RelExprAST("", emptyAST, emptyAST);
  }

  consumeToken(sign);
  FactorAST* right = relFactor();
  return new RelExprAST(sign, left, right);
}

FactorAST* Parser::relFactor() {
  // the only diff between relFactor() and factor() is that relFactor() don't
  // consume open and close paren
  ArithAST* exprAST = expr();
  return new FactorAST(exprAST);
}

// =======================================
//  name & number methods
// =======================================

NAME Parser::name() {
  if (!isName()) {
    errorExpected("name");
    return "";
  }
  NAME ret = token;
  nextToken();
  return ret;
}

string Parser::number() {
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

bool Parser::expectToken(string expected) { return token == expected; }

void Parser::consumeToken(string toConsume) {
  if (!expectToken(toConsume)) {
    errorExpected(toConsume);
    return;
  }
  nextToken();
}

void Parser::nextToken() {
  if (noMoreToken()) {
    DMOprintErrMsgAndExit(
        "[Parser] no more token to parse but nextToken() was called");
    return;
  }

  tokenIterator++;

  if (noMoreToken()) {
    // special value to indicate there's no more token
    // speical value should consist of invalid lexical tokens
    // to safely differentiate it from valid ones
    // i.e. mainly to prevent speical value to be interpreted as a Name
    token = "_END_OF_PROGRAM_";
  } else {
    token = *tokenIterator;
  }
}

bool Parser::noMoreToken() { return tokenIterator == tokens.end(); }

void Parser::incrementStmtNo() { this->prevStmtNo++; }

bool Parser::isName() {
  if (token.size() == 0) {
    return false;
  }
  char c = token[0];
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool Parser::isNumber() {
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

bool Parser::isRelExprInParens() {
  // when this fucn is called, the current token should be "("
  if (token != "(") {
    DMOprintErrMsgAndExit(
        "[Parser] isRelExprInParens() asserts token to be ( but got " + token);
    return false;
  }

  auto tokenIteratorCopy = tokenIterator;
  int numOutstandingOpenParen = 0;

  while (tokenIteratorCopy != tokens.end()) {
    if (*tokenIteratorCopy == "(") {
      numOutstandingOpenParen++;
      tokenIteratorCopy++;
    } else if (*tokenIteratorCopy == ")") {
      numOutstandingOpenParen--;
      if (numOutstandingOpenParen == 0) return false;
      tokenIteratorCopy++;
    } else if (*tokenIteratorCopy == ">=") {
      return true;
    } else if (*tokenIteratorCopy == "<=") {
      return true;
    } else if (*tokenIteratorCopy == "==") {
      return true;
    } else if (*tokenIteratorCopy == "!=") {
      return true;
    } else if (*tokenIteratorCopy == ">") {
      return true;
    } else if (*tokenIteratorCopy == "<") {
      return true;
    } else {
      tokenIteratorCopy++;
    }
  }

  // consume all tokens and did not find comparison operators
  return false;
}

bool Parser::isNextTokenEqualSign() {
  string errMsg =
      "[Parser] expected another token but parser has reach the EOF.";

  if (noMoreToken()) {
    throw runtime_error(errMsg);
    return false;
  }

  auto tokenIteratorCopy = tokenIterator;
  tokenIteratorCopy++;
  if (tokenIteratorCopy == tokens.end()) {
    throw runtime_error(errMsg);
    return false;
  }

  return *tokenIteratorCopy == "=";
}

void Parser::errorExpected(string expected) {
  stringstream exMsg;
  exMsg << "[Parser] Expected token '" << expected
        << "' but encoutered token: '" << token << "'";
  throw runtime_error(exMsg.str());
}
