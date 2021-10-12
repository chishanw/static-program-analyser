#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ================ Testing pattern assignment relationship ================
TEST_CASE("Valid queries with one pattern clause for assignment succeeds") {
  SECTION("Valid pattern a(_, _)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::SUB_EXPRESSION, "x"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::NAME_LITERAL, "x",
                                    {query::MatchType::ANY, "_"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::NAME_LITERAL, "x",
                                    {query::MatchType::SUB_EXPRESSION, "x"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::SYNONYM, "v",
                                    {query::MatchType::ANY, "_"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::SYNONYM, "v",
                                    {query::MatchType::SUB_EXPRESSION, "x"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(_, \"1\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, \"1\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(v, \"x\")") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"x\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(v, \"1\")") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"1\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(\"x\", \"x\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"x\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(\"x\", \"1\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"1\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}

TEST_CASE("Invalid queries with one pattern clause for assignment throws") {
  SECTION("Invalid pattern s(_, _)") {
    string invalidQuery =
        "stmt s;"
        "Select s pattern s(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_PATTERN_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(1, _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(1, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(s, _)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid pattern a(s, _\"x\")") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, _\"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(s, \"x\"_)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, \"x\"_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(s, \"x_)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, \"x_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(s, x)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, x)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(_, _") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, _";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_INSUFFICIENT_TOKENS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(_ _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_ _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(_)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}

// ================ Testing pattern while relationship ================
TEST_CASE("Valid queries with one pattern clause for while succeeds") {
  SECTION("Valid pattern w(_, _)") {
    string validQuery =
        "while w;"
        "Select w pattern w(_, _)";

    // expected
    SynonymMap map = {{"w", query::DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::WHILE, "w"},
                                    query::ParamType::WILDCARD, "_", {});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};
    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern w(\"x\", _)") {
    string validQuery =
        "while w;"
        "Select w pattern w(\"x\", _)";

    // expected
    SynonymMap map = {{"w", query::DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::WHILE, "w"},
                                    query::ParamType::NAME_LITERAL, "x", {});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};
    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern w(v, _)") {
    string validQuery =
        "while w; variable v;"
        "Select w pattern w(v, _)";

    // expected
    SynonymMap map = {
        {"w", query::DesignEntity::WHILE},
        {"v", query::DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::WHILE, "w"},
                                    query::ParamType::SYNONYM, "v", {});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};
    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries with one pattern clause for while throws") {
  SECTION("Invalid pattern w(_)") {
    string invalidQuery =
        "while w;"
        "Select w pattern w(_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern w(p, _)") {
    string invalidQuery =
        "while w; procedure p;"
        "Select w pattern w(p, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid pattern w(s, _)") {
    string invalidQuery =
        "while w; stmt s;"
        "Select w pattern w(s, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid pattern w(1, _)") {
    string invalidQuery =
        "while w;"
        "Select w pattern w(1, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}

// ================ Testing pattern if relationship ================
TEST_CASE("Valid queries with one pattern clause for if succeeds") {
  SECTION("Valid pattern ifs(_, _, _)") {
    string validQuery =
        "if ifs;"
        "Select ifs pattern ifs(_, _, _)";

    // expected
    SynonymMap map = {{"ifs", query::DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::IF, "ifs"},
                                    query::ParamType::WILDCARD, "_", {});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};
    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern ifs(\"x\", _, _)") {
    string validQuery =
        "if ifs;"
        "Select ifs pattern ifs(\"x\", _, _)";

    // expected
    SynonymMap map = {{"ifs", query::DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::IF, "ifs"},
                                    query::ParamType::NAME_LITERAL, "x", {});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};
    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern ifs(v, _, _)") {
    string validQuery =
        "if ifs; variable v;"
        "Select ifs pattern ifs(v, _, _)";

    // expected
    SynonymMap map = {
        {"ifs", query::DesignEntity::IF},
        {"v", query::DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::IF, "ifs"},
                                    query::ParamType::SYNONYM, "v", {});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};
    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);
    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries with one pattern clause for if throws") {
  SECTION("Invalid pattern ifs(_)") {
    string invalidQuery =
        "if ifs;"
        "Select ifs pattern ifs(_)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(_, _)") {
    string invalidQuery =
        "if ifs;"
        "Select ifs pattern ifs(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(s, _, _)") {
    string invalidQuery =
        "if ifs; stmt s;"
        "Select ifs pattern ifs(s, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid pattern ifs(p, _, _)") {
    string invalidQuery =
        "if ifs; procedure p;"
        "Select ifs pattern ifs(p, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid pattern ifs(1, _, _)") {
    string invalidQuery =
        "if ifs;"
        "Select ifs pattern ifs(1, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(_, v, _)") {
    string invalidQuery =
        "if ifs; variable v;"
        "Select ifs pattern ifs(_, v, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(_, _, v)") {
    string invalidQuery =
        "if ifs; variable v;"
        "Select ifs pattern ifs(_, _, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(_\"x\"_, _, _)") {
    string invalidQuery =
        "if ifs;"
        "Select ifs pattern ifs(_\"x\"_, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}
