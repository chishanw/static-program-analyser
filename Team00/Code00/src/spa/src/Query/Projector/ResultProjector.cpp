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
        // pkb->getVarName(varIdx)
      }
      return formattedResults;
    case DesignEntity::CONSTANT:
      for (int constIdx : results) {
        // pkb->getConstValue(constIdx) or sth
      }
      return formattedResults;
    case DesignEntity::PROCEDURE:
      for (int procIdx : results) {
        // pkb->getProcName(procIdx)
      }
      return formattedResults;
    default:
      for (int stmtNum : results) {
        formattedResults.push_back(to_string(stmtNum));
      }
      return formattedResults;
  }
}
