#include "QueryParser.h"

#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;
using namespace query;
using namespace qpp;

// ============ Map definitions ============
const unordered_map<string, RelationshipType> keywordToStmtRefRs = {
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
};

const unordered_map<DesignEntity, RefType> designEntityToRefType = {
    {DesignEntity::STATEMENT, STATEMENT_REF},
    {DesignEntity::READ, STATEMENT_REF},
    {DesignEntity::PRINT, STATEMENT_REF},
    {DesignEntity::CALL, STATEMENT_REF},
    {DesignEntity::WHILE, STATEMENT_REF},
    {DesignEntity::IF, STATEMENT_REF},
    {DesignEntity::ASSIGN, STATEMENT_REF},
    {DesignEntity::VARIABLE, ENTITY_REF},
    {DesignEntity::PROCEDURE, ENTITY_REF},
};

const set<DesignEntity> validUsesStmtRefEntities = {
    DesignEntity::PRINT, DesignEntity::STATEMENT, DesignEntity::ASSIGN,
    DesignEntity::IF, DesignEntity::WHILE};

const set<DesignEntity> validModifiesStmtRefEntities = {
    DesignEntity::READ, DesignEntity::STATEMENT, DesignEntity::ASSIGN,
    DesignEntity::IF, DesignEntity::WHILE};

// ============ Helpers (Token) ============
QueryToken QueryParser::consumeToken() {
  if (it == endIt) {
    throw runtime_error(
        "QueryParser expected another QueryToken but received None.");
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
    throw runtime_error("Invalid format. QueryParser expected NAME token.");
  }
  return t.value;
}

char QueryParser::getCharSymbol() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::CHAR_SYMBOL) {
    throw runtime_error(
        "Invalid format. QueryParser expected CHAR_SYMBOL token.");
  }
  return t.value.at(0);
}

char QueryParser::getExactCharSymbol(const char& charMatch) {
  char charSymbol = getCharSymbol();
  if (charSymbol != charMatch) {
    throw runtime_error(
        "Invalid format. QueryParser expected a specific char token.");
  }
  return charSymbol;
}

DesignEntity QueryParser::getDesignEntity() {
  QueryToken t = consumeToken();
  string keyword = t.value;
  if (!isDesignEntity(keyword)) {
    throw runtime_error(
        "Invalid format. QueryParser expects a DesignEntity token.");
  }
  return keywordToDesignEntity.at(keyword);
}

string QueryParser::getKeyword() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::KEYWORD &&
      t.tokenType != TokenType::NAME_OR_KEYWORD) {
    throw runtime_error("Invalid format. QueryParser expects a keyword token.");
  }
  return t.value;
}

string QueryParser::getExactKeyword(const string& keywordMatch) {
  string keyword = getKeyword();
  if (keyword != keywordMatch) {
    throw runtime_error(
        "Invalid format. QueryParser expects a specific keyword token.");
  }
  return keyword;
}

// ============ Helpers (Getters for DesignEntity) ============
DesignEntity QueryParser::getEntityFromSynonymName(const string& name) {
  if (synonymMap.find(name) == synonymMap.end()) {
    throw runtime_error(
        "Invalid format. QueryParser expects a declared synonym name.");
  }
  return synonymMap.at(name);
}

RefType QueryParser::getRefTypeFromEntity(const DesignEntity& entity) {
  if (designEntityToRefType.find(entity) == designEntityToRefType.end()) {
    throw runtime_error(
        "Missing DesignEntity in the DesignEntity to RefType map");
  }
  return designEntityToRefType.at(entity);
}

// ============ Helpers (Getters for Param) ============
Param QueryParser::getStmtRefParam() {
  QueryToken t = consumeToken();
  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '_') {
    return {ParamType::WILDCARD, t.value};
  }

  if (t.tokenType == TokenType::INTEGER) {
    return {ParamType::INTEGER_LITERAL, t.value};
  }

  if (t.tokenType == TokenType::NAME_OR_KEYWORD) {
    // Get synonym name
    string synonymName = t.value;

    // Verify synonym is StmtRef type
    DesignEntity entity = getEntityFromSynonymName(synonymName);
    RefType refType = getRefTypeFromEntity(entity);

    if (refType == STATEMENT_REF) {
      return {ParamType::SYNONYM, synonymName};
    }
  }

  throw runtime_error("Invalid format. QueryParser expected a valid STMT_REF");
}

