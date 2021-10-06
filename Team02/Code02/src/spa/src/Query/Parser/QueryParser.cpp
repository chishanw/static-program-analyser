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
    {"call", DesignEntity::CALL},
    {"while", DesignEntity::WHILE},
    {"if", DesignEntity::IF},
    {"assign", DesignEntity::ASSIGN},
    {"variable", DesignEntity::VARIABLE},
    {"constant", DesignEntity::CONSTANT},
    {"procedure", DesignEntity::PROCEDURE},
    {"prog_line", DesignEntity::PROG_LINE}};

const set<DesignEntity> validFollowsParentParamEntities = {
    DesignEntity::STATEMENT, DesignEntity::READ,     DesignEntity::PRINT,
    DesignEntity::WHILE,     DesignEntity::IF,       DesignEntity::ASSIGN,
    DesignEntity::CALL,      DesignEntity::PROG_LINE};

const set<DesignEntity> validUsesStmtParamEntities = {
    DesignEntity::PRINT,    DesignEntity::STATEMENT, DesignEntity::ASSIGN,
    DesignEntity::IF,       DesignEntity::WHILE,     DesignEntity::CALL,
    DesignEntity::PROG_LINE};

const set<DesignEntity> validModifiesStmtParamEntities = {
    DesignEntity::READ,     DesignEntity::STATEMENT, DesignEntity::ASSIGN,
    DesignEntity::IF,       DesignEntity::WHILE,     DesignEntity::CALL,
    DesignEntity::PROG_LINE};

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

// ============ Parsers (Main) ============
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

  while (hasNextToken()) {
    optional<QueryToken> nextToken = peekToken();
    string keyword = nextToken.value().value;

    if (keyword == "such") {
      parseSuchThatClause(conditionClauses);

    } else if (keyword == "pattern") {
      parsePatternClause(conditionClauses);

    } else {
      throw runtime_error(INVALID_ST_P_KEYWORD_MSG);
    }
  }

  return {selectSynonym, conditionClauses};
}

