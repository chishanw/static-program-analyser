#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "Common/Common.h"
#include "Common/Global.h"

class ArithAST {
 public:
  const std::string Sign;
  const ArithAST* LeftNode;
  const ArithAST* RightNode;

  ArithAST() : hasOnlyOneNode(false) {}  // default constructor for child class

  explicit ArithAST(ArithAST* leftNode)
      : Sign(""),
        LeftNode(leftNode),
        RightNode(nullptr),
        hasOnlyOneNode(true) {}

  ArithAST(std::string sign, ArithAST* leftNode, ArithAST* rightNode)
      : Sign(sign),
        LeftNode(leftNode),
        RightNode(rightNode),
        hasOnlyOneNode(false) {}

  virtual ~ArithAST() {}

  bool HasOnlyOneNode() const { return this->hasOnlyOneNode; }
  virtual std::unordered_set<Name> GetAllVarNames() const;
  virtual std::unordered_set<std::string> GetAllConsts() const;
  virtual std::unordered_set<std::string> GetSubExprPatternStrs() const;
  virtual std::string GetFullExprPatternStr() const;
  virtual std::string GetDebugStr() const;

 private:
  const bool hasOnlyOneNode;
};

class FactorAST : public ArithAST {
 public:
  const Name VarName;
  const std::string ConstValue;
  const ArithAST* Expr;

  explicit FactorAST(Name varName)
      : VarName(varName),
        ConstValue(""),
        Expr(nullptr),
        isVarName(true),
        isConstValue(false),
        isExpr(false) {}

  explicit FactorAST(std::string constValue, bool isConstant)
      : VarName(""),
        ConstValue(constValue),
        Expr(nullptr),
        isVarName(false),
        isConstValue(isConstant),
        isExpr(false) {}

  explicit FactorAST(ArithAST* expr)
      : VarName(""),
        ConstValue(""),
        Expr(expr),
        isVarName(false),
        isConstValue(false),
        isExpr(true) {}

  bool IsVarName() const { return this->isVarName; }
  bool IsConstValue() const { return this->isConstValue; }
  bool IsExpr() const { return this->isExpr; }
  std::unordered_set<std::string> GetAllVarNames() const override;
  std::unordered_set<std::string> GetAllConsts() const override;
  std::unordered_set<std::string> GetSubExprPatternStrs() const override;
  std::string GetFullExprPatternStr() const override;
  std::string GetDebugStr() const override;

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

  std::unordered_set<Name> GetAllVarNames() const;
  std::unordered_set<std::string> GetAllConsts() const;
};

class CondExprAST {
 public:
  const std::string Sign;
  const RelExprAST* RelExpr;
  const CondExprAST* LeftNode;
  const CondExprAST* RightNode;

  explicit CondExprAST(RelExprAST* relExpr)
      : Sign(""),
        RelExpr(relExpr),
        LeftNode(nullptr),
        RightNode(nullptr),
        hasOnlyOneRelExpr(true),
        hasOnlyOneCondExpr(false) {}

  CondExprAST(std::string sign, CondExprAST* condExpr)
      : Sign(sign),
        RelExpr(nullptr),
        LeftNode(condExpr),
        RightNode(nullptr),
        hasOnlyOneRelExpr(false),
        hasOnlyOneCondExpr(true) {}

  CondExprAST(std::string sign, CondExprAST* leftNode, CondExprAST* rightNode)
      : Sign(sign),
        LeftNode(leftNode),
        RightNode(rightNode),
        hasOnlyOneRelExpr(false),
        hasOnlyOneCondExpr(false) {}

  bool HasOnlyOneRelExpr() const { return this->hasOnlyOneRelExpr; }
  bool HasOnlyOneCondExpr() const { return this->hasOnlyOneCondExpr; }
  std::unordered_set<Name> GetAllVarNames() const;
  std::unordered_set<std::string> GetAllConsts() const;

 private:
  const bool hasOnlyOneRelExpr;
  const bool hasOnlyOneCondExpr;
};

class StmtAST {
 public:
  const StmtNo StmtNo;
  virtual ~StmtAST() {}

 protected:
  explicit StmtAST(::StmtNo stmtNo) : StmtNo(stmtNo) {}
};

class ReadStmtAST : public StmtAST {
 public:
  const Name VarName;
  ReadStmtAST(::StmtNo stmtNo, Name varName)
      : StmtAST(stmtNo), VarName(varName) {}
};

class PrintStmtAST : public StmtAST {
 public:
  const Name VarName;
  PrintStmtAST(::StmtNo stmtNo, Name varName)
      : StmtAST(stmtNo), VarName(varName) {}
};

class CallStmtAST : public StmtAST {
 public:
  const Name ProcName;
  CallStmtAST(::StmtNo stmtNo, Name procName)
      : StmtAST(stmtNo), ProcName(procName) {}
};

class WhileStmtAST : public StmtAST {
 public:
  const CondExprAST* CondExpr;
  const std::vector<StmtAST*> StmtList;
  WhileStmtAST(::StmtNo stmtNo, CondExprAST* condExpr,
               std::vector<StmtAST*> stmtList)
      : StmtAST(stmtNo), CondExpr(condExpr), StmtList(stmtList) {}
};

class IfStmtAST : public StmtAST {
 public:
  const CondExprAST* CondExpr;
  const std::vector<StmtAST*> ThenBlock;
  const std::vector<StmtAST*> ElseBlock;
  IfStmtAST(::StmtNo stmtNo, CondExprAST* condExpr,
            std::vector<StmtAST*> thenBlock, std::vector<StmtAST*> elseBlock)
      : StmtAST(stmtNo),
        CondExpr(condExpr),
        ThenBlock(thenBlock),
        ElseBlock(elseBlock) {}
};

class AssignStmtAST : public StmtAST {
 public:
  const Name VarName;
  const ArithAST* Expr;
  AssignStmtAST(::StmtNo stmtNo, Name varName, ArithAST* expr)
      : StmtAST(stmtNo), VarName(varName), Expr(expr) {}
};

class ProcedureAST {
 public:
  const Name ProcName;
  const std::vector<StmtAST*> StmtList;
  ProcedureAST(Name procName, std::vector<StmtAST*> stmtList)
      : ProcName(procName), StmtList(stmtList) {}
};

class ProgramAST {
 public:
  const std::vector<ProcedureAST*> ProcedureList;
  explicit ProgramAST(std::vector<ProcedureAST*> procedureList)
      : ProcedureList(procedureList) {}
};
