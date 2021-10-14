#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

class ResultProjector {
 public:
  explicit ResultProjector(PKB*);
  std::list<std::string> formatResults(
      std::unordered_map<std::string, query::DesignEntity> synonymMap,
      query::SelectType selectType, std::vector<query::Synonym> selectSynonyms,
      std::vector<std::vector<int>> results);

 private:
  PKB* pkb;
  std::string getStringByDesignEntity(query::DesignEntity designEntity,
                                      int result);
};
