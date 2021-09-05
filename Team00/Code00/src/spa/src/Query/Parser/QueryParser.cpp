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
QueryToken QueryParser::consumeToken(tIterator& it, tIterator& endIt) {
  if (it == endIt) {
    throw runtime_error(
        "QueryParser expected another QueryToken but received None.");
  }
  QueryToken token = *it;
  ++it;
  return token;
}

bool QueryParser::hasNextToken(tIterator& it, tIterator& endIt) {
  return it != endIt;
}

optional<QueryToken> QueryParser::peekToken(tIterator& it, tIterator& endIt) {
  if (!hasNextToken(it, endIt)) {
    return nullopt;
  }
  return {*it};
}
// ============ Helpers (Bool checks) ============

bool QueryParser::isDesignEntity(const string& maybeEntity) {
  return keywordToDesignEntity.find(maybeEntity) != keywordToDesignEntity.end();
}

// ============ Helpers (Getters and validation for Token) ============
string QueryParser::getName(tIterator& it, tIterator& endIt) {
  QueryToken t = consumeToken(it, endIt);
  if (t.tokenType != TokenType::NAME) {
    throw runtime_error("Invalid format. QueryParser expected NAME token.");
  }
  return t.value;
}

char QueryParser::getCharSymbol(tIterator& it, tIterator& endIt) {
  QueryToken t = consumeToken(it, endIt);
  if (t.tokenType != TokenType::CHAR_SYMBOL) {
    throw runtime_error(
        "Invalid format. QueryParser expected CHAR_SYMBOL token.");
  }
  return t.value.at(0);
}

char QueryParser::getExactCharSymbol(tIterator& it, tIterator& endIt,
                                     const char& charMatch) {
  char charSymbol = getCharSymbol(it, endIt);
  if (charSymbol != charMatch) {
    throw runtime_error(
        "Invalid format. QueryParser expected a specific char token.");
  }
  return charSymbol;
}

DesignEntity QueryParser::getDesignEntity(tIterator& it, tIterator& endIt) {
  QueryToken t = consumeToken(it, endIt);
  string keyword = t.value;
  if (!isDesignEntity(keyword)) {
    throw runtime_error(
        "Invalid format. QueryParser expects a DesignEntity token.");
  }
  return keywordToDesignEntity.at(keyword);
}

string QueryParser::getKeyword(tIterator& it, tIterator& endIt) {
  QueryToken t = consumeToken(it, endIt);
  if (t.tokenType != TokenType::KEYWORD && t.tokenType != TokenType::NAME) {
    throw runtime_error("Invalid format. QueryParser expects a keyword token.");
  }
  return t.value;
}

string QueryParser::getExactKeyword(tIterator& it, tIterator& endIt,
                                    const string& keywordMatch) {
  string keyword = getKeyword(it, endIt);
  if (keyword != keywordMatch) {
    throw runtime_error(
        "Invalid format. QueryParser expects a specific keyword token.");
  }
  return keyword;
}

// ============ Helpers (Getters for DesignEntity) ============
DesignEntity QueryParser::getEntityFromSynonymName(
    const string& name, const SynonymMap& synonymMap) {
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
Param QueryParser::getStmtRefParam(tIterator& it, tIterator& endIt,
                                   const SynonymMap& synonymMap) {
  QueryToken t = consumeToken(it, endIt);
  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '_') {
    return {ParamType::WILDCARD, t.value};
  }

  if (t.tokenType == TokenType::INTEGER) {
    return {ParamType::LITERAL, t.value};
  }

  if (t.tokenType == TokenType::NAME) {
    // Get synonym name
    string synonymName = t.value;

    // Verify synonym is StmtRef type
    DesignEntity entity = getEntityFromSynonymName(synonymName, synonymMap);
    RefType refType = getRefTypeFromEntity(entity);

    if (refType == STATEMENT_REF) {
      return {ParamType::SYNONYM, synonymName};
    }
  }

  throw runtime_error("Invalid format. QueryParser expected a valid STMT_REF");
}

Param QueryParser::getEntRefParam(tIterator& it, tIterator& endIt,
                                  const SynonymMap& synonymMap) {
  QueryToken t = consumeToken(it, endIt);
  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '_') {
    return {ParamType::WILDCARD, t.value};
  }

  if (t.tokenType == TokenType::CHAR_SYMBOL && t.value.at(0) == '"') {
    string ident = getName(it, endIt);
    getExactCharSymbol(it, endIt, '"');

    return {ParamType::LITERAL, ident};
  }

  if (t.tokenType == TokenType::NAME) {
    // Get synonym name
    string synonymName = t.value;

    // Verify synonym is EntRef type
    DesignEntity entity = getEntityFromSynonymName(synonymName, synonymMap);
    RefType refType = getRefTypeFromEntity(entity);

    if (refType == ENTITY_REF) {
      return {ParamType::SYNONYM, synonymName};
    }
  }

  throw runtime_error("Invalid format. QueryParser expected a valid ENT_REF");
}

pair<RefType, Param> QueryParser::getEntOrStmtRefParam(
    tIterator& it, tIterator& endIt, const SynonymMap& synonymMap) {
  QueryToken t = consumeToken(it, endIt);
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
    string ident = getName(it, endIt);

    // Verify closing quotations
    getExactCharSymbol(it, endIt, '"');

    return {RefType::ENTITY_REF, {ParamType::LITERAL, ident}};
  }

  if (t.tokenType == TokenType::NAME) {
    // Get synonym name
    string synonymName = t.value;

    // Get type of synonym
    DesignEntity entity = getEntityFromSynonymName(synonymName, synonymMap);
    RefType refType = getRefTypeFromEntity(entity);

    return {refType, {ParamType::SYNONYM, synonymName}};
  }

  throw runtime_error(
      "Invalid format. QueryParser expected a valid ENT_REF or STMT_REF");
}

