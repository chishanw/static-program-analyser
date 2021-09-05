#pragma once

#include <string>
#include <vector>

namespace query {
enum class DesignEntity {
  STATEMENT,
  READ,
  PRINT,
  CALL,
  WHILE,
  IF,
  ASSIGN,
  VARIABLE,
  CONSTANT,
  PROCEDURE
};

enum class RelationshipType {
  FOLLOWS,
  FOLLOWS_T,
  PARENT,
  PARENT_T,
  USES_S,
  USES_P,
  MODIFIES_S,
  MODIFIES_P
};

enum class MatchType { EXACT, SUB_EXPRESSION };

enum class ParamType { SYNONYM, LITERAL, WILDCARD };

struct Synonym {
  const DesignEntity entity;
  const std::string name;
};

struct Param {
  const ParamType type;
  const std::string value;
};

struct SuchThatClause {
  const RelationshipType relationshipType;
  const Param leftParam;
  const Param rightParam;
};

struct PatternClause {
  const MatchType matchType;
  const Synonym matchSynonym;
  const Param leftParam;
  const Param rightParam;
};

struct SelectClause {
  const Synonym selectSynonym;
  const std::vector<SuchThatClause> suchThatClauses;
  const std::vector<PatternClause> patternClauses;
};
}  // namespace query
