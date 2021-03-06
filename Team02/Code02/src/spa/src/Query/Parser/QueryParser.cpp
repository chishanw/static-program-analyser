#include "QueryParser.h"

#include <Common/Common.h>
#include <Common/ExprParser.h>
#include <Common/Tokenizer.h>

#include <set>
#include <unordered_set>

using namespace std;
using namespace query;
using namespace qpp;

// ============ Map definitions ============
const unordered_map<string, RelationshipType> keywordToFollowsParentType = {
    {"Follows", RelationshipType::FOLLOWS},
    {"Follows*", RelationshipType::FOLLOWS_T},
    {"Parent", RelationshipType::PARENT},
    {"Parent*", RelationshipType::PARENT_T}};

const unordered_map<string, RelationshipType> keywordToCallsType = {
    {"Calls", RelationshipType::CALLS}, {"Calls*", RelationshipType::CALLS_T}};

const unordered_map<string, RelationshipType> keywordToNextType = {
    {"Next", RelationshipType::NEXT},
    {"Next*", RelationshipType::NEXT_T},
    {"NextBip", RelationshipType::NEXT_BIP},
    {"NextBip*", RelationshipType::NEXT_BIP_T},
};

const unordered_map<string, RelationshipType> keywordToAffectsType = {
    {"Affects", RelationshipType::AFFECTS},
    {"Affects*", RelationshipType::AFFECTS_T},
    {"AffectsBip", RelationshipType::AFFECTS_BIP},
};

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

const unordered_map<string, Attribute> keywordToAttribute = {
    {"procName", Attribute::PROC_NAME},
    {"varName", Attribute::VAR_NAME},
    {"value", Attribute::VALUE},
    {"stmt#", Attribute::STMT_NUM}};

const set<DesignEntity> validStmtRefEntities = {
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

const set<DesignEntity> validAffectsParamEntities = {
    DesignEntity::STATEMENT, DesignEntity::PROG_LINE, DesignEntity::ASSIGN};

const set<ParamType> integerParamTypes = {
    ParamType::INTEGER_LITERAL, ParamType::ATTRIBUTE_STMT_NUM,
    ParamType::ATTRIBUTE_VALUE, ParamType::SYNONYM};

const set<ParamType> stringParamTypes = {ParamType::NAME_LITERAL,
                                         ParamType::ATTRIBUTE_VAR_NAME,
                                         ParamType::ATTRIBUTE_PROC_NAME};

const set<ParamType> validStmtRefParamTypes = {
    ParamType::INTEGER_LITERAL, ParamType::SYNONYM, ParamType::WILDCARD};

const set<ParamType> validEntRefParamTypes = {
    ParamType::NAME_LITERAL, ParamType::SYNONYM, ParamType::WILDCARD};

// ============ Helpers (Token) ============
QueryToken QueryParser::consumeToken() {
  if (it == endIt) {
    throw SyntacticErrorException(INVALID_INSUFFICIENT_TOKENS_MSG);
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
string QueryParser::getNameOrKeyword() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::NAME_OR_KEYWORD) {
    throw SyntacticErrorException(INVALID_NAME_MSG);
  }
  return t.value;
}

char QueryParser::getCharSymbol() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::CHAR_SYMBOL) {
    throw SyntacticErrorException(INVALID_CHAR_SYMBOL_MSG);
  }
  return t.value.at(0);
}

