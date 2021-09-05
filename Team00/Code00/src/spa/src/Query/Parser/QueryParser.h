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
typedef std::vector<qpp::QueryToken>::iterator tIterator;

class QueryParser {
 public:
  static std::tuple<SynonymMap, query::SelectClause> ParseQuery(
      const std::string&);

 private:
  static bool hasNextToken(tIterator&, tIterator&);
  static qpp::QueryToken consumeToken(tIterator&, tIterator&);
  static std::optional<qpp::QueryToken> peekToken(tIterator&, tIterator&);

  static bool isDesignEntity(const std::string&);

  static std::string getName(tIterator&, tIterator&);
  static char getCharSymbol(tIterator&, tIterator&);
  static char getExactCharSymbol(tIterator&, tIterator&, const char&);
  static query::DesignEntity getDesignEntity(tIterator&, tIterator&);
  static std::string getKeyword(tIterator&, tIterator&);
  static std::string getExactKeyword(tIterator&, tIterator&,
                                     const std::string&);

  static query::DesignEntity getEntityFromSynonymName(const std::string&,
                                                      const SynonymMap&);
  static RefType getRefTypeFromEntity(const query::DesignEntity&);
  static query::Param getStmtRefParam(tIterator&, tIterator&,
                                      const SynonymMap&);
  static query::Param getEntRefParam(tIterator&, tIterator&, const SynonymMap&);
  static std::pair<RefType, query::Param> getEntOrStmtRefParam(
      tIterator&, tIterator&, const SynonymMap&);

  static SynonymMap parseSynonyms(tIterator&, tIterator&);
  static query::SelectClause parseSelectClause(tIterator&, tIterator&,
                                               const SynonymMap&);
  static query::SuchThatClause parseSuchThatClause(tIterator&, tIterator&,
                                                   const SynonymMap&);
  static query::PatternClause parsePatternClause(
      tIterator&, tIterator&,
      const SynonymMap&);  // TODO(Beatrice): Implement parser for pattern
                           // clause
};
