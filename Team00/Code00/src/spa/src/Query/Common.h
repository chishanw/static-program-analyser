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

enum class ParamType {
  SYNONYM,
  INTEGER_LITERAL,
  NAME_LITERAL,
  WILDCARD
};

enum class ConditionClauseType {
  SUCH_THAT,
  PATTERN
};

enum class MatchType { EXACT, SUB_EXPRESSION, ANY };

struct Synonym {
  const DesignEntity entity;
  const std::string name;
  bool operator ==(const Synonym& other) const {
    return entity == other.entity && name == other.name;
  }
};

struct Param {
  const ParamType type;
  const std::string value;
  bool operator ==(const Param& other) const {
    return type == other.type && value == other.value;
  }
};

struct SuchThatClause {
  const RelationshipType relationshipType;
  const Param leftParam;
  const Param rightParam;
  bool operator ==(const SuchThatClause& other) const {
    return relationshipType == other.relationshipType
      && leftParam == other.leftParam
      && rightParam == other.rightParam;
  }
};

struct PatternExpr {
  const MatchType matchType;
  const std::string expr;
  bool operator ==(const PatternExpr& other) const {
    return matchType == other.matchType
    && expr == other.expr;
  }
};

struct PatternClause {
  const Synonym matchSynonym;
  const Param leftParam;
  const PatternExpr patternExpr;
  bool operator ==(const PatternClause& other) const {
    return matchSynonym == other.matchSynonym
    && leftParam == other.leftParam
    && patternExpr == other.patternExpr;
  }
};

struct ConditionClause {
  const SuchThatClause suchThatClause;
  const PatternClause patternClause;
  const ConditionClauseType conditionClauseType;
  bool operator ==(const ConditionClause& other) const {
    return suchThatClause == other.suchThatClause
    && patternClause == other.patternClause
    && conditionClauseType == other.conditionClauseType;
  }
};

struct SelectClause {
  const Synonym selectSynonym;
  const std::vector<ConditionClause> conditionClauses;
  bool operator ==(const SelectClause& other) const {
    return selectSynonym == other.selectSynonym
    && conditionClauses == other.conditionClauses;
  }
};
}  // namespace query
