#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Common/Global.h"
#include "Query/Common.h"
#include "QueryLexer.h"

enum RefType { STATEMENT_REF, ENTITY_REF };

typedef std::unordered_map<std::string, query::DesignEntity> SynonymMap;

class QueryParser {
 public:
  QueryParser();
  std::tuple<SynonymMap, query::SelectClause> Parse(const std::string&);

 private:
  std::vector<qpp::QueryToken> tokens;
  std::vector<qpp::QueryToken>::iterator it;
  std::vector<qpp::QueryToken>::iterator endIt;
  SynonymMap synonymMap;

  bool hasNextToken();
  qpp::QueryToken consumeToken();
  std::optional<qpp::QueryToken> peekToken();

  bool isDesignEntity(const std::string&);

  std::string getName();
  char getCharSymbol();
  char getExactCharSymbol(const char&);
  query::DesignEntity getDesignEntity();
  std::string getKeyword();
  std::string getExactKeyword(const std::string&);

  query::DesignEntity getEntityFromSynonymName(const std::string&);
  RefType getRefTypeFromEntity(const query::DesignEntity&);
  query::Param getStmtRefParam();
  query::Param getEntRefParam();
  std::pair<RefType, query::Param> getEntOrStmtRefParam();

  SynonymMap parseSynonyms();
  query::SelectClause parseSelectClause();
  query::ConditionClause parseSuchThatClause();
  query::ConditionClause parsePatternClause();
  // TODO(Beatrice): Implement parser for pattern clause
};
