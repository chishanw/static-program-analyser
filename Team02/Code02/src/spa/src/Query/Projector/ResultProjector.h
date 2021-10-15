#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <list>
#include <string>
#include <vector>

class ResultProjector {
 public:
  explicit ResultProjector(PKB*);
  std::list<std::string> formatResults(
      query::SelectType selectType, std::vector<query::Synonym> selectSynonyms,
      std::vector<std::vector<int>> results);

 private:
  PKB* pkb;
  std::string getStringForSynonym(query::Synonym synonym, int result);
};
