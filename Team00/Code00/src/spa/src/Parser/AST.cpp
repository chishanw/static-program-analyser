#include "AST.h"

using namespace std;

ostream& operator<<(ostream& out, ExprAST const& obj) {
  // print expr in postfix form
  if (obj.hasOnlyOneNode) {
    FactorAST* castedLeftNode = dynamic_cast<FactorAST*>(obj.LeftNode);

    out << "["
        << "1NExprAST(" << &obj << ")"
        << ", " << *castedLeftNode << ", " << obj.RightNode << ", " << obj.Sign
        << "]";
  } else {
    out << "["
        << "2NExprAST(" << &obj << ")"
        << ", " << *(obj.LeftNode) << ", " << *(obj.RightNode) << ", "
        << obj.Sign << "]";
  }
  return out;
}

ostream& operator<<(ostream& out, FactorAST const& obj) {
  if (obj.isVarName) {
    out << "["
        << "FactorAST(" << &obj << "): "
        << "VarNmae(" << obj.VarName << ")"
        << "]";
  } else if (obj.isConstValue) {
    out << "["
        << "FactorAST(" << &obj << "): "
        << "ConstValue(" << obj.ConstValue << ")"
        << "]";
  } else if (obj.isExpr) {
    out << "["
        << "FactorAST(" << &obj << "): "
        << " ( " << obj.Expr << " ) "
        << "]";
  } else {
    Debug("FactorAST has wrong bool value");
  }
  return out;
}
