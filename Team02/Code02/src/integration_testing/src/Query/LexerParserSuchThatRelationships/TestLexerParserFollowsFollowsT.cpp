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

// ====================== Testing Follows relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for Follows relationship "
    "succeeds") {
  SECTION("Valid Follows(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
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

  SECTION("Valid Follows(1, _))") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(1, _)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(1, s))") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(1, s)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::SYNONYM, "s");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(_, 1))") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(_, 1)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::INTEGER_LITERAL, "1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(_, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(_, _)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(_, s))") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(_, s)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::SYNONYM, "s");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(s, 1)") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(s, 1)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::INTEGER_LITERAL, "1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(s, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(s, _)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(s, s1))") {
    string validQuery =
        "stmt s, s1;"
        "Select s such that Follows(s, s1)";

    // expected
    SynonymMap map = {
        {"s", query::DesignEntity::STATEMENT},
        {"s1", query::DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::SYNONYM, "s1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(r, r1))") {
    string validQuery =
        "read r, r1;"
        "Select r such that Follows(r, r1)";

    // expected
    SynonymMap map = {
        {"r", query::DesignEntity::READ},
        {"r1", query::DesignEntity::READ},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::READ, "r", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "r",
                                     query::ParamType::SYNONYM, "r1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(p, p1))") {
    string validQuery =
        "print p, p1;"
        "Select p such that Follows(p, p1)";

    // expected
    SynonymMap map = {
        {"p", query::DesignEntity::PRINT},
        {"p1", query::DesignEntity::PRINT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::PRINT, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "p",
                                     query::ParamType::SYNONYM, "p1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(w, w1))") {
    string validQuery =
        "while w, w1;"
        "Select w such that Follows(w, w1)";

    // expected
    SynonymMap map = {
        {"w", query::DesignEntity::WHILE},
        {"w1", query::DesignEntity::WHILE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "w",
                                     query::ParamType::SYNONYM, "w1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(ifs, ifs1))") {
    string validQuery =
        "if ifs, ifs1;"
        "Select ifs such that Follows(ifs, ifs1)";

    // expected
    SynonymMap map = {
        {"ifs", query::DesignEntity::IF},
        {"ifs1", query::DesignEntity::IF},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "ifs",
                                     query::ParamType::SYNONYM, "ifs1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(a, a1))") {
    string validQuery =
        "assign a, a1;"
        "Select a such that Follows(a, a1)";

    // expected
    SynonymMap map = {
        {"a", query::DesignEntity::ASSIGN},
        {"a1", query::DesignEntity::ASSIGN},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::SYNONYM, "a1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(c, c1))") {
    string validQuery =
        "call c, c1;"
        "Select c such that Follows(c, c1)";

    // expected
    SynonymMap map = {
        {"c", query::DesignEntity::CALL},
        {"c1", query::DesignEntity::CALL},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::CALL, "c", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "c",
                                     query::ParamType::SYNONYM, "c1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Follows(n, n1))") {
    string validQuery =
        "prog_line n, n1;"
        "Select n such that Follows(n, n1)";

    // expected
    SynonymMap map = {
        {"n", query::DesignEntity::PROG_LINE},
        {"n1", query::DesignEntity::PROG_LINE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::PROG_LINE, "n", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "n",
                                     query::ParamType::SYNONYM, "n1");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries for one such that clause for Follows throws") {
  SECTION("Invalid Follows(\"x\", 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(\"x\", 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Follows(1, \"x\")") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1, \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Follows(v, 2)") {
    string invalidQuery =
        "variable v;"
        "Select v such that Follows(v, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Follows(1, v)") {
    string invalidQuery =
        "variable v;"
        "Select v such that Follows(1, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Follows(p, 1)") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Follows(p, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Follows query(1, p)") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Follows(1, p)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Follows(con, 1)") {
    string invalidQuery =
        "constant con;"
        "Select con such that Follows(con, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Follows(1, con)") {
    string invalidQuery =
        "constant con;"
        "Select con such that Follows(1, con)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Follows(1, 2") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1, 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_INSUFFICIENT_TOKENS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Follows(1 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Follows(1)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}

// ====================== Testing Follows* relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for Follows* relationship "
    "succeeds") {
  SECTION("Valid Follows*(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows*(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses,
                                     query::RelationshipType::FOLLOWS_T,
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

TEST_CASE("Invalid queries for one such that clause for Follows* throws") {
  SECTION("Invalid Follows * (1, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows * (\"x\", 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}
