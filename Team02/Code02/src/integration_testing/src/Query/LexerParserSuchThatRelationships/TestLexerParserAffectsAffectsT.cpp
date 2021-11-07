#include <Common/Common.h>
#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../TestQueryUtil.h"
#include "catch.hpp"

using namespace std;
using namespace query;

// ====================== Testing Affects relationship ======================
TEST_CASE("Valid queries for Affects relationship succeeds") {
  SECTION("Valid Affects(1, 2)") {
    string validQuery = "Select BOOLEAN such that Affects(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Affects(1, _)") {
    string validQuery = "Select BOOLEAN such that Affects(1, _)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Affects(1, s2)") {
    string validQuery = "stmt s2; Select BOOLEAN such that Affects(1, s2)";

    // expected
    SynonymMap map = {{"s2", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::SYNONYM, "s2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Affects(1, n)") {
    string validQuery = "prog_line n; Select BOOLEAN such that Affects(1, n)";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::SYNONYM, "n");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Affects(1, a)") {
    string validQuery = "assign a; Select BOOLEAN such that Affects(1, a)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::SYNONYM, "a");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Affects(s1, a)") {
    string validQuery =
        "stmt s1; assign a; Select BOOLEAN such that Affects(s1, a)";

    // expected
    SynonymMap map = {{"s1", DesignEntity::STATEMENT},
                      {"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS,
                                     query::ParamType::SYNONYM, "s1",
                                     query::ParamType::SYNONYM, "a");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid Affects throws") {
  SECTION("Invalid Affects(s.stmt#, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Affects(s.stmt#, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Affects(1, v.varName)") {
    string invalidQuery =
        "variable v;"
        "Select v.varName such that Affects(1, v.varName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Affects(1, c.procName)") {
    string invalidQuery =
        "call c;"
        "Select v.varName such that Affects(1, c.procName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Affects(1, con.value)") {
    string invalidQuery =
        "constant con;"
        "Select BOOLEAN such that Affects(1, con.value)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Affects(s.stmt#, con.value)") {
    string invalidQuery =
        "constant con; stmt s;"
        "Select BOOLEAN such that Affects(s.stmt#, con.value)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid boolean Affects(1, \"hello\")") {
    string invalidQuery = "Select BOOLEAN such that Affects(1, \"hello\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Affects(1, p)") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Affects(1, p)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Affects(1, con)") {
    string invalidQuery =
        "constant con;"
        "Select con such that Affects(1, con)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Affects(1, v)") {
    string invalidQuery =
        "variable v;"
        "Select BOOLEAN such that Affects(1, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid Affects(1, r)") {
    string invalidQuery =
        "read r;"
        "Select BOOLEAN such that Affects(1, r)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid Affects(1, pri)") {
    string invalidQuery =
        "print pri;"
        "Select BOOLEAN such that Affects(1, pri)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid Affects(1, ca)") {
    string invalidQuery =
        "call ca;"
        "Select BOOLEAN such that Affects(1, ca)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid Affects(1, w)") {
    string invalidQuery =
        "while w;"
        "Select BOOLEAN such that Affects(1, w)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid Affects(1, ifs)") {
    string invalidQuery =
        "if ifs;"
        "Select BOOLEAN such that Affects(1, ifs)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_AFFECTS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }
}

// ====================== Testing Affects* relationship ======================
TEST_CASE("Valid queries for Affects* relationship succeeds") {
  SECTION("Valid Affects*(1, 2)") {
    string validQuery = "Select BOOLEAN such that Affects*(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS_T,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

// ====================== Testing AffectsBip relationship ======================
TEST_CASE("Valid queries for AffectsBip relationship succeeds") {
  SECTION("Valid AffectsBip(1, 2)") {
    string validQuery = "Select BOOLEAN such that AffectsBip(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::AFFECTS_BIP,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

// =============== Testing AffectsBip* relationship ===============
TEST_CASE("Invalid queries for AffectsBip* relationship throws") {
  SECTION("Invalid AffectsBip*(1, 2)") {
    string invalidQuery = "Select BOOLEAN such that AffectsBip*(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_RELATIONSHIP_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}
