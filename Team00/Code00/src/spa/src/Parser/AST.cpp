#include "AST.h"

using namespace std;

// print expr in postfix form
string ExprAST::GetPatternStr() {
  if (FactorAST* f = dynamic_cast<FactorAST*>(this)) {
    cout << "casted to F*" << endl;
    return f->GetPatternStr();
  }

  stringstream out;
  if (hasOnlyOneNode) {
    out << "["
        << "1NExprAST(" << this << ")"
        << ", " << this->LeftNode << ", " << this->RightNode << ", "
        << this->Sign << "]";
  } else {
    out << "["
        << "2NExprAST(" << this << ")"
        << ", " << this->LeftNode->GetPatternStr() << ", "
        << this->RightNode->GetPatternStr() << ", " << this->Sign << "]";
  }
  return out.str();
}

// ostream& operator<<(std::ostream& out, ExprAST const& obj) {
//   return out << obj.GetPatternStr();
// }

string FactorAST::GetPatternStr() {
  stringstream out;
  if (isVarName) {
    out << "["
        << "FactorAST(" << this << "): "
        << "VarNmae(" << VarName << ")"
        << "]";
  } else if (isConstValue) {
    out << "["
        << "FactorAST(" << this << "): "
        << "ConstValue(" << ConstValue << ")"
        << "]";
  } else if (isExpr) {
    out << "["
        << "FactorAST(" << this << "): "
        << " ( " << Expr << " ) "
        << "]";
  } else {
    Debug("FactorAST has wrong bool value");
  }
  return out.str();
}

// ostream& operator<<(std::ostream& out, FactorAST const& obj) {
//   return out << obj.GetPatternStr();
// }
