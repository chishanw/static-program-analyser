#include <Common/Common.h>
#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ====================== Testing Next relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for Next relationship "
    "succeeds") {
  SECTION("Valid Next(1, 2)") {
    string validQuery = "Select BOOLEAN such that Next(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
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

  SECTION("Valid Next(1, _)") {
    string validQuery = "Select BOOLEAN such that Next(1, _)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
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

  SECTION("Valid Next(1, n)") {
    string validQuery = "prog_line n; Select BOOLEAN such that Next(1, n)";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
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

  SECTION("Valid Next(_, 2)") {
    string validQuery = "Select BOOLEAN such that Next(_, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(_, _)") {
    string validQuery = "Select BOOLEAN such that Next(_, _)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
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

  SECTION("Valid Next(_, n)") {
    string validQuery = "prog_line n; Select BOOLEAN such that Next(_, n)";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::SYNONYM, "n");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(n1, 2)") {
    string validQuery = "prog_line n1; Select BOOLEAN such that Next(n1, 2)";

    // expected
    SynonymMap map = {{"n1", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "n1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(n1, _)") {
    string validQuery = "prog_line n1; Select BOOLEAN such that Next(n1, _)";

    // expected
    SynonymMap map = {{"n1", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "n1",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(n1, n2)") {
    string validQuery =
        "prog_line n1, n2; Select BOOLEAN such that Next(n1, n2)";

    // expected
    SynonymMap map = {{"n1", DesignEntity::PROG_LINE},
                      {"n2", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "n1",
                                     query::ParamType::SYNONYM, "n2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(s1, s2)") {
    string validQuery = "stmt s1, s2; Select BOOLEAN such that Next(s1, s2)";

    // expected
    SynonymMap map = {{"s1", DesignEntity::STATEMENT},
                      {"s2", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "s1",
                                     query::ParamType::SYNONYM, "s2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(r1, r2)") {
    string validQuery = "read r1, r2; Select BOOLEAN such that Next(r1, r2)";

    // expected
    SynonymMap map = {{"r1", DesignEntity::READ}, {"r2", DesignEntity::READ}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "r1",
                                     query::ParamType::SYNONYM, "r2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(pr1, pr2)") {
    string validQuery =
        "print pr1, pr2; Select BOOLEAN such that Next(pr1, pr2)";

    // expected
    SynonymMap map = {{"pr1", DesignEntity::PRINT},
                      {"pr2", DesignEntity::PRINT}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "pr1",
                                     query::ParamType::SYNONYM, "pr2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(c1, c2)") {
    string validQuery = "call c1, c2; Select BOOLEAN such that Next(c1, c2)";

    // expected
    SynonymMap map = {{"c1", DesignEntity::CALL}, {"c2", DesignEntity::CALL}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "c1",
                                     query::ParamType::SYNONYM, "c2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(w1, w2)") {
    string validQuery = "while w1, w2; Select BOOLEAN such that Next(w1, w2)";

    // expected
    SynonymMap map = {{"w1", DesignEntity::WHILE}, {"w2", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "w1",
                                     query::ParamType::SYNONYM, "w2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(ifs1, ifs2)") {
    string validQuery =
        "if ifs1, ifs2; Select BOOLEAN such that Next(ifs1, ifs2)";

    // expected
    SynonymMap map = {{"ifs1", DesignEntity::IF}, {"ifs2", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "ifs1",
                                     query::ParamType::SYNONYM, "ifs2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Next(a1, a2)") {
    string validQuery = "assign a1, a2; Select BOOLEAN such that Next(a1, a2)";

    // expected
    SynonymMap map = {{"a1", DesignEntity::ASSIGN},
                      {"a2", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT,
                                     query::ParamType::SYNONYM, "a1",
                                     query::ParamType::SYNONYM, "a2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::BOOLEAN, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid Next throws") {
  SECTION("Invalid Next(1, p)") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Next(1, p)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Next(1, con)") {
    string invalidQuery =
        "constant con;"
        "Select con such that Next(1, con)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid boolean Next(1, v)") {
    string invalidQuery =
        "variable v;"
        "Select BOOLEAN such that Next(1, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }

  SECTION("Invalid Next(\"x\", 2)") {
    string invalidQuery = "Select BOOLEAN such that Next(\"x\", 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_LINE_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Next(1, \"y\")") {
    string invalidQuery = "Select BOOLEAN such that Next(1, \"y\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_LINE_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}

// ====================== Testing Next* relationship ======================
TEST_CASE("Valid queries for Next* relationship succeeds") {
  SECTION("Valid Next*(1, 2)") {
    string validQuery = "Select BOOLEAN such that Next*(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT_T,
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

// ====================== Testing NextBip relationship ======================
TEST_CASE("Valid queries with NextBip relationship succeeds") {
  SECTION("Valid NextBip(1, 2)") {
    string validQuery = "Select BOOLEAN such that NextBip(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT_BIP,
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

// ====================== Testing NextBip* relationship ======================
TEST_CASE("Valid queries with NextBip* relationship succeeds") {
  SECTION("Valid NextBip*(1, 2)") {
    string validQuery = "Select BOOLEAN such that NextBip*(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::NEXT_BIP_T,
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
