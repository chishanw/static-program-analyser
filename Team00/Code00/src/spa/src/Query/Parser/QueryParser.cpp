#include "QueryParser.h"

#include <set>

using namespace std;
using namespace query;
using namespace qpp;

// ============ Map definitions ============
const unordered_map<string, RelationshipType> keywordToFollowsParentType = {
    {"Follows", RelationshipType::FOLLOWS},
    {"Follows*", RelationshipType::FOLLOWS_T},
    {"Parent", RelationshipType::PARENT},
    {"Parent*", RelationshipType::PARENT_T}};

const unordered_map<string, DesignEntity> keywordToDesignEntity = {
    {"stmt", DesignEntity::STATEMENT},
    {"read", DesignEntity::READ},
    {"print", DesignEntity::PRINT},
    {"while", DesignEntity::WHILE},
    {"if", DesignEntity::IF},
    {"assign", DesignEntity::ASSIGN},
    {"variable", DesignEntity::VARIABLE},
    {"constant", DesignEntity::CONSTANT},
    {"procedure", DesignEntity::PROCEDURE}};

const set<DesignEntity> validFollowsParentParamEntities = {
    DesignEntity::STATEMENT, DesignEntity::READ, DesignEntity::PRINT,
    DesignEntity::WHILE,     DesignEntity::IF,   DesignEntity::ASSIGN};

const set<DesignEntity> validUsesStmtParamEntities = {
    DesignEntity::PRINT, DesignEntity::STATEMENT, DesignEntity::ASSIGN,
    DesignEntity::IF, DesignEntity::WHILE};

const set<DesignEntity> validModifiesStmtParamEntities = {
    DesignEntity::READ, DesignEntity::STATEMENT, DesignEntity::ASSIGN,
    DesignEntity::IF, DesignEntity::WHILE};

// ============ Helpers (Token) ============
QueryToken QueryParser::consumeToken() {
  if (it == endIt) {
    throw runtime_error(INVALID_INSUFFICIENT_TOKENS_MSG);
  }
  QueryToken token = *it;
  ++it;
  return token;
}

bool QueryParser::hasNextToken() { return it != endIt; }

optional<QueryToken> QueryParser::peekToken() {
  if (!hasNextToken()) {
    return nullopt;
  }
  return {*it};
}
// ============ Helpers (Bool checks) ============

bool QueryParser::isDesignEntity(const string& maybeEntity) {
  return keywordToDesignEntity.find(maybeEntity) != keywordToDesignEntity.end();
}

// ============ Helpers (Getters and validation for Token) ============
string QueryParser::getName() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::NAME_OR_KEYWORD) {
    throw runtime_error(INVALID_NAME_MSG);
  }
  return t.value;
}

char QueryParser::getCharSymbol() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::CHAR_SYMBOL) {
    throw runtime_error(INVALID_CHAR_SYMBOL_MSG);
  }
  return t.value.at(0);
}

char QueryParser::getExactCharSymbol(const char& charMatch) {
  char charSymbol = getCharSymbol();
  if (charSymbol != charMatch) {
    throw runtime_error(INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }
  return charSymbol;
}

DesignEntity QueryParser::getDesignEntity() {
  QueryToken t = consumeToken();
  string keyword = t.value;
  if (!isDesignEntity(keyword)) {
    throw runtime_error(INVALID_DESIGN_ENTITY_MSG);
  }
  return keywordToDesignEntity.at(keyword);
}

string QueryParser::getKeyword() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::KEYWORD &&
      t.tokenType != TokenType::NAME_OR_KEYWORD) {
    throw runtime_error(INVALID_KEYWORD_MSG);
  }
  return t.value;
}

string QueryParser::getExactKeyword(const string& keywordMatch) {
  string keyword = getKeyword();
  if (keyword != keywordMatch) {
    throw runtime_error(INVALID_SPECIFIC_KEYWORD_MSG);
  }
  return keyword;
}

// ============ Helpers (Getters for DesignEntity) ============
DesignEntity QueryParser::getEntityFromSynonymName(const string& name) {
  if (synonymMap.find(name) == synonymMap.end()) {
    throw runtime_error(INVALID_UNDECLARED_SYNONYM_MSG);
  }
  return synonymMap.at(name);
}

// ============ Helpers (Getters for Param) ============
query::Param QueryParser::getRefParam() {
  QueryToken t = consumeToken();
  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value == "_") {
    return {ParamType::WILDCARD, t.value};
  }

  if (t.tokenType == TokenType::INTEGER) {
    return {ParamType::INTEGER_LITERAL, t.value};
  }

  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value == "\"") {
    string ident = getName();
    // Check if closing quotations is present
    getExactCharSymbol('"');
    return {ParamType::NAME_LITERAL, ident};
  }

  if (t.tokenType == TokenType::NAME_OR_KEYWORD) {
    string synonymName = t.value;
    return {ParamType::SYNONYM, synonymName};
  }

  throw runtime_error(INVALID_STMT_ENT_REF_MSG);
}

