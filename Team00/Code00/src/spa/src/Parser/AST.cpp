#include "AST.h"

using namespace std;

string ArithAST::GetPatternStr() {
  if (FactorAST* f = dynamic_cast<FactorAST*>(this)) {
    // cout << "casted to F*" << endl; // TODO(gf): clean up
    return f->GetPatternStr();
  }

  // no space between tokens
  stringstream out;
  if (hasOnlyOneNode) {
    out << this->LeftNode->GetPatternStr();
  } else {
    out << this->LeftNode->GetPatternStr() << this->Sign
        << this->RightNode->GetPatternStr();
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
        << "sign: " << this->Sign << ", "
        << "ln: " << this->LeftNode->GetDebugStr() << ", "
        << "rn: " << this->RightNode << "}";
  } else {
    out << "2NExprAST(" << this << "){"
        << "sign: " << this->Sign << ", "
        << "ln: " << this->LeftNode->GetDebugStr() << ", "
        << "rn: " << this->RightNode->GetDebugStr() << "}";
  }
  return out.str();
}

// ostream& operator<<(std::ostream& out, ArithAST const& obj) {
//   return out << obj.GetDebugStr();
// }

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
