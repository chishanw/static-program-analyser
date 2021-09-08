#include "TestWrapper.h"

#include <Common/Global.h>
#include <DesignExtractor/DesignExtractor.h>
#include <Parser/Parser.h>
#include <Parser/Tokenizer.h>
#include <Query/Parser/QueryParser.h>

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

  try {
    tuple<SynonymMap, SelectClause> parsedQuery = QueryParser().Parse(query);

    // TODO(Beatrice): Remove before submission
    PrintDebugMessage("PARSED SYNONYM MAP:");
    SynonymMap map = get<0>(parsedQuery);
    for (auto& it: map) {
      PrintDebugMessage(
          "[" + it.first + "," + to_string(static_cast<std::underlying_type<DesignEntity>::type>(it.second)) + "]");
    }

    SelectClause clause = get<1>(parsedQuery);
    PrintDebugMessage("PARSED SELECT SYNONYM:");
    PrintDebugMessage(clause.selectSynonym.name + "," +
                      to_string(static_cast<std::underlying_type<DesignEntity>::type>(clause.selectSynonym.entity)));

    SuchThatClause suchThatClause = clause.suchThatClauses.front();
    PrintDebugMessage("FIRST PARSED SUCH_THAT CLAUSE: ");
    string clauseString =
        "[" + to_string(static_cast<std::underlying_type<RelationshipType>::type>(suchThatClause.relationshipType)) +
        ","
        + "(" + to_string(static_cast<std::underlying_type<ParamType>::type>(suchThatClause.leftParam.type)) + "," +
        suchThatClause.leftParam.value + "),"
        + "(" + to_string(static_cast<std::underlying_type<ParamType>::type>(suchThatClause.rightParam.type)) + "," +
        suchThatClause.rightParam.value + ")"
        + "]";
    PrintDebugMessage(clauseString);

  } catch (const exception& ex) {
    cout << "Exception caught: " << ex.what() << "\n";
  }

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
}
