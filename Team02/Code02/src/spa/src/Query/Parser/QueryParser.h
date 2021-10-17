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

typedef std::unordered_map<std::string, query::DesignEntity> SynonymMap;

class QueryParser {
 public:
  QueryParser();
  std::tuple<SynonymMap, query::SelectClause> Parse(const std::string&);

  // Error messages for invalid queries
  inline static const std::string INVALID_INSUFFICIENT_TOKENS_MSG =
      "QueryParser expected another QueryToken but received None.";
  inline static const std::string INVALID_NAME_MSG =
      "QueryParser expected a NAME.";
  inline static const std::string INVALID_CHAR_SYMBOL_MSG =
      "QueryParser expected a CHAR_SYMBOL.";
  inline static const std::string INVALID_SPECIFIC_CHAR_SYMBOL_MSG =
      "QueryParser expected a specific char symbol.";
  inline static const std::string INVALID_DESIGN_ENTITY_MSG =
      "QueryParser expects a DesignEntity.";
  inline static const std::string INVALID_KEYWORD_MSG =
      "QueryParser expects a keyword.";
  inline static const std::string INVALID_SPECIFIC_KEYWORD_MSG =
      "QueryParser expects a specific keyword.";

  inline static const std::string INVALID_UNDECLARED_SYNONYM_MSG =
      "QueryParser expects a declared synonym name but received an undeclared "
      "NAME.";
  inline static const std::string INVALID_DUPLICATE_SYNONYM_MSG =
      "QueryParser expects a unique synonym name but found duplicates.";
  inline static const std::string INVALID_SYNONYM_MSG =
      "QueryParser expects a valid synonym.";
  inline static const std::string INVALID_SYNONYM_NON_PROCEDURE_MSG =
      "QueryParser expects a procedure synonym";
  inline static const std::string INVALID_SYNONYM_NON_PROG_LINE_MSG =
      "QueryParser expects a prog_line synonym";
  inline static const std::string INVALID_SYNONYM_NON_VARIABLE_MSG =
      "QueryParser expects a variable synonym.";
  inline static const std::string INVALID_PATTERN_SYNONYM_MSG =
      "QueryParser expects an assignment, while or if synonym.";

  inline static const std::string INVALID_ATTRIBUTE =
      "QueryParser expects a valid attribute name.";
  inline static const std::string INVALID_SYNONYM_ATTRIBUTE_MATCH =
      "QueryParser found an invalid attribute for the given synonym.";
  inline static const std::string INVALID_STMT_REF_MSG =
      "QueryParser expects a STMT_REF.";
  inline static const std::string INVALID_LINE_REF_MSG =
      "QueryParser expects a LINE_REF.";
  inline static const std::string INVALID_ENT_REF_MSG =
      "QueryParser expects an ENTITY_REF.";
  inline static const std::string INVALID_STMT_ENT_REF_MSG =
      "QueryParser expects a STMT_REF or a ENTITY_REF.";
  inline static const std::string INVALID_PARAM_TYPE_MSG =
      "QueryParser expects a valid param type.";

  inline static const std::string INVALID_RESULT_TYPE_MSG =
      "QueryParser expects a BOOLEAN, a tuple or one synonym for result "
      "clause.";
  inline static const std::string INVALID_ST_P_KEYWORD_MSG =
      "QueryParser expects a such that or pattern or with keyword.";
  inline static const std::string INVALID_ST_RELATIONSHIP_MSG =
      "QueryParser expects a valid such-that relationship.";
  inline static const std::string INVALID_ST_USES_MODIFIES_WILDCARD_MSG =
      "QueryParser expects SYNONYM or LITERAL param type but got WILDCARD.";
  inline static const std::string INVALID_ST_USES_MODIFIES_INTEGER_MSG =
      "QueryParser expects SYNONYM, WILDCARD, or IDENT param type but got "
      "INTEGER.";
  inline static const std::string INVALID_ST_USES_SYNONYM_ENTITY_MSG =
      "QueryParser expects a print, statement, call, assign, if, while, "
      "procedure or prog_line synonym.";
  inline static const std::string INVALID_ST_MODIFIES_SYNONYM_ENTITY_MSG =
      "QueryParser expects a read, statement, call, assign, if, while, "
      "procedure or prog_line synonym.";

  inline static const std::string INVALID_P_EXPR_CHARA_MSG =
      "QueryParser found an invalid character in the pattern expr.";

  inline static const std::string INVALID_W_SYNONYM =
      "QueryParser expects a prog_line synonym.";
  inline static const std::string INVALID_W_PARAM =
      "QueryParser expects an integer, string, attrRef or synonym.";
  inline static const std::string INVALID_W_DIFF_PARAM_TYPES =
      "QueryParser expects the same type (string/integer) to be compared in "
      "the with-clause.";

 private:
  std::vector<qpp::QueryToken>::iterator it;
  std::vector<qpp::QueryToken>::iterator endIt;
  SynonymMap synonymMap;

  // used to return FALSE for semantically invalid Select BOOLEAN clause
  bool isSemanticallyValid;
  std::string semanticErrorMsg;

  bool hasNextToken();
  qpp::QueryToken consumeToken();
  std::optional<qpp::QueryToken> peekToken();

  bool isDesignEntity(const std::string&);

  std::string getNameOrKeyword();
  char getCharSymbol();
  char getExactCharSymbol(const char&);
  query::DesignEntity getDesignEntity();
  std::string getKeyword();
  std::string getExactKeyword(const std::string&);
  query::Attribute getAttribute(const std::string&);

  query::DesignEntity getEntityFromSynonymName(const std::string&);
  query::Param getRefParam();

  SynonymMap parseSynonymDeclarations();
  query::SelectClause parseSelectClause();
  std::tuple<std::vector<query::Synonym>, query::SelectType>
  parseResultClause();

  void parseSuchThatClause(std::vector<query::ConditionClause>&);
  query::ConditionClause parseFollowsParentClause(const std::string&);
  query::ConditionClause parseCallsClause(const std::string&);
  query::ConditionClause parseNextClause(const std::string&);
  query::ConditionClause parseUsesClause();
  query::ConditionClause parseModifiesClause();

  void parsePatternClause(std::vector<query::ConditionClause>&);
  query::ConditionClause parseWhilePatternClause();
  query::ConditionClause parseIfPatternClause();
  query::ConditionClause parseAssignPatternClause();
  query::PatternExpr parsePatternExpr();

  void parseWithClause(std::vector<query::ConditionClause>&);
};
