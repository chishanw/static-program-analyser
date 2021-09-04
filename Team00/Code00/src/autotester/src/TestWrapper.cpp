#include "TestWrapper.h"

#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>

#include <vector>

using namespace std;

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
  // // parse program into tokens
  std::string program =
      "procedure Example {\n"
      "  x = 1-2+3; }\n"
      "\n";
  std::vector<std::string> tokens = Tokenizer::TokenizeProgramString(program);

  // // // parse program into tokens
  // std::vector<std::string> tokens = Tokenizer::TokenizeFile(filename);

  std::cout << "Parsed tokens:" << std::endl;
  for (const std::string token : tokens) {
    std::cout << token << " ";
  }
  std::cout << std::endl;

  // then tokends will be passed to parser
  ProgramAST* ast = Parser().Parse(tokens);
  StmtAST* stmt = ast->ProcedureList[0]->StmtList[0];
  cout << "Stmt No: " << stmt->StmtNo << endl;
  if (AssignStmtAST* assign = dynamic_cast<AssignStmtAST*>(stmt)) {
    cout << assign->Expr->GetPatternStr() << endl;

    // cout << assign->VarName << endl;
    // ExprAST* expr123 = assign->Expr;
    // ExprAST* expr12 = expr123->LeftNode;
    // ExprAST* expr3 = expr123->RightNode;
    // ExprAST* expr1 = expr12->LeftNode;
    // ExprAST* expr2 = expr12->RightNode;

    // cout << "123" << endl;
    // cout << *expr123 << endl;
    // cout << "12" << endl;
    // cout << *expr12 << endl;
    // cout << "3" << endl;
    // cout << *expr3 << endl;
    // cout << "1" << endl;
    // cout << *expr1 << endl;
    // cout << "2" << endl;
    // cout << *expr2 << endl;

    // if (FactorAST* f2 = dynamic_cast<FactorAST*>(expr2)) {
    //   cout << f2->GetPatternStr() << endl;
    //   cout << f2->ConstValue << endl;
    // }
  }

  cout << endl << "done" << endl;
  exit(0);

  // then ast will be passed to DE
  // ...
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  // call your evaluator to evaluate the query here
  // ...code to evaluate query...

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
}
