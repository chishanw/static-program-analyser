#include "AST.h"

#include <algorithm>

using namespace std;

vector<string> ArithAST::GetAllPatternStr() {
  vector<string> res;
  if (FactorAST* f = dynamic_cast<FactorAST*>(this)) {
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

string ArithAST::GetPatternStr() {
  if (FactorAST* f = dynamic_cast<FactorAST*>(this)) {
    // cout << "casted to F*" << endl; // TODO(gf): clean up
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

string ArithAST::GetDebugStr() {
  if (FactorAST* f = dynamic_cast<FactorAST*>(this)) {
    // cout << "casted to F*" << endl; // TODO(gf): clean up
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

// ostream& operator<<(std::ostream& out, ArithAST const& obj) {
//   return out << obj.GetDebugStr();
// }

vector<string> FactorAST::GetAllPatternStr() {
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

string FactorAST::GetPatternStr() {
  // no space between tokens
  stringstream out;
  if (isVarName) {
    out << VarName;
  } else if (isConstValue) {
    out << ConstValue;
  } else if (isExpr) {
    out << "(" << Expr->GetPatternStr() << ")";
  } else {
    Debug("FactorAST has wrong bool value");
  }
  return out.str();
}

string FactorAST::GetDebugStr() {
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
    Debug("FactorAST has wrong bool value");
  }
  return out.str();
}

// ostream& operator<<(std::ostream& out, FactorAST const& obj) {
//   return out << obj.GetDebugStr();
// }