// ============ Parsers ============
SynonymMap QueryParser::parseSynonyms(tIterator& it, tIterator& endIt) {
  SynonymMap synonyms;

  bool hasDeclaration = true;
  while (hasDeclaration) {
    // Parse design entity and synonym name
    DesignEntity entity = getDesignEntity(it, endIt);
    string name = getName(it, endIt);

    // Populate synonym map
    synonyms[name] = entity;

    // Parse delimiter symbol
    char symbol = getCharSymbol(it, endIt);

    // Parse any extra synonym names for the same design entity
    bool hasSynonymName = symbol == ',';
    while (hasSynonymName) {
      // Parse next synonym name for the same design entity
      string nextName = getName(it, endIt);

      // Populate synonym map
      synonyms[nextName] = entity;

      // Parse and update delimiter symbol
      symbol = getCharSymbol(it, endIt);

      hasSynonymName = symbol == ',';
    }

    if (symbol != ';') {
      throw runtime_error("Invalid format. QueryParser expected ; token");
    }

    optional<QueryToken> maybeToken = peekToken(it, endIt);
    hasDeclaration =
        maybeToken.has_value() && isDesignEntity(maybeToken.value().value);
  }
  return synonyms;
}

SelectClause QueryParser::parseSelectClause(tIterator& it, tIterator& endIt,
                                            const SynonymMap& synonymMap) {
  // Verify select keyword is present
  getExactKeyword(it, endIt, "Select");

  // Parse select selectSynonym
  string sName = getName(it, endIt);
  DesignEntity designEntity = getEntityFromSynonymName(sName, synonymMap);
  Synonym selectSynonym = {designEntity, sName};

  // Parse clauses
  vector<SuchThatClause> suchThatClauses;
  vector<PatternClause> patternClauses;

  bool hasNext = hasNextToken(it, endIt);

  while (hasNext) {
    string clauseKeyword = getKeyword(it, endIt);

    if (clauseKeyword == "such") {
      string nextKeyword = getExactKeyword(it, endIt, "that");
      SuchThatClause suchThatClause =
          parseSuchThatClause(it, endIt, synonymMap);
      suchThatClauses.push_back(suchThatClause);

    } else if (clauseKeyword == "pattern") {
      continue;  // TODO(Beatrice): Implement parser for pattern clause

    } else {
      throw runtime_error(
          "Invalid format. QueryParser expects either SUCH_THAT_KEYWORDS"
          " or PATTERN_KEYWORD token");
    }

    hasNext = hasNextToken(it, endIt);
  }

  return {selectSynonym, suchThatClauses, patternClauses};
}

SuchThatClause QueryParser::parseSuchThatClause(tIterator& it, tIterator& endIt,
                                                const SynonymMap& synonymMap) {
  string relationship = getKeyword(it, endIt);
  if (keywordToStmtRefRs.find(relationship) != keywordToStmtRefRs.end()) {
    RelationshipType type = keywordToStmtRefRs.at(relationship);

    getExactCharSymbol(it, endIt, '(');
    Param left = getStmtRefParam(it, endIt, synonymMap);

    getExactCharSymbol(it, endIt, ',');
    Param right = getStmtRefParam(it, endIt, synonymMap);

    getExactCharSymbol(it, endIt, ')');

    return {type, left, right};
  }

  if (relationship == "Uses") {
    getExactCharSymbol(it, endIt, '(');

    // Parse left parameter
    pair<RefType, Param> left = getEntOrStmtRefParam(it, endIt, synonymMap);
    RelationshipType type;
    if (left.first == ENTITY_REF) {
      type = RelationshipType::USES_P;
    } else if (left.first == STATEMENT_REF) {
      type = RelationshipType::USES_S;
    } else {
      throw runtime_error("Invalid fist param ref.");
    }

    getExactCharSymbol(it, endIt, ',');

    // Parse right parameter
    // TODO(Beatrice): validate that entity is variable
    Param right = getEntRefParam(it, endIt, synonymMap);
    getExactCharSymbol(it, endIt, ')');

    return {type, left.second, right};
  }

  if (relationship == "Modifies") {
    getExactCharSymbol(it, endIt, '(');

    // Parse left parameter
    pair<RefType, Param> left = getEntOrStmtRefParam(it, endIt, synonymMap);
    RelationshipType type;
    if (left.first == ENTITY_REF) {
      type = RelationshipType::MODIFIES_P;
    } else if (left.first == STATEMENT_REF) {
      type = RelationshipType::MODIFIES_S;
    } else {
      throw runtime_error("Invalid fist param ref.");
    }

    getExactCharSymbol(it, endIt, ',');

    // Parse right parameter
    // TODO(Beatrice): validate that entity is variable
    Param right = getEntRefParam(it, endIt, synonymMap);
    getExactCharSymbol(it, endIt, ')');

    return {type, left.second, right};
  }

  throw runtime_error("Invalid param format.");
}

// ============ MAIN ============
tuple<SynonymMap, SelectClause> QueryParser::ParseQuery(const string& query) {
  vector<QueryToken> tokens = QueryLexer::Tokenize(query);

  PrintDebugMessage("Query tokens:");
  for (const QueryToken& t : tokens) {
    PrintDebugMessage("[" + to_string(t.tokenType) + "," + t.value + "]");
  }

  auto it = tokens.begin();
  auto endIt = tokens.end();

  SynonymMap synonyms = parseSynonyms(it, endIt);
  SelectClause selectClause = parseSelectClause(it, endIt, synonyms);

  return {synonyms, selectClause};
}
