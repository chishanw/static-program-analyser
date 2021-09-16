#include <Query/Parser/QueryParser.h>
#include <Query/Parser/QueryToken.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ====================== Testing ModifiesS relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for ModifiesS relationship "
    "succeeds") {
  SECTION("Valid Modifies(1, \"x\")") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(1, \"x\")";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses,
                                     query::RelationshipType::MODIFIES_S,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(1, v)") {
    string validQuery =
        "stmt s; variable v;"
        "Select s such that Modifies(1, v)";

    // expected
    SynonymMap map = {
        {"s", query::DesignEntity::STATEMENT},
        {"v", query::DesignEntity::VARIABLE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S,
        query::ParamType::INTEGER_LITERAL, "1", query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(1, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(1, _)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses,
                                     query::RelationshipType::MODIFIES_S,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(a, \"x\")") {
    string validQuery =
        "assign a;"
        "Select a such that Modifies(a, \"x\")";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "a", query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(a, v)") {
    string validQuery =
        "assign a; variable v;"
        "Select a such that Modifies(a, v)";

    // expected
    SynonymMap map = {
        {"a", query::DesignEntity::ASSIGN},
        {"v", query::DesignEntity::VARIABLE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "a", query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(a, _)") {
    string validQuery =
        "assign a;"
        "Select a such that Modifies(a, _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "a", query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(r, \"x\")") {
    string validQuery =
        "read r;"
        "Select r such that Modifies(r, \"x\")";

    // expected
    SynonymMap map = {{"r", query::DesignEntity::READ}};
    query::Synonym selectSynonym = {query::DesignEntity::READ, "r"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "r", query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(r, v)") {
    string validQuery =
        "read r; variable v;"
        "Select r such that Modifies(r, v)";

    // expected
    SynonymMap map = {
        {"r", query::DesignEntity::READ},
        {"v", query::DesignEntity::VARIABLE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::READ, "r"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "r", query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(r, _)") {
    string validQuery =
        "read r;"
        "Select r such that Modifies(r, _)";

    // expected
    SynonymMap map = {{"r", query::DesignEntity::READ}};
    query::Synonym selectSynonym = {query::DesignEntity::READ, "r"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "r", query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(ifs, \"x\")") {
    string validQuery =
        "if ifs;"
        "Select ifs such that Modifies(ifs, \"x\")";

    // expected
    SynonymMap map = {{"ifs", query::DesignEntity::IF}};
    query::Synonym selectSynonym = {query::DesignEntity::IF, "ifs"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "ifs", query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(ifs, v)") {
    string validQuery =
        "if ifs; variable v;"
        "Select ifs such that Modifies(ifs, v)";

    // expected
    SynonymMap map = {
        {"ifs", query::DesignEntity::IF},
        {"v", query::DesignEntity::VARIABLE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::IF, "ifs"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "ifs", query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(ifs, _)") {
    string validQuery =
        "if ifs;"
        "Select ifs such that Modifies(ifs, _)";

    // expected
    SynonymMap map = {{"ifs", query::DesignEntity::IF}};
    query::Synonym selectSynonym = {query::DesignEntity::IF, "ifs"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "ifs", query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(w, \"x\")") {
    string validQuery =
        "while w;"
        "Select w such that Modifies(w, \"x\")";

    // expected
    SynonymMap map = {{"w", query::DesignEntity::WHILE}};
    query::Synonym selectSynonym = {query::DesignEntity::WHILE, "w"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "w", query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(w, v)") {
    string validQuery =
        "while w; variable v;"
        "Select w such that Modifies(w, v)";

    // expected
    SynonymMap map = {
        {"w", query::DesignEntity::WHILE},
        {"v", query::DesignEntity::VARIABLE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::WHILE, "w"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "w", query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(w, _)") {
    string validQuery =
        "while w;"
        "Select w such that Modifies(w, _)";

    // expected
    SynonymMap map = {{"w", query::DesignEntity::WHILE}};
    query::Synonym selectSynonym = {query::DesignEntity::WHILE, "w"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "w", query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(s, \"x\")") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(s, \"x\")";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "s", query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(s, v)") {
    string validQuery =
        "stmt s; variable v;"
        "Select s such that Modifies(s, v)";

    // expected
    SynonymMap map = {
        {"s", query::DesignEntity::STATEMENT},
        {"v", query::DesignEntity::VARIABLE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "s", query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(s, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(s, _)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "s", query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries for one such that clause for ModifiesS throws") {
  SECTION("Invalid Modifies(pn, \"x\")") {
    string invalidQuery =
        "print pn;"
        "Select pn such that Modifies(pn, \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_MODIFIES_SYNONYM_ENTITY_MSG);
  }

  SECTION("Invalid Modifies(_, v)") {
    string invalidQuery =
        "stmt s; variable v;"
        "Select s such that Modifies(_, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_MODIFIES_WILDCARD_MSG);
  }

  SECTION("Invalid Modifies(1, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Modifies(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_MODIFIES_INTEGER_MSG);
  }

  SECTION("Invalid Modifies(1, s)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Modifies(1, s)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
  }
}

// ====================== Testing ModifiesP relationship ======================
TEST_CASE("Invalid queries for one such that clause for ModifiesP throws") {
  SECTION("Invalid Modifies(p, \"x\")") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Modifies(p, \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_MODIFIES_SYNONYM_ENTITY_MSG);
  }

  SECTION("Invalid Modifies(\"y\", \"x\")") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Modifies(\"y\", \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USESP_MODIFIESP_MSG);
  }
}
