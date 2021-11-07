#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;
using namespace query;

// ================ Testing pattern assignment relationship ================
TEST_CASE("Valid pattern clauses for assignment succeeds") {
  SECTION("Valid pattern a(_, _)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::WILDCARD, "_", {query::MatchType::ANY, "_"});

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::ASSIGN, "a", false, {}},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::SUB_EXPRESSION, "[x]"});

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::NAME_LITERAL, "x", {query::MatchType::ANY, "_"});

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::ASSIGN, "a", false, {}},
                                    query::ParamType::NAME_LITERAL, "x",
                                    {query::MatchType::SUB_EXPRESSION, "[x]"});

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN},
                      {"v", DesignEntity::VARIABLE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::SYNONYM, "v", {query::MatchType::ANY, "_"});

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN},
                      {"v", DesignEntity::VARIABLE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::ASSIGN, "a", false, {}},
                                    query::ParamType::SYNONYM, "v",
                                    {query::MatchType::SUB_EXPRESSION, "[x]"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(_, \"x\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, \"x\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::WILDCARD, "_", {query::MatchType::EXACT, "[x]"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Invalid pattern a(_, \"1\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, \"1\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::WILDCARD, "_", {query::MatchType::EXACT, "[1]"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(v, \"x\")") {
    string validQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"x\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN},
                      {"v", DesignEntity::VARIABLE}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::SYNONYM, "v", {query::MatchType::EXACT, "[x]"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(v, \"1\")") {
    string validQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"1\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN},
                      {"v", DesignEntity::VARIABLE}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::SYNONYM, "v", {query::MatchType::EXACT, "[1]"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", \"x\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"x\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::NAME_LITERAL, "x", {query::MatchType::EXACT, "[x]"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", \"1\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"1\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::NAME_LITERAL, "x", {query::MatchType::EXACT, "[1]"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", \"1+2-3\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"1+2-3\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::ASSIGN, "a", false, {}},
                                    query::ParamType::NAME_LITERAL, "x",
                                    {query::MatchType::EXACT, "[1][2]+[3]-"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", _\"(1+(2-3))\"_)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", _\"(1+(2-3))\"_)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::NAME_LITERAL, "x",
        {query::MatchType::SUB_EXPRESSION, "[1][2][3]-+"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", _\"a*b\"_)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", _\"a*b\"_)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::NAME_LITERAL, "x",
        {query::MatchType::SUB_EXPRESSION, "[a][b]*"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", _\"Follows*Parent*Modifies\"_)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", _\"Follows*Parent*Modifies\"_)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(
        clauses, {DesignEntity::ASSIGN, "a", false, {}},
        query::ParamType::NAME_LITERAL, "x",
        {query::MatchType::SUB_EXPRESSION, "[Follows][Parent]*[Modifies]*"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(\"x\", _\"abc123 + 2 - v23 * 100000000000000\"_)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", _\"abc123 + 2 - v23 * 100000000000000\"_)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};

    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::ASSIGN, "a", false, {}},
                                    query::ParamType::NAME_LITERAL, "x",
                                    {query::MatchType::SUB_EXPRESSION,
                                     "[abc123][2]+[v23][100000000000000]*-"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid synonyms for pattern clause throws") {
  SECTION("Invalid pattern a.stmt# (_, _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a.stmt# (_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs.stmt# (_, _)") {
    string invalidQuery =
        "if ifs;"
        "Select ifs pattern ifs.stmt# (_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern w.stmt# (_, _)") {
    string invalidQuery =
        "while w;"
        "Select w pattern w.stmt# (_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern s(_, _)") {
    string invalidQuery =
        "stmt s;"
        "Select s pattern s(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid pattern p(_, _)") {
    string invalidQuery =
        "print p;"
        "Select BOOLEAN pattern p(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid pattern p(_, _\"x+y\"_)") {
    string invalidQuery =
        "print p;"
        "Select BOOLEAN pattern p(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid pattern p(_, \"x+y\")") {
    string invalidQuery =
        "print p;"
        "Select BOOLEAN pattern p(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid pattern p(_, _, _)") {
    string invalidQuery =
        "print p;"
        "Select BOOLEAN pattern p(_, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid pattern ifs(_, _)") {
    string invalidQuery =
        "if ifs;"
        "Select ifs pattern ifs(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid pattern a(_, _, _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }
}

TEST_CASE("Invalid queries with one pattern clause for assignment throws") {
  SECTION("Invalid pattern a(v.varName, _)") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(v.varName, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(p.procName, _)") {
    string invalidQuery =
        "assign a; procedure p;"
        "Select a pattern a(p.procName, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(s.stmt#, _)") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s.stmt#, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
  SECTION("Invalid pattern a(_, v.varName)") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(_, v.varName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(v.varName, s.stmt#)") {
    string invalidQuery =
        "assign a; variable v; stmt s;"
        "Select a pattern a(v.varName, s.stmt#)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(_.stmt#, _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_.stmt#, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
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
                        QueryParser::INVALID_INSUFFICIENT_TOKENS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(s, \"_x_\")") {
    string invalidQuery =
        "assign a; stmt s;"
        "Select a pattern a(s, \"_x_\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_P_EXPR_CHARA_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(v, \"keyword_keyword*\")") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"keyword_keyword*\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_P_EXPR_CHARA_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern a(v, \"x.5\")") {
    string invalidQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"x.5\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_P_EXPR_CHARA_MSG);
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

  SECTION("Invalid pattern a(1+2-3, _)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(1+2-3, _)";
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
    SynonymMap map = {{"w", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::WHILE, "w", false, {}},
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
    SynonymMap map = {{"w", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::WHILE, "w", false, {}},
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
        {"w", DesignEntity::WHILE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::WHILE, "w", false, {}},
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
  SECTION("Invalid pattern w(v.varName, _)") {
    string invalidQuery =
        "while w; variable v;"
        "Select w pattern w(v.varName, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern w(p.procName, _)") {
    string invalidQuery =
        "while w; procedure p;"
        "Select w pattern w(p.procName, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern w(s.stmt#, _)") {
    string invalidQuery =
        "assign a; while w;"
        "Select a pattern a(w.stmt#, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
  SECTION("Invalid pattern w(_, v.varName)") {
    string invalidQuery =
        "while w; variable v;"
        "Select w pattern w(_, v.varName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern w(v.varName, s.stmt#)") {
    string invalidQuery =
        "while w; variable v; stmt s;"
        "Select w pattern w(v.varName, s.stmt#)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

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
    SynonymMap map = {{"ifs", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::IF, "ifs", false, {}},
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
    SynonymMap map = {{"ifs", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::IF, "ifs", false, {}},
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
        {"ifs", DesignEntity::IF},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses,
                                    {DesignEntity::IF, "ifs", false, {}},
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
  SECTION("Invalid pattern ifs(v.varName, _, _)") {
    string invalidQuery =
        "if ifs; variable v;"
        "Select ifs pattern ifs(v.varName, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(p.procName, _, _)") {
    string invalidQuery =
        "if ifs; procedure p;"
        "Select ifs pattern ifs(p.procName, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(s.stmt#, _, _)") {
    string invalidQuery =
        "if ifs; stmt s;"
        "Select ifs pattern ifs(s.stmt#, _, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(_, v.varName, _)") {
    string invalidQuery =
        "if ifs; variable v;"
        "Select ifs pattern ifs(_, v.varName, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(_, _, v.varName)") {
    string invalidQuery =
        "if ifs; variable v;"
        "Select ifs pattern ifs(_, _, v.varName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(v.varName, s.stmt#, _)") {
    string invalidQuery =
        "if ifs; variable v; stmt s;"
        "Select ifs pattern ifs(v.varName, s.stmt#, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern ifs(v.varName, s.stmt#, p.procName)") {
    string invalidQuery =
        "if ifs; variable v; stmt s; procedure p;"
        "Select ifs pattern ifs(v.varName, s.stmt#, p.procName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

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
