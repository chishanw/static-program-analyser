#pragma once

#include <Common/Common.h>

#include <string>
#include <unordered_map>
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
  PROCEDURE,
  PROG_LINE
};

enum class Attribute { PROC_NAME, VAR_NAME, VALUE, STMT_NUM };

enum class ParamType {
  SYNONYM,
  INTEGER_LITERAL,
  NAME_LITERAL,
  WILDCARD,
  ATTRIBUTE_PROC_NAME,
  ATTRIBUTE_VAR_NAME,
  ATTRIBUTE_VALUE,
  ATTRIBUTE_STMT_NUM
};

enum class ConditionClauseType { SUCH_THAT, PATTERN, WITH };

enum class MatchType { EXACT, SUB_EXPRESSION, ANY };

enum class SelectType { SYNONYMS, BOOLEAN };

struct Synonym {
  const DesignEntity entity;
  const std::string name;
  const bool hasAttribute;
  const Attribute attribute;
  bool operator==(const Synonym& other) const {
    return entity == other.entity && name == other.name &&
           hasAttribute == other.hasAttribute && attribute == other.attribute;
  }
};

struct Param {
  const ParamType type;
  const std::string value;
  bool operator==(const Param& other) const {
    return type == other.type && value == other.value;
  }
};

struct SuchThatClause {
  const RelationshipType relationshipType;
  const Param leftParam;
  const Param rightParam;
  bool operator==(const SuchThatClause& other) const {
    return relationshipType == other.relationshipType &&
           leftParam == other.leftParam && rightParam == other.rightParam;
  }
};

struct PatternExpr {
  const MatchType matchType;
  const std::string expr;
  bool operator==(const PatternExpr& other) const {
    return matchType == other.matchType && expr == other.expr;
  }
};

struct PatternClause {
  const Synonym matchSynonym;
  const Param leftParam;
  const PatternExpr patternExpr;
  bool operator==(const PatternClause& other) const {
    return matchSynonym == other.matchSynonym && leftParam == other.leftParam &&
           patternExpr == other.patternExpr;
  }
};

struct WithClause {
  const Param leftParam;
  const Param rightParam;
  bool operator==(const WithClause& other) const {
    return leftParam == other.leftParam && rightParam == other.rightParam;
  }
};

struct ConditionClause {
  const SuchThatClause suchThatClause;
  const PatternClause patternClause;
  const WithClause withClause;
  const ConditionClauseType conditionClauseType;
  bool operator==(const ConditionClause& other) const {
    return suchThatClause == other.suchThatClause &&
           patternClause == other.patternClause &&
           withClause == other.withClause &&
           conditionClauseType == other.conditionClauseType;
  }
};

struct SelectClause {
  const std::vector<Synonym> selectSynonyms;
  const SelectType selectType;
  const std::vector<ConditionClause> conditionClauses;
  bool operator==(const SelectClause& other) const {
    return selectSynonyms == other.selectSynonyms &&
           conditionClauses == other.conditionClauses;
  }
};

struct GroupDetails {
  const bool isBooleanGroup;
  const std::vector<Synonym> selectedSynonyms;
  bool operator==(const GroupDetails& other) const {
    return isBooleanGroup == other.isBooleanGroup &&
    selectedSynonyms == other.selectedSynonyms;
  }
};

const int FALSE_SELECT_BOOL_RESULT = 0;
const int TRUE_SELECT_BOOL_RESULT = 1;

typedef std::string SYN_NAME;
typedef std::unordered_map<std::string, int> QueryResult;
typedef std::unordered_map<SYN_NAME , int> SynonymCountsTable;
}  // namespace query