char QueryParser::getExactCharSymbol(const char& charMatch) {
  char charSymbol = getCharSymbol();
  if (charSymbol != charMatch) {
    throw SyntacticErrorException(INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }
  return charSymbol;
}

DesignEntity QueryParser::getDesignEntity() {
  QueryToken t = consumeToken();
  string keyword = t.value;
  if (!isDesignEntity(keyword)) {
    throw SyntacticErrorException(INVALID_DESIGN_ENTITY_MSG);
  }
  return keywordToDesignEntity.at(keyword);
}

string QueryParser::getKeyword() {
  QueryToken t = consumeToken();
  if (t.tokenType != TokenType::KEYWORD &&
      t.tokenType != TokenType::NAME_OR_KEYWORD) {
    throw SyntacticErrorException(INVALID_KEYWORD_MSG);
  }
  return t.value;
}

string QueryParser::getExactKeyword(const string& keywordMatch) {
  string keyword = getKeyword();
  if (keyword != keywordMatch) {
    throw SyntacticErrorException(INVALID_SPECIFIC_KEYWORD_MSG);
  }
  return keyword;
}

// ============ Helpers (Getters for DesignEntity) ============
DesignEntity QueryParser::getEntityFromSynonymName(const string& name) {
  if (synonymMap.find(name) == synonymMap.end()) {
    isSemanticallyValid = false;  // semantic error
    semanticErrorMsg = INVALID_UNDECLARED_SYNONYM_MSG;
    return {};
  }
  return synonymMap.at(name);
}

// ============ Helpers (Getters for Attribute) ============
query::Attribute QueryParser::getAttribute(const std::string& synonymName) {
  DesignEntity entity = getEntityFromSynonymName(synonymName);
  string attrName = getKeyword();

  if (keywordToAttribute.find(attrName) == keywordToAttribute.end()) {
    throw SyntacticErrorException(INVALID_ATTRIBUTE);
  }
  Attribute attribute = keywordToAttribute.at(attrName);

  // validate attribute against synonym type
  switch (attribute) {
    case Attribute::PROC_NAME:
      if (entity == DesignEntity::PROCEDURE || entity == DesignEntity::CALL) {
        return attribute;
      }
    case Attribute::VAR_NAME:
      if (entity == DesignEntity::VARIABLE || entity == DesignEntity::READ ||
          entity == DesignEntity::PRINT) {
        return attribute;
      }
    case Attribute::STMT_NUM:
      if (entity == DesignEntity::STATEMENT || entity == DesignEntity::READ ||
          entity == DesignEntity::PRINT || entity == DesignEntity::CALL ||
          entity == DesignEntity::WHILE || entity == DesignEntity::IF ||
          entity == DesignEntity::ASSIGN) {
        return attribute;
      }
    case Attribute::VALUE:
      if (entity == DesignEntity::CONSTANT) {
        return attribute;
      }
    default:
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_SYNONYM_ATTRIBUTE_MATCH;
  }

  return attribute;
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
    string ident = getNameOrKeyword();
    // Check if closing quotations is present
    getExactCharSymbol('"');
    return {ParamType::NAME_LITERAL, ident};
  }

  if (t.tokenType == TokenType::NAME_OR_KEYWORD) {
    string synonymName = t.value;

    // validate that synonym had been declared
    getEntityFromSynonymName(synonymName);

    if (!(peekToken().has_value() &&
          peekToken().value().tokenType == TokenType::CHAR_SYMBOL &&
          peekToken().value().value == ".")) {
      return {ParamType::SYNONYM, synonymName};
    }

    // synonym has an attribute
    consumeToken();  // consume '.'
    Attribute attribute = getAttribute(synonymName);

    switch (attribute) {
      case Attribute::PROC_NAME:
        return {ParamType::ATTRIBUTE_PROC_NAME, synonymName};
      case Attribute::VAR_NAME:
        return {ParamType::ATTRIBUTE_VAR_NAME, synonymName};
      case Attribute::VALUE:
        return {ParamType::ATTRIBUTE_VALUE, synonymName};
      case Attribute::STMT_NUM:
        return {ParamType::ATTRIBUTE_STMT_NUM, synonymName};
      default:
        isSemanticallyValid = false;  // semantic error
        semanticErrorMsg = INVALID_SYNONYM_ATTRIBUTE_MATCH;
    }
  }

  throw SyntacticErrorException(INVALID_STMT_ENT_REF_MSG);
}

