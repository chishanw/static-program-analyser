#include "TestWrapper.h"

#include <DesignExtractor/DesignExtractor.h>
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
TestWrapper::TestWrapper() { pkb = new PKB(); }

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
  // parse program into tokens
  vector<string> tokens = Tokenizer::TokenizeFile(filename);
  // TDOO(gf): rm later
  cout << "Parsed tokens:" << endl;
  for (const string token : tokens) {
    cout << token << " ";
  }
  cout << endl;

  // then tokends will be passed to parser
  const ProgramAST* programAST = Parser().Parse(tokens);

  // then programAST will be passed to DE
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(programAST);
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  // call your evaluator to evaluate the query here
  // ...code to evaluate query...

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
}
