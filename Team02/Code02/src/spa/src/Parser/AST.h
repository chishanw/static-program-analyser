#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "Common/Global.h"

typedef int STMT_NO;
typedef std::string NAME;

class ArithAST {
 public:
  const std::string Sign;
  const ArithAST* LeftNode;
  const ArithAST* RightNode;

  ArithAST() : hasOnlyOneNode(false) {}  // default constructor for child class

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
  std::unordered_set<NAME> GetAllVarNames() const;
  std::unordered_set<std::string> GetAllConsts() const;
  std::vector<std::string> GetSubExprPatternStrs() const;
  std::string GetFullExprPatternStr() const;
  std::string GetDebugStr() const;

 private:
  const bool hasOnlyOneNode;
};

class FactorAST : public ArithAST {
 public:
  const NAME VarName;
  const std::string ConstValue;
  const ArithAST* Expr;

  explicit FactorAST(NAME varName)
      : isVarName(true),
        isConstValue(false),
        isExpr(false),
        VarName(varName),
        ConstValue(""),
        Expr(nullptr) {}

  explicit FactorAST(std::string constValue, bool isConstant)
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
        ConstValue(""),
        Expr(expr) {}

  bool IsVarName() const { return this->isVarName; }
  bool IsConstValue() const { return this->isConstValue; }
  bool IsExpr() const { return this->isExpr; }
  std::unordered_set<std::string> GetAllVarNames() const;
  std::unordered_set<std::string> GetAllConsts() const;
  std::vector<std::string> GetSubExprPatternStrs() const;
  std::string GetFullExprPatternStr() const;
  std::string GetDebugStr() const;

 private:
  const bool isVarName;
  const bool isConstValue;
  const bool isExpr;
};

class RelExprAST {
 public:
  const std::string Sign;
  const FactorAST* LeftNode;   // Factor and RelFactor are almost the same thing
  const FactorAST* RightNode;  // except for the bracket expr

  RelExprAST(std::string sign, FactorAST* leftNode, FactorAST* rightNode)
      : Sign(sign), LeftNode{leftNode}, RightNode{rightNode} {}

  std::unordered_set<NAME> GetAllVarNames() const;
  std::unordered_set<std::string> GetAllConsts() const;
};

class CondExprAST {
 public:
  const std::string Sign;
  const RelExprAST* RelExpr;
  const CondExprAST* LeftNode;
  const CondExprAST* RightNode;

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

  bool HasOnlyOneRelExpr() const { return this->hasOnlyOneRelExpr; }
  bool HasOnlyOneCondExpr() const { return this->hasOnlyOneCondExpr; }
  std::unordered_set<NAME> GetAllVarNames() const;
  std::unordered_set<std::string> GetAllConsts() const;

 private:
  const bool hasOnlyOneRelExpr;
  const bool hasOnlyOneCondExpr;
};

class StmtAST {
 public:
  const STMT_NO StmtNo;
  virtual ~StmtAST() {}

 protected:
  explicit StmtAST(STMT_NO stmtNo) : StmtNo(stmtNo) {}
};

class ReadStmtAST : public StmtAST {
 public:
  const NAME VarName;
  ReadStmtAST(STMT_NO stmtNo, NAME varName)
      : StmtAST(stmtNo), VarName(varName) {}
};

class PrintStmtAST : public StmtAST {
 public:
  const NAME VarName;
  PrintStmtAST(STMT_NO stmtNo, NAME varName)
      : StmtAST(stmtNo), VarName(varName) {}
};

class CallStmtAST : public StmtAST {
 public:
  const NAME ProcName;
  CallStmtAST(STMT_NO stmtNo, NAME procName)
      : StmtAST(stmtNo), ProcName(procName) {}
};

class WhileStmtAST : public StmtAST {
 public:
  const CondExprAST* CondExpr;
  const std::vector<StmtAST*> StmtList;
  WhileStmtAST(STMT_NO stmtNo, CondExprAST* condExpr,
               std::vector<StmtAST*> stmtList)
      : StmtAST(stmtNo), CondExpr(condExpr), StmtList(stmtList) {}
};

class IfStmtAST : public StmtAST {
 public:
  const CondExprAST* CondExpr;
  const std::vector<StmtAST*> ThenBlock;
  const std::vector<StmtAST*> ElseBlock;
  IfStmtAST(STMT_NO stmtNo, CondExprAST* condExpr,
            std::vector<StmtAST*> thenBlock, std::vector<StmtAST*> elseBlock)
      : StmtAST(stmtNo),
        CondExpr(condExpr),
        ThenBlock(thenBlock),
        ElseBlock(elseBlock) {}
};

class AssignStmtAST : public StmtAST {
 public:
  const NAME VarName;
  const ArithAST* Expr;
  AssignStmtAST(STMT_NO stmtNo, NAME varName, ArithAST* expr)
      : StmtAST(stmtNo), VarName(varName), Expr(expr) {}
};

class ProcedureAST {
 public:
  const NAME ProcName;
  const std::vector<StmtAST*> StmtList;
  ProcedureAST(NAME procName, std::vector<StmtAST*> stmtList)
      : ProcName(procName), StmtList(stmtList) {}
};

class ProgramAST {
 public:
  const std::vector<ProcedureAST*> ProcedureList;
  explicit ProgramAST(std::vector<ProcedureAST*> procedureList)
      : ProcedureList(procedureList) {}
};
