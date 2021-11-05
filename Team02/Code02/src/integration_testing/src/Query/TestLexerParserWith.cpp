#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>
#include <Common/Common.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ================ Testing with relationship ================
TEST_CASE("Valid queries with with-clause succeeds") {
  SECTION("Valid with 1 = 2") {
    string validQuery = "Select BOOLEAN with 1 = 2";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = n") {
    string validQuery =
        "prog_line n;"
        "Select BOOLEAN with 1 = n";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::SYNONYM, "n");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = con.value") {
    string validQuery =
        "constant con;"
        "Select BOOLEAN with 1 = con.value";

    // expected
    SynonymMap map = {{"con", DesignEntity::CONSTANT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_VALUE, "con");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = s.stmt#") {
    string validQuery =
        "stmt s;"
        "Select BOOLEAN with 1 = s.stmt#";

    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "s");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = r.stmt#") {
    string validQuery =
        "read r;"
        "Select BOOLEAN with 1 = r.stmt#";

    // expected
    SynonymMap map = {{"r", DesignEntity::READ}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "r");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = pr.stmt#") {
    string validQuery =
        "print pr;"
        "Select BOOLEAN with 1 = pr.stmt#";

    // expected
    SynonymMap map = {{"pr", DesignEntity::PRINT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "pr");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = c.stmt#") {
    string validQuery =
        "call c;"
        "Select BOOLEAN with 1 = c.stmt#";

    // expected
    SynonymMap map = {{"c", DesignEntity::CALL}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "c");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = w.stmt#") {
    string validQuery =
        "while w;"
        "Select BOOLEAN with 1 = w.stmt#";

    // expected
    SynonymMap map = {{"w", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "w");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = ifs.stmt#") {
    string validQuery =
        "if ifs;"
        "Select BOOLEAN with 1 = ifs.stmt#";

    // expected
    SynonymMap map = {{"ifs", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "ifs");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with 1 = a.stmt#") {
    string validQuery =
        "assign a;"
        "Select BOOLEAN with 1 = a.stmt#";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "a");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with \"x\" = \"y\"") {
    string validQuery = "Select BOOLEAN with \"x\" = \"y\"";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::NAME_LITERAL, "x",
                                 query::ParamType::NAME_LITERAL, "y");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with \"x\" = p.procName") {
    string validQuery =
        "procedure p;"
        "Select BOOLEAN with \"x\" = p.procName";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::NAME_LITERAL, "x",
                                 query::ParamType::ATTRIBUTE_PROC_NAME, "p");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with \"x\" = c.procName") {
    string validQuery =
        "call c;"
        "Select BOOLEAN with \"x\" = c.procName";

    // expected
    SynonymMap map = {{"c", DesignEntity::CALL}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::NAME_LITERAL, "x",
                                 query::ParamType::ATTRIBUTE_PROC_NAME, "c");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with \"x\" = v.varName") {
    string validQuery =
        "variable v;"
        "Select BOOLEAN with \"x\" = v.varName";

    // expected
    SynonymMap map = {{"v", DesignEntity::VARIABLE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::NAME_LITERAL, "x",
                                 query::ParamType::ATTRIBUTE_VAR_NAME, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with \"x\" = r.varName") {
    string validQuery =
        "read r;"
        "Select BOOLEAN with \"x\" = r.varName";

    // expected
    SynonymMap map = {{"r", DesignEntity::READ}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::NAME_LITERAL, "x",
                                 query::ParamType::ATTRIBUTE_VAR_NAME, "r");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with \"x\" = pr.varName") {
    string validQuery =
        "print pr;"
        "Select BOOLEAN with \"x\" = pr.varName";

    // expected
    SynonymMap map = {{"pr", DesignEntity::PRINT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::NAME_LITERAL, "x",
                                 query::ParamType::ATTRIBUTE_VAR_NAME, "pr");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with n = 2") {
    string validQuery =
        "prog_line n;"
        "Select BOOLEAN with n = 2";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::SYNONYM, "n",
                                 query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with con.value = 2") {
    string validQuery =
        "constant con;"
        "Select BOOLEAN with con.value = 2";

    // expected
    SynonymMap map = {{"con", DesignEntity::CONSTANT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::ATTRIBUTE_VALUE,
                                 "con", query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with s.stmt# = 2") {
    string validQuery =
        "stmt s;"
        "Select BOOLEAN with s.stmt# = 2";

    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "s", query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid with con.value = s.stmt#") {
    string validQuery =
        "constant con; stmt s;"
        "Select BOOLEAN with con.value = s.stmt#";

    // expected
    SynonymMap map = {{"con", DesignEntity::CONSTANT},
                      {"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::ATTRIBUTE_VALUE,
                                 "con", query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "s");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries with one with clause throws") {
  SECTION("Valid with 1 = \"y\"") {
    string invalidQuery = "Select BOOLEAN with 1 = \"y\"";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = p.procName") {
    string invalidQuery =
        "procedure p;"
        "Select BOOLEAN with 1 = p.procName";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = c.procName") {
    string invalidQuery =
        "call c;"
        "Select BOOLEAN with 1 = c.procName";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = v.varName") {
    string invalidQuery =
        "variable v;"
        "Select BOOLEAN with 1 = v.varName";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = r.varName") {
    string invalidQuery =
        "read r;"
        "Select BOOLEAN with 1 = r.varName";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = pr.varName") {
    string invalidQuery =
        "print pr;"
        "Select BOOLEAN with 1 = pr.varName";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with \"x\" = 2") {
    string invalidQuery = "Select BOOLEAN with \"x\" = 2";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with p.procName = 2") {
    string invalidQuery =
        "procedure p;"
        "Select BOOLEAN with p.procName = 2";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with con.value = \"stringHere\"") {
    string invalidQuery =
        "constant con;"
        "Select BOOLEAN with con.value = \"stringHere\"";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_DIFF_PARAM_TYPES);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = 02") {
    string invalidQuery = "Select BOOLEAN with 1 = 02";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryLexer::INVALID_INTEGER_START_ZERO_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with s = 2") {
    string invalidQuery =
        "stmt s;"
        "Select BOOLEAN with s = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with r = 2") {
    string invalidQuery =
        "read r;"
        "Select BOOLEAN with r = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with pr = 2") {
    string invalidQuery =
        "print pr;"
        "Select BOOLEAN with pr = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with c = 2") {
    string invalidQuery =
        "call c;"
        "Select BOOLEAN with c = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with w = 2") {
    string invalidQuery =
        "while w;"
        "Select BOOLEAN with w = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with ifs = 2") {
    string invalidQuery =
        "if ifs;"
        "Select BOOLEAN with ifs = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = a") {
    string invalidQuery =
        "assign a;"
        "Select BOOLEAN with 1 = a";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = v") {
    string invalidQuery =
        "variable v;"
        "Select BOOLEAN with 1 = v";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = con") {
    string invalidQuery =
        "constant con;"
        "Select BOOLEAN with 1 = con";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with 1 = p") {
    string invalidQuery =
        "procedure p;"
        "Select BOOLEAN with 1 = p";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_SYNONYM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with p.invalidPr0cName = 2") {
    string invalidQuery =
        "procedure p;"
        "Select BOOLEAN with p.invalidPr0cName = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ATTRIBUTE);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid with p.stmt# = 2") {
    string invalidQuery =
        "procedure p;"
        "Select BOOLEAN with p.stmt# = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_ATTRIBUTE_MATCH);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with p.value = 2") {
    string invalidQuery =
        "procedure p;"
        "Select BOOLEAN with p.value = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_ATTRIBUTE_MATCH);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid with _ = 2") {
    string invalidQuery = "Select BOOLEAN with _ = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_PARAM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid with \"x\" = _") {
    string invalidQuery = "Select BOOLEAN with \"x\" = _";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_W_PARAM);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION(
      "Invalid usage of undeclared synonym in with p.procName = \"count\"") {
    string invalidQuery = "Select BOOLEAN with p.procName = \"count\"";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_UNDECLARED_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }
}

TEST_CASE("Valid query with multiple with clauses succeeds") {
  SECTION(
      "Valid with 1 = 1 and n = 25 with v.varName = \"x\" and p.procName = "
      "c.procName") {
    string validQuery =
        "prog_line n; variable v; procedure p; call c;"
        "Select c "
        "with 1 = 1 and n = 25 with v.varName = \"x\" and p.procName = "
        "c.procName";

    // expected
    SynonymMap map = {
        {"n", DesignEntity::PROG_LINE},
        {"v", DesignEntity::VARIABLE},
        {"p", DesignEntity::PROCEDURE},
        {"c", DesignEntity::CALL},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::CALL, "c", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::INTEGER_LITERAL, "1");
    TestQueryUtil::AddWithClause(clauses, query::ParamType::SYNONYM, "n",
                                 query::ParamType::INTEGER_LITERAL, "25");
    TestQueryUtil::AddWithClause(clauses, query::ParamType::ATTRIBUTE_VAR_NAME,
                                 "v", query::ParamType::NAME_LITERAL, "x");
    TestQueryUtil::AddWithClause(clauses, query::ParamType::ATTRIBUTE_PROC_NAME,
                                 "p", query::ParamType::ATTRIBUTE_PROC_NAME,
                                 "c");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}
