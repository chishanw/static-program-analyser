#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ====================== Testing Parent relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for Parent relationship "
    "succeeds") {
  SECTION("Valid Parent(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Parent(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::PARENT,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

// ====================== Testing Parent* relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for Parent* relationship "
    "succeeds") {
  SECTION("Valid Parent*(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Parent*(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::PARENT_T,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries for one such that clause for Parent* throws") {
  SECTION("Invalid Parent *(1, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Parent *(\"x\", 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}
