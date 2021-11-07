#include "AST.h"

#include <algorithm>
#include <unordered_set>

using namespace std;

// ==================
// GetFullExprPatternStr
// ==================
string ArithAST::GetFullExprPatternStr() const {
  // no space between tokens
  stringstream out;
  if (hasOnlyOneNode) {
    out << LeftNode->GetFullExprPatternStr();
  } else {
    out << LeftNode->GetFullExprPatternStr()
        << RightNode->GetFullExprPatternStr() << Sign;
  }
  return out.str();
}

string FactorAST::GetFullExprPatternStr() const {
  if (isExpr) {
    return Expr->GetFullExprPatternStr();
  }

  // no space between tokens
  stringstream out;
  out << "[";
  if (isVarName) {
    out << VarName;
  } else if (isConstValue) {
    out << ConstValue;
  } else {
    DMOprintErrMsgAndExit("FactorAST has wrong bool value");
  }
  out << "]";
  return out.str();
}

// ==================
// GetSubExprPatternStrs
// ==================
unordered_set<string> ArithAST::GetSubExprPatternStrs() const {
  unordered_set<string> res = {GetFullExprPatternStr()};

  if (LeftNode != nullptr) {
    res.merge(LeftNode->GetSubExprPatternStrs());
  }
  if (RightNode != nullptr) {
    res.merge(RightNode->GetSubExprPatternStrs());
  }

  return res;
}

unordered_set<string> FactorAST::GetSubExprPatternStrs() const {
  if (!isExpr) {
    return {GetFullExprPatternStr()};
  } else {
    return Expr->GetSubExprPatternStrs();
  }
}

// ==================
// GetDebugStr
// ==================
string ArithAST::GetDebugStr() const {
  // format => 1NExprAST(0x1234){sign: ..., ln: ..., rn: ...};
  stringstream out;
  if (hasOnlyOneNode) {
    out << "1NExprAST(" << this << "){"
        << "sign: " << Sign << ", "
        << "ln: " << LeftNode->GetDebugStr() << ", "
        << "rn: " << RightNode << "}";
  } else {
    out << "2NExprAST(" << this << "){"
        << "sign: " << Sign << ", "
        << "ln: " << LeftNode->GetDebugStr() << ", "
        << "rn: " << RightNode->GetDebugStr() << "}";
  }
  return out.str();
}

string FactorAST::GetDebugStr() const {
  // format => FactorAST{addr: 0x11, sign: ..., ln: ..., rn: ...};
  stringstream out;
  if (isVarName) {
    out << "FactorAST(" << this << "){"
        << "VarNmae: " << VarName << "}";
  } else if (isConstValue) {
    out << "FactorAST(" << this << "){"
        << "ConstValue: " << ConstValue << "}";
  } else if (isExpr) {
    out << "FactorAST(" << this << "){"
        << "Expr: " << Expr->GetDebugStr() << "}";
  } else {
    DMOprintErrMsgAndExit("FactorAST has wrong bool value");
  }
  return out.str();
}

// ==================
// GetAllVarNames
// ==================
unordered_set<Name> ArithAST::GetAllVarNames() const {
  unordered_set<Name> res;
  if (LeftNode != nullptr) {
    res.merge(LeftNode->GetAllVarNames());
  }
  if (RightNode != nullptr) {
    res.merge(RightNode->GetAllVarNames());
  }
  return res;
}

unordered_set<Name> CondExprAST::GetAllVarNames() const {
  if (hasOnlyOneRelExpr) {
    return RelExpr->GetAllVarNames();
  }
  if (hasOnlyOneCondExpr) {
    return LeftNode->GetAllVarNames();
  }

  // has two cond expr
  unordered_set<Name> res = LeftNode->GetAllVarNames();
  res.merge(RightNode->GetAllVarNames());
  return res;
}

unordered_set<Name> RelExprAST::GetAllVarNames() const {
  unordered_set<Name> res;
  res.merge(LeftNode->GetAllVarNames());
  res.merge(RightNode->GetAllVarNames());
  return res;
}

unordered_set<Name> FactorAST::GetAllVarNames() const {
  if (isVarName) {
    return {this->VarName};
  } else if (isConstValue) {
    return {};
  } else if (isExpr) {
    return this->Expr->GetAllVarNames();
  } else {
    DMOprintErrMsgAndExit("[AST][GetAllVarNames] shouldn't reach here");
    return {};
  }
}

// ==================
// GetAllConsts
// ==================
unordered_set<string> ArithAST::GetAllConsts() const {
  unordered_set<string> res;
  if (LeftNode != nullptr) {
    res.merge(LeftNode->GetAllConsts());
  }
  if (RightNode != nullptr) {
    res.merge(RightNode->GetAllConsts());
  }
  return res;
}

unordered_set<string> CondExprAST::GetAllConsts() const {
  if (hasOnlyOneRelExpr) {
    return RelExpr->GetAllConsts();
  }
  if (hasOnlyOneCondExpr) {
    return LeftNode->GetAllConsts();
  }
  // has two cond expr
  unordered_set<string> res = LeftNode->GetAllConsts();
  unordered_set<string> resR = RightNode->GetAllConsts();
  res.merge(resR);
  return res;
}

unordered_set<string> RelExprAST::GetAllConsts() const {
  unordered_set<string> res;
  res.merge(LeftNode->GetAllConsts());
  res.merge(RightNode->GetAllConsts());
  return res;
}

unordered_set<string> FactorAST::GetAllConsts() const {
  if (isVarName) {
    return {};
  } else if (isConstValue) {
    return {this->ConstValue};
  } else if (isExpr) {
    return this->Expr->GetAllConsts();
  } else {
    DMOprintErrMsgAndExit("[AST][GetAllConsts] shouldn't reach here");
    return {};
  }
}
