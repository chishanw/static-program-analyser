#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <list>
#include <string>
#include <unordered_set>

class ResultProjector {
 public:
  explicit ResultProjector(PKB*);
  std::list<std::string> formatResults(query::DesignEntity designEntity,
                                       std::unordered_set<int> results);

 private:
  PKB* pkb;
};
