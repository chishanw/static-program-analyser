#include <iostream>
#include <string>
#include <vector>

#include "Common/global.h"

typedef int STMT_NO;
typedef std::string NAME;

// class ExprAST;
// class FactorAST : public ExprAST;
// class RelExprAST;
// class CondExprAST;
// class StmtAST;
// class ReadStmtAST : public StmtAST;
// class PrintStmtAST : public StmtAST;
// class CallStmtAST : public StmtAST;
// class WhileStmtAST : public StmtAST;
// class IfStmtAST : public StmtAST;
// class AssignStmtAST : public StmtAST;
// class ProcedureAST;
// class ProgramAST;

// class ExprAST;
// class FactorAST : public ExprAST;
// class FactorAST;

class ExprAST {
 public:
  std::string Sign;
  ExprAST* LeftNode;
  ExprAST* RightNode;

  ExprAST() = default;  // default constructor for child class

  explicit ExprAST(ExprAST* leftNode)
      : hasOnlyOneNode(true),
        Sign(""),
        LeftNode(leftNode),
        RightNode(nullptr) {}

  ExprAST(std::string sign, ExprAST* leftNode, ExprAST* rightNode)
      : hasOnlyOneNode(false),
        Sign(sign),
        LeftNode(leftNode),
        RightNode(rightNode) {}

  virtual ~ExprAST() {}

  bool HasOnlyOneNode() { return this->hasOnlyOneNode; }

  friend std::ostream& operator<<(std::ostream& out, ExprAST const& obj);

 private:
  bool hasOnlyOneNode;
};

class FactorAST : public ExprAST {
 public:
  NAME VarName;
  int ConstValue;
  ExprAST* Expr;

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

  explicit FactorAST(ExprAST* expr)
      : isVarName(false),
        isConstValue(false),
        isExpr(true),
        VarName(""),
        ConstValue(0),
        Expr(expr) {}

  bool IsVarName() { return this->isVarName; }
  bool IsConstValue() { return this->isConstValue; }
  bool IsExpr() { return this->isExpr; }

  friend std::ostream& operator<<(std::ostream& out, FactorAST const& obj);

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
  ExprAST* Expr;
  AssignStmtAST(STMT_NO stmtNo, NAME varName, ExprAST* expr)
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