Param QueryParser::getEntRefParam() {
  QueryToken t = consumeToken();
  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '_') {
    return {ParamType::WILDCARD, t.value};
  }

  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '"') {
    string ident = getName();
    getExactCharSymbol('"');

    return {ParamType::NAME_LITERAL, ident};
  }

  if (t.tokenType == TokenType::NAME_OR_KEYWORD) {
    // Get synonym name
    string synonymName = t.value;

    // Verify synonym is EntRef type
    DesignEntity entity = getEntityFromSynonymName(synonymName);
    RefType refType = getRefTypeFromEntity(entity);

    if (refType == ENTITY_REF) {
      return {ParamType::SYNONYM, synonymName};
    }
  }

  throw runtime_error("Invalid format. QueryParser expected a valid ENT_REF");
}

pair<RefType, Param> QueryParser::getEntOrStmtRefParam() {
  QueryToken t = consumeToken();
  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '_') {
    throw runtime_error(
        "Invalid format. QueryParser expected a SYNONYM or LITERAL param type"
        " but got WILDCARD");
  }

  if (t.tokenType == TokenType::INTEGER) {
    return {RefType::STATEMENT_REF, {ParamType::INTEGER_LITERAL, t.value}};
  }

  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '"') {
    // Get IDENT
    string ident = getName();

    // Verify closing quotations
    getExactCharSymbol('"');

    return {RefType::ENTITY_REF, {ParamType::NAME_LITERAL, ident}};
  }

  if (t.tokenType == TokenType::NAME_OR_KEYWORD) {
    // Get synonym name
    string synonymName = t.value;

    // Get type of synonym
    DesignEntity entity = getEntityFromSynonymName(synonymName);
    RefType refType = getRefTypeFromEntity(entity);

    return {refType, {ParamType::SYNONYM, synonymName}};
  }

  throw runtime_error(
      "Invalid format. QueryParser expected a valid ENT_REF or STMT_REF");
}

// ============ Parsers ============
SynonymMap QueryParser::parseSynonyms() {
  SynonymMap synonyms;

  bool hasDeclaration = true;
  while (hasDeclaration) {
    // Parse design entity and synonym name
    DesignEntity entity = getDesignEntity();
    string name = getName();

    // if synonym name already exists
    if (synonyms.find(name) != synonyms.end()) {
      throw runtime_error("Invalid format. Duplicate synonym name found");
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
      throw runtime_error("Invalid format. QueryParser expected ; token");
    }

    optional<QueryToken> maybeToken = peekToken();
    hasDeclaration =
        maybeToken.has_value() && isDesignEntity(maybeToken.value().value);
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
      ConditionClause clause = parseSuchThatClause();
      conditionClauses.push_back(clause);

      // if there is a pattern clause after a such that clause
      if (hasNextToken()) {
        getExactKeyword("pattern");
        cout << "Query pattern parsing has not been implemented";
        // TODO(Beatrice): Implement parser for pattern clause
      }

    } else if (clauseKeyword == "pattern") {
      cout << "Query pattern parsing has not been implemented";
      // TODO(Beatrice): Implement parser for pattern clause

    } else {
      throw runtime_error(
          "Invalid format. QueryParser expects either a SUCH_THAT_KEYWORD"
          " or a PATTERN_KEYWORD token, and expects a max of 1 SUCH_THAT_CLAUSE"
          " and a max of 1 PATTERN_CLAUSE");
    }
  }

  return {selectSynonym, conditionClauses};
}

