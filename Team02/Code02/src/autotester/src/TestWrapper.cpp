#include "TestWrapper.h"

#include <Common/Global.h>
#include <DesignExtractor/DesignExtractor.h>
#include <Parser/Parser.h>
#include <Common/Tokenizer.h>
#include <Query/Evaluator/QueryEvaluator.h>
#include <Query/Parser/QueryLexerParserCommon.h>
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
    const ProgramAST* programAST = Parser().Parse(tokens);
    DMOprintInfoMsg("SIMPLE Parser was successful");

    // then programAST will be passed to DE
    DesignExtractor de = DesignExtractor(pkb);
    de.Extract(programAST);
    DMOprintInfoMsg("Design Extractor was successful");

  } catch (const exception& ex) {
    DMOprintInfoMsg("TestWrapper parse() caught an exception");
    cout << "Exception caught: " << ex.what() << endl;
    OurOwnGlobalStop = true;
    return;
  }
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  if (OurOwnGlobalStop || GlobalStop) {
    DMOprintInfoMsg("TestWrapper evaluate()'s GlobalStop is TRUE");
    results = {};
    return;  // only true when parse() encounter exceptions or TLE
  }

  try {
    tuple<SynonymMap, SelectClause> parsedQuery = QueryParser().Parse(query);
    DMOprintInfoMsg("Query Parser was successful");

    std::vector<std::vector<int>> evaluatedResult =
        QueryEvaluator(pkb).evaluateQuery(get<0>(parsedQuery),
                                          get<1>(parsedQuery));
    DMOprintInfoMsg("Query Evaluator was successful");

    SynonymMap map = get<0>(parsedQuery);
    SelectClause clause = get<1>(parsedQuery);
    results = ResultProjector(pkb).formatResults(
        map, clause.selectType, clause.selectSynonyms, evaluatedResult);
    DMOprintInfoMsg("Query Result Projector was successful");

  } catch (const qpp::SyntacticErrorException& ex) {
    DMOprintInfoMsg("TestWrapper evaluate() caught a syntactic error");
    results = {};
    cout << "Exception caught: " << ex.what() << endl;
    return;

  } catch (const qpp::SemanticSynonymErrorException& ex) {
    DMOprintInfoMsg("TestWrapper evaluate() caught a semantic synonym error");
    results = {};
    cout << "Exception caught: " << ex.what() << endl;
    return;

  } catch (const qpp::SemanticBooleanErrorException& ex) {
    DMOprintInfoMsg("TestWrapper evaluate() caught a semantic boolean error");
    results = {"FALSE"};
    cout << "Exception caught: " << ex.what() << endl;
    return;

  } catch (const exception& ex) {
    DMOprintInfoMsg("TestWrapper evaluate() caught an unknown exception");
    results = {};
    cout << "Exception caught: " << ex.what() << endl;
    return;
  }
}
