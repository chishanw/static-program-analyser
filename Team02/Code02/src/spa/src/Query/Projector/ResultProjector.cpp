#include "ResultProjector.h"

#include <Common/Global.h>

#include <list>
#include <string>
#include <vector>

using namespace std;
using namespace query;

ResultProjector::ResultProjector(PKB* pkb) { this->pkb = pkb; }

list<string> ResultProjector::formatResults(SelectType selectType,
                                            vector<Synonym> selectSynonyms,
                                            vector<vector<int>> results) {
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
        DesignEntity designEntity = synonym.entity;
        formattedString += getStringForSynonym(synonym, res[i]);

        if (i != selectSynonyms.size() - 1) {
          formattedString += " ";
        }
      }
      formattedResults.push_back(formattedString);
    }
  }

  return formattedResults;
}

string ResultProjector::getStringForSynonym(Synonym synonym, int result) {
  bool hasAttribute = synonym.hasAttribute;
  Attribute attribute = synonym.attribute;
  DesignEntity designEntity = synonym.entity;

  switch (designEntity) {
    case DesignEntity::VARIABLE:
      return pkb->getElementAt(TableType::VAR_TABLE, result);
    case DesignEntity::PROCEDURE:
      return pkb->getElementAt(TableType::PROC_TABLE, result);
    case DesignEntity::CONSTANT:
      return pkb->getElementAt(TableType::CONST_TABLE, result);
    case DesignEntity::CALL: {
      if (hasAttribute && attribute == Attribute::PROC_NAME) {
        int procIdx =
            *(pkb->getRight(RelationshipType::CALLS_S, result).begin());
        return pkb->getElementAt(TableType::PROC_TABLE, procIdx);
      } else {
        return to_string(result);
      }
    }
    case DesignEntity::READ: {
      if (hasAttribute && attribute == Attribute::VAR_NAME) {
        int varIdx =
            *pkb->getRight(RelationshipType::MODIFIES_S, result).begin();
        return pkb->getElementAt(TableType::VAR_TABLE, varIdx);
      } else {
        return to_string(result);
      }
    }
    case DesignEntity::PRINT: {
      if (hasAttribute && attribute == Attribute::VAR_NAME) {
        int varIdx = *pkb->getRight(RelationshipType::USES_S, result).begin();
        return pkb->getElementAt(TableType::VAR_TABLE, varIdx);
      } else {
        return to_string(result);
      }
    }
    default:
      // all other statement types (and .stmt#) and prog_line
      return to_string(result);
  }
}
