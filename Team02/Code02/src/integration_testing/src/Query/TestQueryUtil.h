#pragma once

#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>
#include <Common/Common.h>

#include <set>
#include <string>
#include <vector>

class TestQueryUtil {
 public:
  static void AddSuchThatClause(std::vector<query::ConditionClause>&,
                                RelationshipType, query::ParamType,
                                std::string, query::ParamType, std::string);
  static void AddPatternClause(std::vector<query::ConditionClause>&,
                               query::Synonym, query::ParamType, std::string,
                               query::PatternExpr);
  static void AddWithClause(std::vector<query::ConditionClause>&,
                               query::ParamType, std::string,
                               query::ParamType, std::string);
  static std::set<int> getUniqueSelectSingleQEResults(
      std::vector<std::vector<int>> results);
};