// ============ Parsers (Main) ============
SynonymMap QueryParser::parseSynonymDeclarations() {
  SynonymMap synonyms;

  optional<QueryToken> maybeToken = peekToken();
  bool hasDeclaration =
      maybeToken.has_value() && isDesignEntity(maybeToken.value().value);
  while (hasDeclaration) {
    // Parse design entity and synonym name
    DesignEntity entity = getDesignEntity();
    string name = getNameOrKeyword();

    // if synonym name already exists
    if (synonyms.find(name) != synonyms.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_DUPLICATE_SYNONYM_MSG;
    } else {
      synonyms[name] = entity;
    }
    char symbol = getCharSymbol();

    // Parse any extra synonym names for the same design entity
    bool hasSynonymName = symbol == ',';
    while (hasSynonymName) {
      // Parse next synonym name for the same design entity
      string nextName = getNameOrKeyword();
      synonyms[nextName] = entity;
      symbol = getCharSymbol();
      hasSynonymName = symbol == ',';
    }

    if (symbol != ';') {
      throw SyntacticErrorException(INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
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

  // Parse result clause
  tuple<vector<Synonym>, SelectType> resultClause = parseResultClause();

  // Parse condition clauses
  vector<ConditionClause> conditionClauses;

  while (hasNextToken()) {
    optional<QueryToken> nextToken = peekToken();
    string keyword = nextToken.value().value;

    if (keyword == "such") {
      parseSuchThatClause(conditionClauses);

    } else if (keyword == "pattern") {
      parsePatternClause(conditionClauses);

    } else if (keyword == "with") {
      parseWithClause(conditionClauses);

    } else {
      throw SyntacticErrorException(INVALID_ST_P_KEYWORD_MSG);
    }
  }

  return {get<0>(resultClause), get<1>(resultClause), conditionClauses};
}

tuple<vector<Synonym>, SelectType> QueryParser::parseResultClause() {
  SelectType selectType;
  vector<Synonym> selectSynonyms = {};

  QueryToken resultToken = consumeToken();
  if (resultToken.tokenType == TokenType::CHAR_SYMBOL &&
      resultToken.value == "<") {
    selectType = SelectType::SYNONYMS;
    bool hasSynonym = true;
    while (hasSynonym) {
      string synonymName = getNameOrKeyword();
      DesignEntity entity = getEntityFromSynonymName(synonymName);

      bool hasAttribute = false;
      Attribute attribute = {};

      if (peekToken().has_value() &&
          peekToken().value().tokenType == TokenType::CHAR_SYMBOL &&
          peekToken().value().value == ".") {
        // synonym has an attribute
        consumeToken();  // consume '.'
        hasAttribute = true;
        attribute = getAttribute(synonymName);
      }

      selectSynonyms.push_back({entity, synonymName, hasAttribute, attribute});

      hasSynonym = peekToken().has_value() && peekToken().value().value == ",";
      if (hasSynonym) {
        getExactCharSymbol(',');
      }
    }
    getExactCharSymbol('>');

  } else if (resultToken.tokenType == TokenType::NAME_OR_KEYWORD) {
    selectType = SelectType::SYNONYMS;
    string name = resultToken.value;
    if ((synonymMap.find(name) == synonymMap.end()) && name == "BOOLEAN") {
      return {selectSynonyms, SelectType::BOOLEAN};
    }

    DesignEntity entity = getEntityFromSynonymName(name);

    bool hasAttribute = false;
    Attribute attribute = {};

    if (peekToken().has_value() &&
        peekToken().value().tokenType == TokenType::CHAR_SYMBOL &&
        peekToken().value().value == ".") {
      // synonym has an attribute
      consumeToken();  // consume '.'
      hasAttribute = true;
      attribute = getAttribute(name);
    }

    selectSynonyms.push_back({entity, name, hasAttribute, attribute});
  } else {
    throw SyntacticErrorException(INVALID_RESULT_TYPE_MSG);
  }

  return {selectSynonyms, selectType};
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

    } else if (keywordToCallsType.find(relationship) !=
               keywordToCallsType.end()) {
      results.push_back(parseCallsClause(relationship));

    } else if (keywordToNextType.find(relationship) !=
               keywordToNextType.end()) {
      results.push_back(parseNextClause(relationship));

    } else if (keywordToAffectsType.find(relationship) !=
               keywordToAffectsType.end()) {
      results.push_back(parseAffectsClause(relationship));

    } else if (relationship == "Uses") {
      results.push_back(parseUsesClause());

    } else if (relationship == "Modifies") {
      results.push_back(parseModifiesClause());

    } else {
      throw SyntacticErrorException(INVALID_ST_RELATIONSHIP_MSG);
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
  if (validStmtRefParamTypes.find(left.type) == validStmtRefParamTypes.end() ||
      validStmtRefParamTypes.find(right.type) == validStmtRefParamTypes.end()) {
    throw SyntacticErrorException(INVALID_STMT_REF_MSG);
  }

  if (left.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(left.value);
    if (validStmtRefEntities.find(entity) == validStmtRefEntities.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_SYNONYM_MSG;
    }
  }

  if (right.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(right.value);
    if (validStmtRefEntities.find(entity) == validStmtRefEntities.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_SYNONYM_MSG;
    }
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parseCallsClause(
    const string& relationship) {
  RelationshipType type = keywordToCallsType.at(relationship);

  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  // Validate parameters
  if (validEntRefParamTypes.find(left.type) == validEntRefParamTypes.end() ||
      validEntRefParamTypes.find(right.type) == validEntRefParamTypes.end()) {
    throw SyntacticErrorException(INVALID_ENT_REF_MSG);
  }

  if (left.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(left.value);
    if (entity != DesignEntity::PROCEDURE) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_SYNONYM_NON_PROCEDURE_MSG;
    }
  }

  if (right.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(right.value);
    if (entity != DesignEntity::PROCEDURE) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_SYNONYM_NON_PROCEDURE_MSG;
    }
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parseNextClause(
    const string& relationship) {
  RelationshipType type = keywordToNextType.at(relationship);

  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  // Validate parameters
  if (validStmtRefParamTypes.find(left.type) == validStmtRefParamTypes.end() ||
      validStmtRefParamTypes.find(right.type) == validStmtRefParamTypes.end()) {
    throw SyntacticErrorException(INVALID_LINE_REF_MSG);
  }

  if (left.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(left.value);
    if (validStmtRefEntities.find(entity) == validStmtRefEntities.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_STMT_REF_MSG;
    }
  }

  if (right.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(right.value);
    if (validStmtRefEntities.find(entity) == validStmtRefEntities.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_STMT_REF_MSG;
    }
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parseAffectsClause(
    const string& relationship) {
  RelationshipType type = keywordToAffectsType.at(relationship);

  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  // Validate parameters
  if (validStmtRefParamTypes.find(left.type) == validStmtRefParamTypes.end() ||
      validStmtRefParamTypes.find(right.type) == validStmtRefParamTypes.end()) {
    throw SyntacticErrorException(INVALID_STMT_REF_MSG);
  }

  if (left.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(left.value);
    if (validAffectsParamEntities.find(entity) ==
        validAffectsParamEntities.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_ST_AFFECTS_MSG;
    }
  }

  if (right.type == ParamType::SYNONYM) {
    DesignEntity entity = getEntityFromSynonymName(right.value);
    if (validAffectsParamEntities.find(entity) ==
        validAffectsParamEntities.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_ST_AFFECTS_MSG;
    }
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parseUsesClause() {
  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  RelationshipType type = {};
  // Identify relationship type and validate left param
  switch (left.type) {
    case ParamType::WILDCARD: {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_ST_USES_MODIFIES_WILDCARD_MSG;
      break;
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
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_ST_USES_SYNONYM_ENTITY_MSG;
      break;
    }
    default:
      throw SyntacticErrorException(INVALID_STMT_ENT_REF_MSG);
  }

  // Validate right param
  if (validEntRefParamTypes.find(right.type) == validEntRefParamTypes.end()) {
    throw SyntacticErrorException(INVALID_ENT_REF_MSG);
  }

  if (right.type == ParamType::SYNONYM &&
      getEntityFromSynonymName(right.value) != DesignEntity::VARIABLE) {
    isSemanticallyValid = false;  // semantic error
    semanticErrorMsg = INVALID_SYNONYM_NON_VARIABLE_MSG;
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, {}, ConditionClauseType::SUCH_THAT};
}

query::ConditionClause QueryParser::parseModifiesClause() {
  getExactCharSymbol('(');
  Param left = getRefParam();
  getExactCharSymbol(',');
  Param right = getRefParam();
  getExactCharSymbol(')');

  RelationshipType type = {};
  // Identify relationship type and validate left param
  switch (left.type) {
    case ParamType::WILDCARD: {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_ST_USES_MODIFIES_WILDCARD_MSG;
      break;
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
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_ST_MODIFIES_SYNONYM_ENTITY_MSG;
      break;
    }
    default:
      throw SyntacticErrorException(INVALID_STMT_ENT_REF_MSG);
  }

  // Validate right param
  if (validEntRefParamTypes.find(right.type) == validEntRefParamTypes.end()) {
    throw SyntacticErrorException(INVALID_ENT_REF_MSG);
  }

  if (right.type == ParamType::SYNONYM &&
      getEntityFromSynonymName(right.value) != DesignEntity::VARIABLE) {
    isSemanticallyValid = false;  // semantic error
    semanticErrorMsg = INVALID_SYNONYM_NON_VARIABLE_MSG;
  }

  SuchThatClause stClause = {type, left, right};
  return {stClause, {}, {}, ConditionClauseType::SUCH_THAT};
}

// ============ Parsers (pattern) ============
void QueryParser::parsePatternClause(vector<ConditionClause>& results) {
  getExactKeyword("pattern");

  bool hasNextPattern = true;
  while (hasNextPattern) {
    string pattSynString = getNameOrKeyword();
    DesignEntity pattSynEntity = getEntityFromSynonymName(pattSynString);
    Synonym pattSynonym = {pattSynEntity, pattSynString, false, {}};

    unordered_set<DesignEntity> expectedEntities = {};

    // parse left param
    getExactCharSymbol('(');
    Param left = getRefParam();
    getExactCharSymbol(',');

    // validate left param
    if (validEntRefParamTypes.find(left.type) == validEntRefParamTypes.end()) {
      throw SyntacticErrorException(INVALID_ENT_REF_MSG);
    }
    if (left.type == ParamType::SYNONYM &&
        getEntityFromSynonymName(left.value) != DesignEntity::VARIABLE) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_SYNONYM_NON_VARIABLE_MSG;
    }

    // parse right param and store clause
    results.push_back(
        parsePatternClauseHelper(pattSynonym, left, expectedEntities));

    // validate synonym type
    if (expectedEntities.find(pattSynEntity) == expectedEntities.end()) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_SYNONYM_MSG;
    }

    // checks if there are multiple patterns connected by 'and'
    hasNextPattern = hasNextToken() && peekToken().value().value == "and";
    if (hasNextPattern) {
      getExactKeyword("and");
    }
  }
}

query::ConditionClause QueryParser::parsePatternClauseHelper(
    Synonym& synonym, Param& leftParam,
    unordered_set<DesignEntity>& expectedEntities) {
  // right param is ,"x+y")
  if (hasNextToken() && peekToken().value().value == "\"") {
    expectedEntities = {DesignEntity::ASSIGN};
    PatternExpr expr = parseAssignExpr(MatchType::EXACT);
    PatternClause pClause = {synonym, leftParam, expr};
    return {{}, pClause, {}, ConditionClauseType::PATTERN};
  }

  // right param is ,_
  getExactCharSymbol('_');

  // right param is ,_ "x+y"_)
  if (hasNextToken() && peekToken().value().value == "\"") {
    expectedEntities = {DesignEntity::ASSIGN};
    PatternExpr expr = parseAssignExpr(MatchType::SUB_EXPRESSION);
    PatternClause pClause = {synonym, leftParam, expr};
    return {{}, pClause, {}, ConditionClauseType::PATTERN};
  }

  // right param is ,_ ,_)
  if (hasNextToken() && peekToken().value().value == ",") {
    expectedEntities = {DesignEntity::IF};
    getExactCharSymbol(',');
    getExactCharSymbol('_');
    getExactCharSymbol(')');
    PatternClause pClause = {synonym, leftParam, {}};
    return {{}, pClause, {}, ConditionClauseType::PATTERN};
  }

  // right param is ,_ )
  getExactCharSymbol(')');
  expectedEntities = {DesignEntity::WHILE, DesignEntity::ASSIGN};
  if (synonym.entity == DesignEntity::ASSIGN) {
    PatternClause pClause = {synonym, leftParam, {MatchType::ANY, "_"}};
    return {{}, pClause, {}, ConditionClauseType::PATTERN};
  }

  PatternClause pClause = {synonym, leftParam, {}};
  return {{}, pClause, {}, ConditionClauseType::PATTERN};
}

query::PatternExpr QueryParser::parseAssignExpr(MatchType type) {
  getExactCharSymbol('"');

  string exprString;
  bool hasExprTokens = true;
  while (hasExprTokens) {
    QueryToken current = consumeToken();
    exprString += current.value;
    hasExprTokens = hasNextToken() && peekToken().value().value != "\"";
  }

  getExactCharSymbol('"');
  if (type == MatchType::SUB_EXPRESSION) {
    getExactCharSymbol('_');
  }
  getExactCharSymbol(')');

  // convert expression tokens into exprString
  try {
    vector<string> exprTokens = Tokenizer::TokenizeProgramString(exprString);
    auto exprTokensIt = exprTokens.begin();
    ArithAST* exprAST = ExprParser().Parse(&exprTokensIt, exprTokens.end());
    string parsedExprString = exprAST->GetFullExprPatternStr();

    return {type, parsedExprString};
  } catch (exception e) {
    throw SyntacticErrorException(INVALID_P_EXPR_CHARA_MSG);
  }
}

// ============ Parsers (with) ============
void QueryParser::parseWithClause(
    std::vector<query::ConditionClause>& results) {
  getExactKeyword("with");

  bool hasNextWith = true;
  while (hasNextWith) {
    Param left = getRefParam();
    getExactCharSymbol('=');
    Param right = getRefParam();

    // validate synonym params
    if (left.type == ParamType::SYNONYM &&
        getEntityFromSynonymName(left.value) != DesignEntity::PROG_LINE) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_W_SYNONYM;
    }

    if (right.type == ParamType::SYNONYM &&
        getEntityFromSynonymName(right.value) != DesignEntity::PROG_LINE) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_W_SYNONYM;
    }

    // validate type of params
    if (left.type == ParamType::WILDCARD || right.type == ParamType::WILDCARD) {
      throw SyntacticErrorException(INVALID_W_PARAM);
    }

    bool areBothIntegers =
        (integerParamTypes.find(left.type) != integerParamTypes.end()) &&
        (integerParamTypes.find(right.type) != integerParamTypes.end());
    bool areBothStrings =
        (stringParamTypes.find(left.type) != stringParamTypes.end()) &&
        (stringParamTypes.find(right.type) != stringParamTypes.end());

    if (!(areBothIntegers || areBothStrings)) {
      isSemanticallyValid = false;  // semantic error
      semanticErrorMsg = INVALID_W_DIFF_PARAM_TYPES;
    }

    WithClause withClause = {left, right};
    results.push_back({{}, {}, withClause, ConditionClauseType::WITH});

    // checks if there are multiple with clauses connected by 'and'
    hasNextWith = hasNextToken() && peekToken().value().value == "and";
    if (hasNextWith) {
      getExactKeyword("and");
    }
  }
}

// ============ MAIN ============

QueryParser::QueryParser() = default;

tuple<SynonymMap, SelectClause> QueryParser::Parse(const string& query) {
  tuple<vector<QueryToken>, bool, string> tokenizedQuery =
      QueryLexer().Tokenize(query);
  vector<QueryToken> tokens = get<0>(tokenizedQuery);
  isSemanticallyValid = get<1>(tokenizedQuery);
  semanticErrorMsg = get<2>(tokenizedQuery);

  it = tokens.begin();
  endIt = tokens.end();

  synonymMap = parseSynonymDeclarations();
  SelectClause selectClause = parseSelectClause();

  // if SelectClause was parsed successfully but there is a semantic error
  if (!isSemanticallyValid && selectClause.selectType == SelectType::SYNONYMS) {
    throw SemanticSynonymErrorException(semanticErrorMsg);
  }

  if (!isSemanticallyValid && selectClause.selectType == SelectType::BOOLEAN) {
    throw SemanticBooleanErrorException(semanticErrorMsg);
  }

  return {synonymMap, selectClause};
}