ConditionClause QueryParser::parseSuchThatClause() {
  string relationship = getKeyword();
  if (keywordToStmtRefRs.find(relationship) != keywordToStmtRefRs.end()) {
    RelationshipType type = keywordToStmtRefRs.at(relationship);

    getExactCharSymbol('(');
    Param left = getStmtRefParam();

    getExactCharSymbol(',');
    Param right = getStmtRefParam();

    getExactCharSymbol(')');

    SuchThatClause stClause = {type, left, right};
    return {stClause, {}, ConditionClauseType::SUCH_THAT};
  }

  if (relationship == "Uses") {
    getExactCharSymbol('(');

    // Parse left parameter
    pair<RefType, Param> leftRefTypeAndParam = getEntOrStmtRefParam();
    RefType leftRefType = leftRefTypeAndParam.first;
    Param left = leftRefTypeAndParam.second;

    // Validate left parameter REF and its DesignEntity type
    RelationshipType type;
    switch (leftRefType) {
      case STATEMENT_REF:
        type = RelationshipType::USES_S;
        break;
      case ENTITY_REF:
        throw runtime_error("UsesP is not supported yet.");
      default:
        throw runtime_error("Invalid fist param ref.");
    }
    if (left.type == ParamType::SYNONYM) {
      DesignEntity entity = getEntityFromSynonymName(left.value);
      if (validUsesStmtRefEntities.find(entity) ==
          validUsesStmtRefEntities.end()) {
        throw runtime_error("Invalid Uses first param synonym type.");
      }
    }

    getExactCharSymbol(',');

    // Parse right parameter
    Param right = getEntRefParam();

    // Validate right parameter is variable
    if (right.type == ParamType::SYNONYM &&
        getEntityFromSynonymName(right.value) != DesignEntity::VARIABLE) {
      throw runtime_error("Invalid Uses second param synonym type.");
    }
    getExactCharSymbol(')');

    SuchThatClause stClause = {type, left, right};
    return {stClause, {}, ConditionClauseType::SUCH_THAT};
  }

  if (relationship == "Modifies") {
    getExactCharSymbol('(');

    // Parse left parameter
    pair<RefType, Param> leftRefTypeAndParam = getEntOrStmtRefParam();
    RefType leftRefType = leftRefTypeAndParam.first;
    Param left = leftRefTypeAndParam.second;

    // Validate left parameter REF and its DesignEntity type
    RelationshipType type;
    switch (leftRefType) {
      case STATEMENT_REF:
        type = RelationshipType::MODIFIES_S;
        break;
      case ENTITY_REF:
        throw runtime_error("ModifiesP is not supported yet.");
      default:
        throw runtime_error("Invalid fist param ref.");
    }
    if (left.type == ParamType::SYNONYM) {
      DesignEntity entity = getEntityFromSynonymName(left.value);
      if (validModifiesStmtRefEntities.find(entity) ==
          validModifiesStmtRefEntities.end()) {
        throw runtime_error("Invalid Modifies first param synonym type.");
      }
    }

    getExactCharSymbol(',');

    // Parse right parameter
    Param right = getEntRefParam();

    // Validate right parameter is variable
    if (right.type == ParamType::SYNONYM &&
        getEntityFromSynonymName(right.value) != DesignEntity::VARIABLE) {
      throw runtime_error("Invalid Modifies second param synonym type.");
    }
    getExactCharSymbol(')');

    SuchThatClause stClause = {type, left, right};
    return {stClause, {}, ConditionClauseType::SUCH_THAT};
  }

  throw runtime_error("Invalid param format.");
}

// ============ MAIN ============

QueryParser::QueryParser() = default;

tuple<SynonymMap, SelectClause> QueryParser::Parse(const string& query) {
  tokens = QueryLexer().Tokenize(query);
  it = tokens.begin();
  endIt = tokens.end();

  DMOprintInfoMsg("Query tokens:");
  for (const QueryToken& t : tokens) {
    DMOprintInfoMsg("[" + to_string(t.tokenType) + "," + t.value + "]");
  }

  synonymMap = parseSynonyms();
  SelectClause selectClause = parseSelectClause();

  return {synonymMap, selectClause};
}
