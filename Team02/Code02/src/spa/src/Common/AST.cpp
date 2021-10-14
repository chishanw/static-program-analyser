#include "AST.h"

#include <algorithm>
#include <unordered_set>

using namespace std;

// ==================
// GetFullExprPatternStr
// ==================
string ArithAST::GetFullExprPatternStr() const {
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    return f->GetFullExprPatternStr();
  }

  // no space between tokens
  stringstream out;
  out << "[";
  if (hasOnlyOneNode) {
    out << LeftNode->GetFullExprPatternStr();
  } else {
    out << LeftNode->GetFullExprPatternStr() << Sign
        << RightNode->GetFullExprPatternStr();
  }
  out << "]";
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
vector<string> ArithAST::GetSubExprPatternStrs() const {
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    return f->GetSubExprPatternStrs();
  }

  vector<string> res;
  res.push_back(GetFullExprPatternStr());
  if (LeftNode != nullptr) {
    vector<string> resL = LeftNode->GetSubExprPatternStrs();
    copy(resL.begin(), resL.end(), back_inserter(res));
  }
  if (RightNode != nullptr) {
    vector<string> resR = RightNode->GetSubExprPatternStrs();
    copy(resR.begin(), resR.end(), back_inserter(res));
  }
  return res;
}

vector<string> FactorAST::GetSubExprPatternStrs() const {
  vector<string> res;
  if (!isExpr) {
    res.push_back(GetFullExprPatternStr());
    return res;
  }

  // NOTE: itself must be in the 1st index in the res vector,
  // cos DE expects the 1st string be the whole expr
  res = Expr->GetSubExprPatternStrs();
  res.insert(res.begin(), GetFullExprPatternStr());
  return res;
}

// ==================
// GetDebugStr
// ==================
string ArithAST::GetDebugStr() const {
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    return f->GetDebugStr();
  }

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
unordered_set<NAME> ArithAST::GetAllVarNames() const {
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    return f->GetAllVarNames();
  }

  unordered_set<NAME> res;
  if (LeftNode != nullptr) {
    res.merge(LeftNode->GetAllVarNames());
  }
  if (RightNode != nullptr) {
    res.merge(RightNode->GetAllVarNames());
  }
  return res;
}

unordered_set<NAME> CondExprAST::GetAllVarNames() const {
  if (hasOnlyOneRelExpr) {
    return RelExpr->GetAllVarNames();
  }
  if (hasOnlyOneCondExpr) {
    return LeftNode->GetAllVarNames();
  }

  // has two cond expr
  unordered_set<NAME> res = LeftNode->GetAllVarNames();
  res.merge(RightNode->GetAllVarNames());
  return res;
}

unordered_set<NAME> RelExprAST::GetAllVarNames() const {
  unordered_set<NAME> res;
  res.merge(LeftNode->GetAllVarNames());
  res.merge(RightNode->GetAllVarNames());
  return res;
}

unordered_set<NAME> FactorAST::GetAllVarNames() const {
  if (isVarName) {
    return {this->VarName};
  } else if (isConstValue) {
    return {};
  } else if (isExpr) {
    const ArithAST* expr = this->Expr;
    const ArithAST* temp = dynamic_cast<const ArithAST*>(this->Expr);
    return temp->GetAllVarNames();
  } else {
    DMOprintErrMsgAndExit("[AST][GetAllVarNames] shouldn't reach here");
    return {};
  }
}

// ==================
// GetAllConsts
// ==================
unordered_set<string> ArithAST::GetAllConsts() const {
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    return f->GetAllConsts();
  }

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
