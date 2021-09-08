#include "QueryParser.h"

#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;
using namespace query;
using namespace qpp;

// ============ Map definitions ============
static const unordered_map<string, RelationshipType> keywordToStmtRefRs = {
    {"Follows", RelationshipType::FOLLOWS},
    {"Follows*", RelationshipType::FOLLOWS_T},
    {"Parent", RelationshipType::PARENT},
    {"Parent*", RelationshipType::PARENT_T}};

static const unordered_map<string, DesignEntity> keywordToDesignEntity = {
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

static const unordered_map<DesignEntity, RefType> designEntityToRefType = {
    {DesignEntity::STATEMENT, STATEMENT_REF},
    {DesignEntity::READ, STATEMENT_REF},
    {DesignEntity::PRINT, STATEMENT_REF},
    {DesignEntity::CALL, STATEMENT_REF},
    {DesignEntity::WHILE, STATEMENT_REF},
    {DesignEntity::IF, STATEMENT_REF},
    {DesignEntity::ASSIGN, STATEMENT_REF},
    {DesignEntity::VARIABLE, ENTITY_REF},
    {DesignEntity::CONSTANT, ENTITY_REF},
    {DesignEntity::PROCEDURE, ENTITY_REF},
};

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
    return {ParamType::LITERAL, t.value};
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

    return {ParamType::LITERAL, ident};
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
    return {RefType::STATEMENT_REF, {ParamType::LITERAL, t.value}};
  }

  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '"') {
    // Get IDENT
    string ident = getName();

    // Verify closing quotations
    getExactCharSymbol('"');

    return {RefType::ENTITY_REF, {ParamType::LITERAL, ident}};
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
  vector<SuchThatClause> suchThatClauses;
  vector<PatternClause> patternClauses;

  bool hasNext = hasNextToken();

  while (hasNext) {
    string clauseKeyword = getKeyword();

    if (clauseKeyword == "such") {
      string nextKeyword = getExactKeyword("that");
      SuchThatClause suchThatClause = parseSuchThatClause();
      suchThatClauses.push_back(suchThatClause);

    } else if (clauseKeyword == "pattern") {
      continue;  // TODO(Beatrice): Implement parser for pattern clause

    } else {
      throw runtime_error(
          "Invalid format. QueryParser expects either SUCH_THAT_KEYWORDS"
          " or PATTERN_KEYWORD token");
    }

    hasNext = hasNextToken();
  }

  return {selectSynonym, suchThatClauses, patternClauses};
}

SuchThatClause QueryParser::parseSuchThatClause() {
  string relationship = getKeyword();
  if (keywordToStmtRefRs.find(relationship) != keywordToStmtRefRs.end()) {
    RelationshipType type = keywordToStmtRefRs.at(relationship);

    getExactCharSymbol('(');
    Param left = getStmtRefParam();

    getExactCharSymbol(',');
    Param right = getStmtRefParam();

    getExactCharSymbol(')');

    return {type, left, right};
  }

  if (relationship == "Uses") {
    getExactCharSymbol('(');

    // Parse left parameter
    pair<RefType, Param> left = getEntOrStmtRefParam();
    RelationshipType type;
    if (left.first == ENTITY_REF) {
      type = RelationshipType::USES_P;
    } else if (left.first == STATEMENT_REF) {
      type = RelationshipType::USES_S;
    } else {
      throw runtime_error("Invalid fist param ref.");
    }

    getExactCharSymbol(',');

    // Parse right parameter
    // TODO(Beatrice): validate that entity is variable
    Param right = getEntRefParam();
    getExactCharSymbol(')');

    return {type, left.second, right};
  }

  if (relationship == "Modifies") {
    getExactCharSymbol('(');

    // Parse left parameter
    pair<RefType, Param> left = getEntOrStmtRefParam();
    RelationshipType type;
    if (left.first == ENTITY_REF) {
      type = RelationshipType::MODIFIES_P;
    } else if (left.first == STATEMENT_REF) {
      type = RelationshipType::MODIFIES_S;
    } else {
      throw runtime_error("Invalid fist param ref.");
    }

    getExactCharSymbol(',');

    // Parse right parameter
    // TODO(Beatrice): validate that entity is variable
    Param right = getEntRefParam();
    getExactCharSymbol(')');

    return {type, left.second, right};
  }

  throw runtime_error("Invalid param format.");
}

// ============ MAIN ============

QueryParser::QueryParser() = default;

tuple<SynonymMap, SelectClause> QueryParser::Parse(const string& query) {
  tokens = QueryLexer().Tokenize(query);
  it = tokens.begin();
  endIt = tokens.end();

  PrintDebugMessage("Query tokens:");
  for (const QueryToken& t : tokens) {
    PrintDebugMessage("[" + to_string(t.tokenType) + "," + t.value + "]");
  }

  synonymMap = parseSynonyms();
  SelectClause selectClause = parseSelectClause();

  return {synonymMap, selectClause};
}
