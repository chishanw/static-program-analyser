#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Common/global.h"

typedef int STMT_NO;
typedef std::string NAME;

class ArithAST {
 public:
  std::string Sign;
  ArithAST* LeftNode;
  ArithAST* RightNode;

  ArithAST() = default;  // default constructor for child class

  explicit ArithAST(ArithAST* leftNode)
      : hasOnlyOneNode(true), Sign(""), LeftNode(leftNode), RightNode(nullptr) {
    // TODO(gf): clean up
    // std::cout << "1N_Arith is called: " << leftNode << std::endl;
  }

  ArithAST(std::string sign, ArithAST* leftNode, ArithAST* rightNode)
      : hasOnlyOneNode(false),
        Sign(sign),
        LeftNode(leftNode),
        RightNode(rightNode) {
    // TODO(gf): clean up
    // std::cout << "2N_Arith is called: " << leftNode << std::endl;
  }

  virtual ~ArithAST() {}

  bool HasOnlyOneNode() const { return this->hasOnlyOneNode; }
  std::vector<std::string> GetAllPatternStr();
  std::string GetPatternStr();
  std::string GetDebugStr();

  //   friend std::ostream& operator<<(std::ostream& out, ArithAST const& obj);

 private:
  bool hasOnlyOneNode;
};

class FactorAST : public ArithAST {
 public:
  NAME VarName;
  int ConstValue;
  ArithAST* Expr;

  explicit FactorAST(NAME varName)
      : isVarName(true),
        isConstValue(false),
        isExpr(false),
        VarName(varName),
        ConstValue(0),
        Expr(nullptr) {}

  explicit FactorAST(int constValue)
      : isVarName(false),
        isConstValue(true),
        isExpr(false),
        VarName(""),
        ConstValue(constValue),
        Expr(nullptr) {}

  explicit FactorAST(ArithAST* expr)
      : isVarName(false),
        isConstValue(false),
        isExpr(true),
        VarName(""),
        ConstValue(0),
        Expr(expr) {}

  bool IsVarName() { return this->isVarName; }
  bool IsConstValue() { return this->isConstValue; }
  bool IsExpr() { return this->isExpr; }
  std::vector<std::string> GetAllPatternStr();
  std::string GetPatternStr();
  std::string GetDebugStr();

  //   friend std::ostream& operator<<(std::ostream& out, FactorAST const& obj);

 private:
  bool isVarName;
  bool isConstValue;
  bool isExpr;
};

class RelExprAST {
 public:
  std::string Sign;
  FactorAST* LeftNode;  // Factor and RelFactor are almost the same thing except
  FactorAST* RightNode;  // for the bracket expr
  RelExprAST(std::string sign, FactorAST* leftNode, FactorAST* rightNode)
      : Sign(sign), LeftNode{leftNode}, RightNode{rightNode} {}
};

class CondExprAST {
 public:
  std::string Sign;
  RelExprAST* RelExpr;
  CondExprAST* LeftNode;
  CondExprAST* RightNode;

  explicit CondExprAST(RelExprAST* relExpr)
      : hasOnlyOneRelExpr(true),
        hasOnlyOneCondExpr(false),
        Sign(""),
        RelExpr(relExpr),
        LeftNode(nullptr),
        RightNode(nullptr) {}

  CondExprAST(std::string sign, CondExprAST* condExpr)
      : hasOnlyOneRelExpr(false),
        hasOnlyOneCondExpr(true),
        Sign(sign),
        RelExpr(nullptr),
        LeftNode(condExpr),
        RightNode(nullptr) {}

  CondExprAST(std::string sign, CondExprAST* leftNode, CondExprAST* rightNode)
      : hasOnlyOneRelExpr(false),
        hasOnlyOneCondExpr(false),
        Sign(sign),
        LeftNode(leftNode),
        RightNode(rightNode) {}

  bool HasOnlyOneRelExpr() { return this->hasOnlyOneRelExpr; }
  bool HasOnlyOneCondExpr() { return this->hasOnlyOneCondExpr; }

 private:
  bool hasOnlyOneRelExpr;
  bool hasOnlyOneCondExpr;
};

class StmtAST {
 public:
  STMT_NO StmtNo;
  virtual ~StmtAST() {}

 protected:
  explicit StmtAST(STMT_NO stmtNo) : StmtNo(stmtNo) {}
};

class ReadStmtAST : public StmtAST {
 public:
  NAME VarName;
  ReadStmtAST(STMT_NO stmtNo, NAME varName)
      : StmtAST(stmtNo), VarName(varName) {}
};

class PrintStmtAST : public StmtAST {
 public:
  NAME VarName;
  PrintStmtAST(STMT_NO stmtNo, NAME varName)
      : StmtAST(stmtNo), VarName(varName) {}
};

class CallStmtAST : public StmtAST {
 public:
  NAME ProcName;
  CallStmtAST(STMT_NO stmtNo, NAME procName)
      : StmtAST(stmtNo), ProcName(procName) {}
};

class WhileStmtAST : public StmtAST {
 public:
  CondExprAST* CondExpr;
  std::vector<StmtAST*> StmtList;
  WhileStmtAST(STMT_NO stmtNo, CondExprAST* condExpr,
               std::vector<StmtAST*> stmtList)
      : StmtAST(stmtNo), CondExpr(condExpr), StmtList(stmtList) {}
};

class IfStmtAST : public StmtAST {
 public:
  CondExprAST* CondExpr;
  std::vector<StmtAST*> ThenBlock;
  std::vector<StmtAST*> ElseBlock;
  IfStmtAST(STMT_NO stmtNo, CondExprAST* condExpr,
            std::vector<StmtAST*> thenBlock, std::vector<StmtAST*> elseBlock)
      : StmtAST(stmtNo),
        CondExpr(condExpr),
        ThenBlock(thenBlock),
        ElseBlock(elseBlock) {}
};

class AssignStmtAST : public StmtAST {
 public:
  NAME VarName;
  ArithAST* Expr;
  AssignStmtAST(STMT_NO stmtNo, NAME varName, ArithAST* expr)
      : StmtAST(stmtNo), VarName(varName), Expr(expr) {}
};

class ProcedureAST {
 public:
  NAME ProcName;
  std::vector<StmtAST*> StmtList;
  ProcedureAST(NAME procName, std::vector<StmtAST*> stmtList)
      : ProcName(procName), StmtList(stmtList) {}
};

class ProgramAST {
 public:
  std::vector<ProcedureAST*> ProcedureList;
  explicit ProgramAST(std::vector<ProcedureAST*> procedureList)
      : ProcedureList(procedureList) {}
};
