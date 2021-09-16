#include "ResultProjector.h"

#include <list>
#include <string>
#include <unordered_set>

using namespace query;

ResultProjector::ResultProjector(PKB* pkb) { this->pkb = pkb; }

list<string> ResultProjector::formatResults(DesignEntity designEntity,
                                            unordered_set<int> results) {
  list<string> formattedResults = {};
  switch (designEntity) {
    case DesignEntity::VARIABLE:
      for (int varIdx : results) {
        formattedResults.push_back(pkb->getVarName(varIdx));
      }
      return formattedResults;
    case DesignEntity::PROCEDURE:
      for (int procIdx : results) {
        formattedResults.push_back(pkb->getProcName(procIdx));
      }
      return formattedResults;
    default:
      // all statement types and constants
      for (int num : results) {
        formattedResults.push_back(to_string(num));
      }
      return formattedResults;
  }
}