// ============ Parsers (such that) ============
void QueryParser::parseSuchThatClause(vector<ConditionClause>& results) {
  getExactKeyword("such");
  getExactKeyword("that");

  bool hasNextRelationship = true;
  while (hasNextRelationship) {
    string relationship = getKeyword();

    if (keywordToFollowsParentType.find(relationship) !=
        keywordToFollowsParentType.end()) {
      results.push_back(parseFollowsParentClause(relationship));

    } else if (relationship == "Uses") {
      results.push_back(parseUsesClause());

    } else if (relationship == "Modifies") {
      results.push_back(parseModifiesClause());

    } else {
      throw runtime_error(INVALID_ST_RELATIONSHIP_MSG);
    }

    // checks if there are multiple relationships connected by 'and'
    hasNextRelationship = hasNextToken() && peekToken().value().value == "and";
    if (hasNextRelationship) {
      getExactKeyword("and");
    }
  }
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
      type = RelationshipType::USES_P;
      break;
    }
    case ParamType::INTEGER_LITERAL: {
      type = RelationshipType::USES_S;
      break;
    }
    case ParamType::SYNONYM: {
      DesignEntity entity = getEntityFromSynonymName(left.value);
      if (entity == DesignEntity::PROCEDURE) {
        type = RelationshipType::USES_P;
        break;
      }
      if (validUsesStmtParamEntities.find(entity) !=
          validUsesStmtParamEntities.end()) {
        type = RelationshipType::USES_S;
        break;
      }
      throw runtime_error(INVALID_ST_USES_SYNONYM_ENTITY_MSG);
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
      type = RelationshipType::MODIFIES_P;
      break;
    }
    case ParamType::INTEGER_LITERAL: {
      type = RelationshipType::MODIFIES_S;
      break;
    }
    case ParamType::SYNONYM: {
      DesignEntity entity = getEntityFromSynonymName(left.value);
      if (entity == DesignEntity::PROCEDURE) {
        type = RelationshipType::MODIFIES_P;
        break;
      }
      if (validModifiesStmtParamEntities.find(entity) !=
          validModifiesStmtParamEntities.end()) {
        type = RelationshipType::MODIFIES_S;
        break;
      }
      throw runtime_error(INVALID_ST_MODIFIES_SYNONYM_ENTITY_MSG);
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

// ============ Parsers (pattern) ============
void QueryParser::parsePatternClause(vector<ConditionClause>& results) {
  getExactKeyword("pattern");

  bool hasNextPattern = true;
  while (hasNextPattern) {
    QueryToken nextToken = peekToken().value();
    if (nextToken.tokenType != TokenType::NAME_OR_KEYWORD) {
      throw new runtime_error(INVALID_NAME_MSG);
    }
    string nextSynonym = nextToken.value;
    DesignEntity entity = getEntityFromSynonymName(nextSynonym);

    switch (entity) {
      case DesignEntity::IF:
        results.push_back(parseIfPatternClause());
        break;
      case DesignEntity::WHILE:
        results.push_back(parseWhilePatternClause());
        break;
      case DesignEntity::ASSIGN:
        results.push_back(parseAssignPatternClause());
        break;
      default:
        throw runtime_error(INVALID_PATTERN_SYNONYM_MSG);
    }

    // checks if there are multiple patterns connected by 'and'
    hasNextPattern = hasNextToken() && peekToken().value().value == "and";
    if (hasNextPattern) {
      getExactKeyword("and");
    }
  }
}

query::ConditionClause QueryParser::parseIfPatternClause() {
  string synonymName = getName();
  DesignEntity entity = getEntityFromSynonymName(synonymName);
  // validate synonym type
  if (entity != DesignEntity::IF) {
    throw runtime_error(INVALID_SYNONYM_MSG);
  }

  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  getExactCharSymbol('_');
  getExactCharSymbol(',');
  getExactCharSymbol('_');
  getExactCharSymbol(')');

  // validate left param
  if (left.type == ParamType::INTEGER_LITERAL) {
    throw runtime_error(INVALID_ENT_REF_MSG);
  }
  if (left.type == ParamType::SYNONYM &&
      getEntityFromSynonymName(left.value) != DesignEntity::VARIABLE) {
    throw runtime_error(INVALID_SYNONYM_NON_VARIABLE_MSG);
  }
  Synonym ifSynonym = {entity, synonymName};
  PatternClause patternClause = {ifSynonym, left, {}};
  return {{}, patternClause, ConditionClauseType::PATTERN};
}

query::ConditionClause QueryParser::parseWhilePatternClause() {
  string synonymName = getName();
  DesignEntity entity = getEntityFromSynonymName(synonymName);
  // validate synonym type
  if (entity != DesignEntity::WHILE) {
    throw runtime_error(INVALID_SYNONYM_MSG);
  }

  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  getExactCharSymbol('_');
  getExactCharSymbol(')');

  // validate left param
  if (left.type == ParamType::INTEGER_LITERAL) {
    throw runtime_error(INVALID_ENT_REF_MSG);
  }
  if (left.type == ParamType::SYNONYM &&
      getEntityFromSynonymName(left.value) != DesignEntity::VARIABLE) {
    throw runtime_error(INVALID_SYNONYM_NON_VARIABLE_MSG);
  }
  Synonym whileSynonym = {entity, synonymName};
  PatternClause patternClause = {whileSynonym, left, {}};
  return {{}, patternClause, ConditionClauseType::PATTERN};
}

query::ConditionClause QueryParser::parseAssignPatternClause() {
  string synonymName = getName();
  DesignEntity entity = getEntityFromSynonymName(synonymName);
  // validate synonym type
  if (entity != DesignEntity::ASSIGN) {
    throw runtime_error(INVALID_SYNONYM_MSG);
  }

  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  PatternExpr patternExpr = parsePatternExpr();
  getExactCharSymbol(')');

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
  getExactCharSymbol('_');

  // expr is [_]
  if (peekToken().has_value() && peekToken().value().value == ")") {
    return {MatchType::ANY, "_"};
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
  return {MatchType::SUB_EXPRESSION, exprToken.value};
}

// ============ MAIN ============

QueryParser::QueryParser() = default;

tuple<SynonymMap, SelectClause> QueryParser::Parse(const string& query) {
  tokens = QueryLexer().Tokenize(query);
  it = tokens.begin();
  endIt = tokens.end();

  synonymMap = parseSynonyms();
  SelectClause selectClause = parseSelectClause();

  return {synonymMap, selectClause};
}
