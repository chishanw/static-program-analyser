#include "TestWrapper.h"

#include <Common/Global.h>
#include <DesignExtractor/DesignExtractor.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>
#include <Query/Evaluator/QueryEvaluator.h>
#include <Query/Parser/QueryParser.h>
#include <Query/Projector/ResultProjector.h>

#include <unordered_set>
#include <vector>

using namespace std;
using namespace query;

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
  pkb = new PKB();
  this->OurOwnGlobalStop = false;
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
  try {
    // parse program into tokens
    vector<string> tokens = Tokenizer::TokenizeFile(filename);

    // then tokends will be passed to parser
    const bool enableIter1restriction = true;
    const ProgramAST* programAST = Parser(enableIter1restriction).Parse(tokens);

    // then programAST will be passed to DE
    DesignExtractor de = DesignExtractor(pkb);
    de.Extract(programAST);

  } catch (const exception& ex) {
    cout << "Exception caught: " << ex.what() << endl;
    OurOwnGlobalStop = true;
    return;
  }
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  if (OurOwnGlobalStop || GlobalStop) {
    return;  // only true when parse() encounter exceptions or TLE
  }

  try {
    tuple<SynonymMap, SelectClause> parsedQuery = QueryParser().Parse(query);

    std::unordered_set<int> evaluatedResult = QueryEvaluator(pkb).evaluateQuery(
        get<0>(parsedQuery), get<1>(parsedQuery));

    SynonymMap map = get<0>(parsedQuery);
    SelectClause clause = get<1>(parsedQuery);
    DesignEntity selectSynDesignEntity = map.at(clause.selectSynonym.name);
    results = ResultProjector(pkb).formatResults(selectSynDesignEntity,
                                                 evaluatedResult);
  } catch (const exception& ex) {
    cout << "Exception caught: " << ex.what() << endl;
    return;
  }
}
