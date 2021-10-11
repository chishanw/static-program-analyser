#include "ResultProjector.h"

#include <Common/Global.h>

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

using namespace query;

ResultProjector::ResultProjector(PKB* pkb) { this->pkb = pkb; }

list<string> ResultProjector::formatResults(
    unordered_map<string, DesignEntity> synonymMap, SelectType selectType,
    vector<Synonym> selectSynonyms, vector<vector<int>> results) {
  list<string> formattedResults = {};

  if (selectType == SelectType::BOOLEAN) {
    if (results.size() != 1 || results[0].size() != 1) {
      DMOprintErrMsgAndExit(
          "[ResultProjector] invalid bool clause results size");
    }

    int boolClauseResult = results[0][0];
    if (boolClauseResult == FALSE_SELECT_BOOL_RESULT) {
      formattedResults.push_back({"FALSE"});
    } else if (boolClauseResult == TRUE_SELECT_BOOL_RESULT) {
      formattedResults.push_back({"TRUE"});
    } else {
      DMOprintErrMsgAndExit(
          "[ResultProjector] invalid bool clause results value");
    }
  }

  if (selectType == SelectType::SYNONYMS) {
    for (auto res : results) {
      string formattedString = "";
      for (int i = 0; i < selectSynonyms.size(); i++) {
        Synonym synonym = selectSynonyms[i];
        DesignEntity designEntity = synonymMap.at(synonym.name);
        formattedString += getStringByDesignEntity(designEntity, res[i]);

        if (i != selectSynonyms.size() - 1) {
          formattedString += " ";
        }
      }
      formattedResults.push_back(formattedString);
    }
  }

  return formattedResults;
}

string ResultProjector::getStringByDesignEntity(DesignEntity designEntity,
                                                int result) {
  switch (designEntity) {
    case DesignEntity::VARIABLE:
      return pkb->getVarName(result);
    case DesignEntity::PROCEDURE:
      return pkb->getProcName(result);
    case DesignEntity::CONSTANT:
      return pkb->getConst(result);
    default:
      // all statement types and prog_line
      return to_string(result);
  }
}
