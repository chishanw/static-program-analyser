#include <Query/Parser/QueryParser.h>
#include <Query/Parser/QueryToken.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ====================== Testing pattern relationship ======================
TEST_CASE("Valid queries with one pattern clause for assignment succeeds") {
  SECTION("Valid pattern a(_, _)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    query::MatchType::ANY, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(_, _\"x\"_)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _\"x\"_)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    query::MatchType::SUB_EXPRESSION, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", _)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::NAME_LITERAL, "x",
                                    query::MatchType::ANY, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", _\"x\"_)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", _\"x\"_)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::NAME_LITERAL, "x",
                                    query::MatchType::SUB_EXPRESSION, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(v, _)") {
    string validQuery =
        "assign a; variable v;"
        "Select a pattern a(v, _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN},
                      {"v", query::DesignEntity::VARIABLE}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::SYNONYM, "v",
                                    query::MatchType::ANY, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(v, _\"x\"_)") {
    string validQuery =
        "assign a; variable v;"
        "Select a pattern a(v, _\"x\"_)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN},
                      {"v", query::DesignEntity::VARIABLE}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::SYNONYM, "v",
                                    query::MatchType::SUB_EXPRESSION, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid EXACT pattern_expr throws") {
  SECTION("Invalid pattern a(_, \"x\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, \"x\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(_, \"1\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, \"1\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(v, \"x\")") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"x\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(v, \"1\")") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"1\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(\"x\", \"x\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"x\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(\"x\", \"1\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"1\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }
}

TEST_CASE("Invalid queries with one pattern clause for assignment throws") {
  SECTION("Invalid pattern s(_, _)") {
    string invalidQuery =
        "stmt s;"
        "Select s pattern s(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_ASSIGN_MSG);
  }

  SECTION("Invalid pattern a(1, _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(1, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
  }

  SECTION("Invalid pattern a(s, _)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
  }

  SECTION("Invalid pattern a(s, _\"x\")") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, _\"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(s, \"x\"_)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, \"x\"_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(s, \"x_)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, \"x_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(s, x)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, x)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(_, _") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, _";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_INSUFFICIENT_TOKENS_MSG);
  }

  SECTION("Invalid pattern a(_ _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_ _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid pattern a(_)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }
}
