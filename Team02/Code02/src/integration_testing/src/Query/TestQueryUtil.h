#pragma once

#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <vector>

class TestQueryUtil {
 public:
  static void AddSuchThatClause(std::vector<query::ConditionClause>&,
                                query::RelationshipType, query::ParamType,
                                std::string, query::ParamType, std::string);
  static void AddPatternClause(std::vector<query::ConditionClause>&,
                               query::Synonym, query::ParamType, std::string,
                               query::PatternExpr);
};
