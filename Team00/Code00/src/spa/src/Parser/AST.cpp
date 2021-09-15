#include "AST.h"

#include <algorithm>
#include <unordered_set>

using namespace std;

vector<NAME> ArithAST::GetAllVarNames() const {
  vector<NAME> res;
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    if (f->IsVarName()) {
      res.push_back(f->VarName);
    }
    return res;
  }

  if (LeftNode != nullptr) {
    vector<NAME> resL = LeftNode->GetAllVarNames();
    copy(resL.begin(), resL.end(), back_inserter(res));
  }
  if (RightNode != nullptr) {
    vector<NAME> resR = RightNode->GetAllVarNames();
    copy(resR.begin(), resR.end(), back_inserter(res));
  }
  return res;
}

unordered_set<int> ArithAST::GetAllConsts() const {
  unordered_set<int> res;
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    if (f->IsConstValue()) {
      res.insert(f->ConstValue);
    }
    return res;
  }

  if (LeftNode != nullptr) {
    res.merge(LeftNode->GetAllConsts());
  }
  if (RightNode != nullptr) {
    res.merge(RightNode->GetAllConsts());
  }
  return res;
}

vector<string> ArithAST::GetAllPatternStr() const {
  vector<string> res;
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    return f->GetAllPatternStr();
  }

  res.push_back(GetPatternStr());
  if (LeftNode != nullptr) {
    vector<string> resL = LeftNode->GetAllPatternStr();
    copy(resL.begin(), resL.end(), back_inserter(res));
  }
  if (RightNode != nullptr) {
    vector<string> resR = RightNode->GetAllPatternStr();
    copy(resR.begin(), resR.end(), back_inserter(res));
  }
  return res;
}

string ArithAST::GetPatternStr() const {
  if (const FactorAST* f = dynamic_cast<const FactorAST*>(this)) {
    return f->GetPatternStr();
  }

  // no space between tokens
  stringstream out;
  if (hasOnlyOneNode) {
    out << LeftNode->GetPatternStr();
  } else {
    out << LeftNode->GetPatternStr() << Sign << RightNode->GetPatternStr();
  }
  return out.str();
}

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

vector<string> FactorAST::GetAllPatternStr() const {
  vector<string> res;
  if (!isExpr) {
    res.push_back(GetPatternStr());
    return res;
  }

  // NOTE: itself must be in the 1st index in the res vector,
  // cos DE expects the 1st string be the whole expr
  res = Expr->GetAllPatternStr();
  res.insert(res.begin(), GetPatternStr());
  return res;
}

string FactorAST::GetPatternStr() const {
  // no space between tokens
  stringstream out;
  if (isVarName) {
    out << VarName;
  } else if (isConstValue) {
    out << ConstValue;
  } else if (isExpr) {
    out << "(" << Expr->GetPatternStr() << ")";
  } else {
    DMOprintErrMsgAndExit("FactorAST has wrong bool value");
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

vector<NAME> CondExprAST::GetAllVarNames() const {
  if (hasOnlyOneRelExpr) {
    return RelExpr->GetAllVarNames();
  }
  if (hasOnlyOneCondExpr) {
    return LeftNode->GetAllVarNames();
  }
  // has two cond expr
  vector<NAME> res = LeftNode->GetAllVarNames();
  vector<NAME> resR = RightNode->GetAllVarNames();
  copy(resR.begin(), resR.end(), back_inserter(res));
  return res;
}

unordered_set<int> CondExprAST::GetAllConsts() const {
  if (hasOnlyOneRelExpr) {
    return RelExpr->GetAllConsts();
  }
  if (hasOnlyOneCondExpr) {
    return LeftNode->GetAllConsts();
  }
  // has two cond expr
  unordered_set<int> res = LeftNode->GetAllConsts();
  unordered_set<int> resR = RightNode->GetAllConsts();
  res.merge(resR);
  return res;
}

vector<NAME> RelExprAST::GetAllVarNames() const {
  vector<NAME> res;
  if (LeftNode->IsVarName()) {
    res.push_back(LeftNode->VarName);
  }
  if (RightNode->IsVarName()) {
    res.push_back(RightNode->VarName);
  }
  return res;
}

unordered_set<int> RelExprAST::GetAllConsts() const {
  unordered_set<int> res;
  if (LeftNode->IsVarName()) {
    res.insert(LeftNode->ConstValue);
  }
  if (RightNode->IsVarName()) {
    res.insert(RightNode->ConstValue);
  }
  return res;
}