// ============ Parsers ============
SynonymMap QueryParser::parseSynonyms() {
  SynonymMap synonyms;

  optional<QueryToken> maybeToken = peekToken();
  bool hasDeclaration =
      maybeToken.has_value() && isDesignEntity(maybeToken.value().value);
  while (hasDeclaration) {
    // Parse design entity and synonym name
    DesignEntity entity = getDesignEntity();
    string name = getName();

    // if synonym name already exists
    if (synonyms.find(name) != synonyms.end()) {
      throw runtime_error(INVALID_DUPLICATE_SYNONYM_MSG);
    }

    // Populate synonym map
    synonyms[name] = entity;

    // Parse delimiter symbol
    char symbol = getCharSymbol();

    // Parse any extra synonym names for the same design entity
    bool hasSynonymName = symbol == ',';
    while (hasSynonymName) {
      // Parse next synonym name for the same design entity
      string nextName = getName();

      // Populate synonym map
      synonyms[nextName] = entity;

      // Parse and update delimiter symbol
      symbol = getCharSymbol();

      hasSynonymName = symbol == ',';
    }

    if (symbol != ';') {
      throw runtime_error(INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    }

    optional<QueryToken> maybeNextToken = peekToken();
    hasDeclaration = maybeNextToken.has_value() &&
                     isDesignEntity(maybeNextToken.value().value);
  }
  return synonyms;
}

SelectClause QueryParser::parseSelectClause() {
  // Verify select keyword is present
  getExactKeyword("Select");

  // Parse select selectSynonym
  string sName = getName();
  DesignEntity designEntity = getEntityFromSynonymName(sName);
  Synonym selectSynonym = {designEntity, sName};

  // Parse clauses
  vector<ConditionClause> conditionClauses;

  if (hasNextToken()) {
    string clauseKeyword = getKeyword();

    if (clauseKeyword == "such") {
      string nextKeyword = getExactKeyword("that");
      ConditionClause suchThatClause = parseSuchThatClause();
      conditionClauses.push_back(suchThatClause);

      // if there is a pattern clause after a such that clause
      if (hasNextToken()) {
        getExactKeyword("pattern");
        ConditionClause patternClause = parsePatternClause();
        conditionClauses.push_back(patternClause);
      }

    } else if (clauseKeyword == "pattern") {
      ConditionClause patternClause = parsePatternClause();
      conditionClauses.push_back(patternClause);

    } else {
      throw runtime_error(INVALID_ST_P_NUM_MSG);
    }
  }
  if (hasNextToken()) {
    throw runtime_error(INVALID_ST_P_NUM_MSG);
  }

  return {selectSynonym, conditionClauses};
}

ConditionClause QueryParser::parseSuchThatClause() {
  string relationship = getKeyword();
  if (keywordToFollowsParentType.find(relationship) !=
      keywordToFollowsParentType.end()) {
    return parseFollowsParentClause(relationship);
  }
  if (relationship == "Uses") {
    return parseUsesClause();
  }
  if (relationship == "Modifies") {
    return parseModifiesClause();
  }
  throw runtime_error(INVALID_ST_RELATIONSHIP_MSG);
}

query::ConditionClause QueryParser::parseFollowsParentClause(
    const string& relationship) {
  RelationshipType type = keywordToFollowsParentType.at(relationship);

  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  // Validate parameters
  if (left.type == ParamType::NAME_LITERAL ||
      right.type == ParamType::NAME_LITERAL) {
    throw runtime_error(INVALID_STMT_REF_MSG);
  }

  if (left.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(left.value);
    if (validFollowsParentParamEntities.find(entity) ==
        validFollowsParentParamEntities.end()) {
      throw runtime_error(INVALID_SYNONYM_MSG);
    }
  }

  if (right.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(right.value);
    if (validFollowsParentParamEntities.find(entity) ==
        validFollowsParentParamEntities.end()) {
      throw runtime_error(INVALID_SYNONYM_MSG);
    }
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parseUsesClause() {
  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  RelationshipType type;
  // Identify relationship type and validate left param
  switch (left.type) {
    case ParamType::WILDCARD: {
      throw runtime_error(INVALID_ST_USES_MODIFIES_WILDCARD_MSG);
    }
    case ParamType::NAME_LITERAL: {
      throw runtime_error(INVALID_ST_USESP_MODIFIESP_MSG);
    }
    case ParamType::INTEGER_LITERAL: {
      type = RelationshipType::USES_S;
      break;
    }
    case ParamType::SYNONYM: {
      type = RelationshipType::USES_S;
      DesignEntity entity = getEntityFromSynonymName(left.value);
      if (validUsesStmtParamEntities.find(entity) ==
          validUsesStmtParamEntities.end()) {
        throw runtime_error(INVALID_ST_USES_SYNONYM_ENTITY_MSG);
      }
      break;
    }
    default:
      throw runtime_error(INVALID_PARAM_TYPE_MSG);
  }

  // Validate right param
  if (right.type == ParamType::INTEGER_LITERAL) {
    throw runtime_error(INVALID_ST_USES_MODIFIES_INTEGER_MSG);
  }

  if (right.type == ParamType::SYNONYM &&
      getEntityFromSynonymName(right.value) != DesignEntity::VARIABLE) {
    throw runtime_error(INVALID_SYNONYM_NON_VARIABLE_MSG);
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parseModifiesClause() {
  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  RelationshipType type;
  // Identify relationship type and validate left param
  switch (left.type) {
    case ParamType::WILDCARD: {
      throw runtime_error(INVALID_ST_USES_MODIFIES_WILDCARD_MSG);
    }
    case ParamType::NAME_LITERAL: {
      throw runtime_error(INVALID_ST_USESP_MODIFIESP_MSG);
    }
    case ParamType::INTEGER_LITERAL: {
      type = RelationshipType::MODIFIES_S;
      break;
    }
    case ParamType::SYNONYM: {
      type = RelationshipType::MODIFIES_S;
      DesignEntity entity = getEntityFromSynonymName(left.value);
      if (validModifiesStmtParamEntities.find(entity) ==
          validModifiesStmtParamEntities.end()) {
        throw runtime_error(INVALID_ST_MODIFIES_SYNONYM_ENTITY_MSG);
      }
      break;
    }
    default:
      throw runtime_error(INVALID_PARAM_TYPE_MSG);
  }

  // Validate right param
  if (right.type == ParamType::INTEGER_LITERAL) {
    throw runtime_error(INVALID_ST_USES_MODIFIES_INTEGER_MSG);
  }

  if (right.type == ParamType::SYNONYM &&
      getEntityFromSynonymName(right.value) != DesignEntity::VARIABLE) {
    throw runtime_error(INVALID_SYNONYM_NON_VARIABLE_MSG);
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parsePatternClause() {
  string synonymName = getName();
  DesignEntity entity = getEntityFromSynonymName(synonymName);
  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  PatternExpr patternExpr = parsePatternExpr();
  getExactCharSymbol(')');

  // validate that synonym is assignment
  if (entity != DesignEntity::ASSIGN) {
    throw runtime_error(INVALID_SYNONYM_NON_ASSIGN_MSG);
  }
  // validate left param
  if (left.type == ParamType::INTEGER_LITERAL) {
    throw runtime_error(INVALID_ENT_REF_MSG);
  }
  if (left.type == ParamType::SYNONYM &&
      getEntityFromSynonymName(left.value) != DesignEntity::VARIABLE) {
    throw runtime_error(INVALID_SYNONYM_NON_VARIABLE_MSG);
  }

  Synonym patternSynonym = {entity, synonymName};
  PatternClause patternClause = {patternSynonym, left, patternExpr};
  return {{}, patternClause, ConditionClauseType::PATTERN};
}

query::PatternExpr QueryParser::parsePatternExpr() {
  MatchType matchType;
  string expr;
  char symbol = getCharSymbol();
  switch (symbol) {
    case '"': {  // expr is ["FACTOR"]
      QueryToken exprToken = consumeToken();
      // if expr is not IDENT or literal, throw error
      if (exprToken.tokenType != TokenType::NAME_OR_KEYWORD &&
          exprToken.tokenType != TokenType::INTEGER) {
        throw runtime_error(INVALID_P_EXPR_CHARA_MSG);
      }
      getExactCharSymbol('"');
      matchType = MatchType::EXACT;
      expr = exprToken.value;
      break;
    }
    case '_': {
      // expr is [_]
      if (peekToken().has_value() && peekToken().value().value == ")") {
        matchType = MatchType::ANY;
        expr = "_";
        break;
      }

      // expr is [_"FACTOR"_]
      getExactCharSymbol('"');
      QueryToken exprToken = consumeToken();
      // if expr is not IDENT or literal, throw error
      if (exprToken.tokenType != TokenType::NAME_OR_KEYWORD &&
          exprToken.tokenType != TokenType::INTEGER) {
        throw runtime_error(INVALID_P_EXPR_CHARA_MSG);
      }
      getExactCharSymbol('"');
      getExactCharSymbol('_');
      matchType = MatchType::SUB_EXPRESSION;
      expr = exprToken.value;
      break;
    }
    default:
      throw runtime_error(INVALID_P_EXPR_CHARA_MSG);
  }
  return {matchType, expr};
}

// ============ MAIN ============

QueryParser::QueryParser() = default;

tuple<SynonymMap, SelectClause> QueryParser::Parse(const string& query) {
  tokens = QueryLexer().Tokenize(query);
  it = tokens.begin();
  endIt = tokens.end();

  // TODO(Beatrice): Remove before submission
  DMOprintInfoMsg("Query tokens:");
  for (const QueryToken& t : tokens) {
    DMOprintInfoMsg("[" + to_string(t.tokenType) + "," + t.value + "]");
  }

  synonymMap = parseSynonyms();
  SelectClause selectClause = parseSelectClause();

  return {synonymMap, selectClause};
}
