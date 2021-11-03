#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>
#include <Common/Common.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ====================== Testing UsesS relationship ======================
TEST_CASE("Valid queries for one such that clause for UsesS succeeds") {
  SECTION("Valid Uses(1, \"x\")") {
    string validQuery =
        "stmt s;"
        "Select s such that Uses(1, \"x\")";

    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
        {"s", DesignEntity::STATEMENT},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
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

  SECTION("Valid Uses(a, \"x\")") {
    string validQuery =
        "assign a;"
        "Select a such that Uses(a, \"x\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
        {"a", DesignEntity::ASSIGN},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"pn", DesignEntity::PRINT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PRINT, "pn", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "pn",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
        {"pn", DesignEntity::PRINT},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PRINT, "pn", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "pn",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"pn", DesignEntity::PRINT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PRINT, "pn", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "pn",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"ifs", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "ifs",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
        {"ifs", DesignEntity::IF},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "ifs",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"ifs", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "ifs",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"w", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "w",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
        {"w", DesignEntity::WHILE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "w",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"w", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "w",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
        {"s", DesignEntity::STATEMENT},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
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

  SECTION("Valid Uses(c, _)") {
    string validQuery =
        "call c;"
        "Select c such that Uses(c, _)";

    // expected
    SynonymMap map = {{"c", DesignEntity::CALL}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::CALL, "c", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "c",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(n, _)") {
    string validQuery =
        "prog_line n;"
        "Select n such that Uses(n, _)";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROG_LINE, "n", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_S,
                                     query::ParamType::SYNONYM, "n",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Uses(_, v)") {
    string invalidQuery =
        "stmt s; variable v;"
        "Select s such that Uses(_, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_MODIFIES_WILDCARD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Uses(1, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Uses(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_MODIFIES_INTEGER_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Uses(1, s)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Uses(1, s)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }
}

// ====================== Testing UsesP relationship ======================
TEST_CASE("Valid queries for one such that clause for UsesP throws") {
  SECTION("Valid Uses(p, \"x\")") {
    string validQuery =
        "procedure p;"
        "Select p such that Uses(p, \"x\")";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_P,
                                     query::ParamType::SYNONYM, "p",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(p, v)") {
    string validQuery =
        "procedure p; variable v;"
        "Select p such that Uses(p, v)";

    // expected
    SynonymMap map = {
        {"p", DesignEntity::PROCEDURE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_P,
                                     query::ParamType::SYNONYM, "p",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(p, _)") {
    string validQuery =
        "procedure p;"
        "Select p such that Uses(p, _)";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_P,
                                     query::ParamType::SYNONYM, "p",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(\"x\", \"y\")") {
    string validQuery =
        "procedure p;"
        "Select p such that Uses(\"x\", \"y\")";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_P,
                                     query::ParamType::NAME_LITERAL, "x",
                                     query::ParamType::NAME_LITERAL, "y");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(\"x\", v)") {
    string validQuery =
        "procedure p; variable v;"
        "Select p such that Uses(\"x\", v)";

    // expected
    SynonymMap map = {
        {"p", DesignEntity::PROCEDURE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_P,
                                     query::ParamType::NAME_LITERAL, "x",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Uses(\"x\", _)") {
    string validQuery =
        "procedure p;"
        "Select p such that Uses(\"x\", _)";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::USES_P,
                                     query::ParamType::NAME_LITERAL, "x",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}
