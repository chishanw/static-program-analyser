#include <Query/Parser/QueryParser.h>
#include <Query/Parser/QueryToken.h>

#include <unordered_map>
#include <vector>
#include <string>

#include "../TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ====================== Testing UsesS relationship ======================
TEST_CASE("Valid queries for one such that clause for UsesS succeeds") {
  SECTION("Valid Uses(1, \"x\")") {
    string validQuery =
        "stmt s;"
        "Select s such that Uses(1, \"x\")";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(1, v)") {
    string validQuery =
        "stmt s; variable v;"
        "Select s such that Uses(1, v)";

    // expected
    SynonymMap map = {
        {"s", query::DesignEntity::STATEMENT},
        {"v", query::DesignEntity::VARIABLE},
        };
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(1, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Uses(1, _)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(a, \"x\")") {
    string validQuery =
        "assign a;"
        "Select a such that Uses(a, \"x\")";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(a, v)") {
    string validQuery =
        "assign a; variable v;"
        "Select a such that Uses(a, v)";

    // expected
    SynonymMap map = {
        {"a", query::DesignEntity::ASSIGN},
        {"v", query::DesignEntity::VARIABLE},
        };
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(a, _)") {
    string validQuery =
        "assign a;"
        "Select a such that Uses(a, _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(pn, \"x\")") {
    string validQuery =
        "print pn;"
        "Select pn such that Uses(pn, \"x\")";

    // expected
    SynonymMap map = {{"pn", query::DesignEntity::PRINT}};
    query::Synonym selectSynonym = {query::DesignEntity::PRINT, "pn"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "pn",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(pn, v)") {
    string validQuery =
        "print pn; variable v;"
        "Select pn such that Uses(pn, v)";

    // expected
    SynonymMap map = {
        {"pn", query::DesignEntity::PRINT},
        {"v", query::DesignEntity::VARIABLE},
        };
    query::Synonym selectSynonym = {query::DesignEntity::PRINT, "pn"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "pn",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(pn, _)") {
    string validQuery =
        "print pn;"
        "Select pn such that Uses(pn, _)";

    // expected
    SynonymMap map = {{"pn", query::DesignEntity::PRINT}};
    query::Synonym selectSynonym = {query::DesignEntity::PRINT, "pn"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "pn",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(ifs, \"x\")") {
    string validQuery =
        "if ifs;"
        "Select ifs such that Uses(ifs, \"x\")";

    // expected
    SynonymMap map = {{"ifs", query::DesignEntity::IF}};
    query::Synonym selectSynonym = {query::DesignEntity::IF, "ifs"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "ifs",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(ifs, v)") {
    string validQuery =
        "if ifs; variable v;"
        "Select ifs such that Uses(ifs, v)";

    // expected
    SynonymMap map = {
        {"ifs", query::DesignEntity::IF},
        {"v", query::DesignEntity::VARIABLE},
        };
    query::Synonym selectSynonym = {query::DesignEntity::IF, "ifs"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "ifs",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(ifs, _)") {
    string validQuery =
        "if ifs;"
        "Select ifs such that Uses(ifs, _)";

    // expected
    SynonymMap map = {{"ifs", query::DesignEntity::IF}};
    query::Synonym selectSynonym = {query::DesignEntity::IF, "ifs"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "ifs",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(w, \"x\")") {
    string validQuery =
        "while w;"
        "Select w such that Uses(w, \"x\")";

    // expected
    SynonymMap map = {{"w", query::DesignEntity::WHILE}};
    query::Synonym selectSynonym = {query::DesignEntity::WHILE, "w"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "w",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(w, v)") {
    string validQuery =
        "while w; variable v;"
        "Select w such that Uses(w, v)";

    // expected
    SynonymMap map = {
        {"w", query::DesignEntity::WHILE},
        {"v", query::DesignEntity::VARIABLE},
        };
    query::Synonym selectSynonym = {query::DesignEntity::WHILE, "w"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "w",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(w, _)") {
    string validQuery =
        "while w;"
        "Select w such that Uses(w, _)";

    // expected
    SynonymMap map = {{"w", query::DesignEntity::WHILE}};
    query::Synonym selectSynonym = {query::DesignEntity::WHILE, "w"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "w",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(s, \"x\")") {
    string validQuery =
        "stmt s;"
        "Select s such that Uses(s, \"x\")";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(s, v)") {
    string validQuery =
        "stmt s; variable v;"
        "Select s such that Uses(s, v)";

    // expected
    SynonymMap map = {
        {"s", query::DesignEntity::STATEMENT},
        {"v", query::DesignEntity::VARIABLE},
        };
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(s, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Uses(s, _)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries for one such that clause for UsesS throws") {
  SECTION("Invalid Uses(r, \"x\")") {
    string invalidQuery =
        "read r;"
        "Select r such that Uses(r, \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_SYNONYM_ENTITY_MSG);
  }

  SECTION("Invalid Uses(_, v)") {
    string invalidQuery =
        "stmt s; variable v;"
        "Select s such that Uses(_, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_MODIFIES_WILDCARD_MSG);
  }

  SECTION("Invalid Uses(1, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Uses(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_MODIFIES_INTEGER_MSG);
  }

  SECTION("Invalid Uses(1, s)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Uses(1, s)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
  }
}

// ====================== Testing UsesP relationship ======================
TEST_CASE("Invalid queries for one such that clause for UsesP throws") {
  SECTION("Invalid Uses(p, \"x\")") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Uses(p, \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_SYNONYM_ENTITY_MSG);
  }

  SECTION("Invalid Uses(\"y\", \"x\")") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Uses(\"y\", \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USESP_MODIFIESP_MSG);
  }
}
