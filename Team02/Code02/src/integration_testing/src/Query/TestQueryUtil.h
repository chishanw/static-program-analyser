#pragma once

#include <Query/Parser/QueryParser.h>
#include <Query/Parser/QueryToken.h>

#include <vector>
#include <string>

class TestQueryUtil {
 public:
  static void AddSuchThatClause(std::vector<query::ConditionClause>&,
                                query::RelationshipType, query::ParamType,
                                std::string, query::ParamType, std::string);
  static void AddPatternClause(std::vector<query::ConditionClause>&,
                               query::Synonym, query::ParamType, std::string,
                               query::PatternExpr);
};
