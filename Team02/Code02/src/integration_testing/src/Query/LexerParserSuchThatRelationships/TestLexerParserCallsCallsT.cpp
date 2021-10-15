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

// ====================== Testing Calls relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for Calls relationship "
    "succeeds") {
  SECTION("Valid Calls(\"x\", \"y\")") {
    string validQuery = "Select BOOLEAN such that Calls(\"x\", \"y\")";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::NAME_LITERAL, "x",
                                     query::ParamType::NAME_LITERAL, "y");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(\"x\", _)") {
    string validQuery = "Select BOOLEAN such that Calls(\"x\", _)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::NAME_LITERAL, "x",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(\"x\", p)") {
    string validQuery = "procedure p; Select BOOLEAN such that Calls(\"x\", p)";

    // expected
    SynonymMap map = {{"p", query::DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::NAME_LITERAL, "x",
                                     query::ParamType::SYNONYM, "p");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(_, \"y\")") {
    string validQuery = "Select BOOLEAN such that Calls(_, \"y\")";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::NAME_LITERAL, "y");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(_, _)") {
    string validQuery = "Select BOOLEAN such that Calls(_, _)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(_, p)") {
    string validQuery = "procedure p; Select BOOLEAN such that Calls(_, p)";

    // expected
    SynonymMap map = {{"p", query::DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::SYNONYM, "p");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(p1, \"y\")") {
    string validQuery =
        "procedure p1; Select BOOLEAN such that Calls(p1, \"y\")";

    // expected
    SynonymMap map = {{"p1", query::DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::SYNONYM, "p1",
                                     query::ParamType::NAME_LITERAL, "y");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(p1, _)") {
    string validQuery = "procedure p1; Select BOOLEAN such that Calls(p1, _)";

    // expected
    SynonymMap map = {{"p1", query::DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::SYNONYM, "p1",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Calls(p1, p2)") {
    string validQuery =
        "procedure p1, p2; Select BOOLEAN such that Calls(p1, p2)";

    // expected
    SynonymMap map = {{"p1", query::DesignEntity::PROCEDURE},
                      {"p2", query::DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS,
                                     query::ParamType::SYNONYM, "p1",
                                     query::ParamType::SYNONYM, "p2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid calls throws ") {
  SECTION("Invalid Call(p1, p2)") {
    string invalidQuery =
        "procedure p1, p2;"
        "Select p1 such that Call(p1, p2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_RELATIONSHIP_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Calls(1, p2)") {
    string invalidQuery =
        "procedure p1, p2;"
        "Select p1 such that Calls(1, p2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Calls(p1, \"\")") {
    string invalidQuery =
        "procedure p1, p2;"
        "Select p1 such that Calls(p1, \"\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_NAME_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Calls(p1, s") {
    string invalidQuery =
        "procedure p1, p2; stmt s;"
        "Select p1 such that Calls(p1, s)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_PROCEDURE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid boolean Calls(p1, s") {
    string invalidQuery =
        "procedure p1, p2; stmt s;"
        "Select BOOLEAN such that Calls(p1, s)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_PROCEDURE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }
}

// ====================== Testing Calls* relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for Calls* relationship "
    "succeeds") {
  SECTION("Valid Calls*(\"x\", \"y\")") {
    string validQuery = "Select BOOLEAN such that Calls*(\"x\", \"y\")";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::CALLS_T,
                                     query::ParamType::NAME_LITERAL, "x",
                                     query::ParamType::NAME_LITERAL, "y");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries for one such that clause for Calls* throws") {
  SECTION("Invalid Calls * (\"x\", \"y\")") {
    string invalidQuery = "Select BOOLEAN such that Calls * (\"x\", \"y\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}
