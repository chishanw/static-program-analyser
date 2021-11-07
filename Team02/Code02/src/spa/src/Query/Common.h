#pragma once

#include <Common/Common.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace query {
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
  DesignEntity entity;
  std::string name;
  bool hasAttribute;
  Attribute attribute;
  bool operator==(const Synonym& other) const {
    return entity == other.entity && name == other.name &&
           hasAttribute == other.hasAttribute && attribute == other.attribute;
  }
};

struct Param {
  ParamType type;
  std::string value;
  bool operator==(const Param& other) const {
    return type == other.type && value == other.value;
  }
};

struct SuchThatClause {
  RelationshipType relationshipType;
  Param leftParam;
  Param rightParam;
  bool operator==(const SuchThatClause& other) const {
    return relationshipType == other.relationshipType &&
           leftParam == other.leftParam && rightParam == other.rightParam;
  }
};

struct PatternExpr {
  MatchType matchType;
  std::string expr;
  bool operator==(const PatternExpr& other) const {
    return matchType == other.matchType && expr == other.expr;
  }
};

struct PatternClause {
  Synonym matchSynonym;
  Param leftParam;
  PatternExpr patternExpr;
  bool operator==(const PatternClause& other) const {
    return matchSynonym == other.matchSynonym && leftParam == other.leftParam &&
           patternExpr == other.patternExpr;
  }
};

struct WithClause {
  Param leftParam;
  Param rightParam;
  bool operator==(const WithClause& other) const {
    return leftParam == other.leftParam && rightParam == other.rightParam;
  }
};

struct ConditionClause {
  SuchThatClause suchThatClause;
  PatternClause patternClause;
  WithClause withClause;
  ConditionClauseType conditionClauseType;
  bool operator==(const ConditionClause& other) const {
    return suchThatClause == other.suchThatClause &&
           patternClause == other.patternClause &&
           withClause == other.withClause &&
           conditionClauseType == other.conditionClauseType;
  }
};

struct SelectClause {
  std::vector<Synonym> selectSynonyms;
  SelectType selectType;
  std::vector<ConditionClause> conditionClauses;
  bool operator==(const SelectClause& other) const {
    return selectSynonyms == other.selectSynonyms &&
           conditionClauses == other.conditionClauses;
  }
};

struct GroupDetails {
  bool isBooleanGroup;
  std::vector<Synonym> selectedSynonyms;
  bool operator==(const GroupDetails& other) const {
    return isBooleanGroup == other.isBooleanGroup &&
           selectedSynonyms == other.selectedSynonyms;
  }
};

const int FALSE_SELECT_BOOL_RESULT = 0;
const int TRUE_SELECT_BOOL_RESULT = 1;

typedef std::string SynName;
typedef std::unordered_map<std::string, int> QueryResult;
typedef std::unordered_map<std::string, int> IntermediateQueryResult;
typedef std::unordered_set<std::vector<int>, VectorHash> ClauseIncomingResults,
    FinalQueryResults;
typedef std::unordered_map<std::string, int> SynonymCountsTable;
typedef std::unordered_map<std::string, std::unordered_set<int>>
    SynonymValuesTable;
typedef std::unordered_map<SynName, int> SynonymCountsTable;
}  // namespace query
