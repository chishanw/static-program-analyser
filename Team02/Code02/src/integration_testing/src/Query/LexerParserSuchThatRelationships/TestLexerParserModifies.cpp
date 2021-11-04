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

// ====================== Testing ModifiesS relationship ======================
TEST_CASE(
    "Valid queries with one such that clause for ModifiesS relationship "
    "succeeds") {
  SECTION("Valid Modifies(1, \"x\")") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(1, \"x\")";

    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(1, v)") {
    string validQuery =
        "stmt s; variable v;"
        "Select s such that Modifies(1, v)";

    // expected
    SynonymMap map = {
        {"s", DesignEntity::STATEMENT},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(1, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(1, _)";

    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(a, \"x\")") {
    string validQuery =
        "assign a;"
        "Select a such that Modifies(a, \"x\")";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(a, v)") {
    string validQuery =
        "assign a; variable v;"
        "Select a such that Modifies(a, v)";

    // expected
    SynonymMap map = {
        {"a", DesignEntity::ASSIGN},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(a, _)") {
    string validQuery =
        "assign a;"
        "Select a such that Modifies(a, _)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(r, \"x\")") {
    string validQuery =
        "read r;"
        "Select r such that Modifies(r, \"x\")";

    // expected
    SynonymMap map = {{"r", DesignEntity::READ}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::READ, "r", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "r",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
        {"r", DesignEntity::READ},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::READ, "r", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "r",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"r", DesignEntity::READ}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::READ, "r", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "r",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    SynonymMap map = {{"ifs", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(ifs, v)") {
    string validQuery =
        "if ifs; variable v;"
        "Select ifs such that Modifies(ifs, v)";

    // expected
    SynonymMap map = {
        {"ifs", DesignEntity::IF},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(ifs, _)") {
    string validQuery =
        "if ifs;"
        "Select ifs such that Modifies(ifs, _)";

    // expected
    SynonymMap map = {{"ifs", DesignEntity::IF}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::IF, "ifs", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(w, \"x\")") {
    string validQuery =
        "while w;"
        "Select w such that Modifies(w, \"x\")";

    // expected
    SynonymMap map = {{"w", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(w, v)") {
    string validQuery =
        "while w; variable v;"
        "Select w such that Modifies(w, v)";

    // expected
    SynonymMap map = {
        {"w", DesignEntity::WHILE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(w, _)") {
    string validQuery =
        "while w;"
        "Select w such that Modifies(w, _)";

    // expected
    SynonymMap map = {{"w", DesignEntity::WHILE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::WHILE, "w", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(s, \"x\")") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(s, \"x\")";

    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(s, v)") {
    string validQuery =
        "stmt s; variable v;"
        "Select s such that Modifies(s, v)";

    // expected
    SynonymMap map = {
        {"s", DesignEntity::STATEMENT},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(s, _)") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(s, _)";

    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(c, \"x\")") {
    string validQuery =
        "call c;"
        "Select c such that Modifies(c, \"x\")";

    // expected
    SynonymMap map = {{"c", DesignEntity::CALL}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::CALL, "c", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "c",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(c, v)") {
    string validQuery =
        "call c; variable v;"
        "Select c such that Modifies(c, v)";

    // expected
    SynonymMap map = {
        {"c", DesignEntity::CALL},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::CALL, "c", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "c",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(c, _)") {
    string validQuery =
        "call c;"
        "Select c such that Modifies(c, _)";

    // expected
    SynonymMap map = {{"c", DesignEntity::CALL}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::CALL, "c", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

  SECTION("Valid Modifies(n, \"x\")") {
    string validQuery =
        "prog_line n;"
        "Select n such that Modifies(n, \"x\")";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROG_LINE, "n", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "n",
                                     query::ParamType::NAME_LITERAL, "x");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(n, v)") {
    string validQuery =
        "prog_line n; variable v;"
        "Select n such that Modifies(n, v)";

    // expected
    SynonymMap map = {
        {"n", DesignEntity::PROG_LINE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROG_LINE, "n", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "n",
                                     query::ParamType::SYNONYM, "v");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Modifies(n, _)") {
    string validQuery =
        "prog_line n;"
        "Select n such that Modifies(n, _)";

    // expected
    SynonymMap map = {{"n", DesignEntity::PROG_LINE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROG_LINE, "n", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
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

TEST_CASE("Invalid queries for one such that clause for ModifiesS throws") {
  SECTION("Invalid Modifies(s.stmt#, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Modifies(s.stmt#, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Modifies(1, v.varName)") {
    string invalidQuery =
        "variable v;"
        "Select v.varName such that Modifies(1, v.varName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Modifies(1, c.procName)") {
    string invalidQuery =
        "call c;"
        "Select v.varName such that Modifies(1, c.procName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Modifies(1, con.value)") {
    string invalidQuery =
        "constant con;"
        "Select BOOLEAN such that Modifies(1, con.value)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Modifies(s.stmt#, v.varName)") {
    string invalidQuery =
        "constant con; variable v;"
        "Select BOOLEAN such that Modifies(s.stmt#, v.varName)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Modifies(pn, \"x\")") {
    string invalidQuery =
        "print pn;"
        "Select pn such that Modifies(pn, \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_MODIFIES_SYNONYM_ENTITY_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Modifies(_, v)") {
    string invalidQuery =
        "stmt s; variable v;"
        "Select s such that Modifies(_, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_USES_MODIFIES_WILDCARD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Modifies(1, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Modifies(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ENT_REF_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Modifies(1, s)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Modifies(1, s)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_NON_VARIABLE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }
}

// ====================== Testing ModifiesP relationship ======================
TEST_CASE("Valid queries for one such that clause for ModifiesP throws") {
  SECTION("Valid Modifies(p, \"x\")") {
    string validQuery =
        "procedure p;"
        "Select p such that Modifies(p, \"x\")";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_P,
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

  SECTION("Valid Modifies(p, v)") {
    string validQuery =
        "procedure p; variable v;"
        "Select p such that Modifies(p, v)";

    // expected
    SynonymMap map = {
        {"p", DesignEntity::PROCEDURE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_P,
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

  SECTION("Valid Modifies(p, _)") {
    string validQuery =
        "procedure p;"
        "Select p such that Modifies(p, _)";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_P,
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

  SECTION("Valid Modifies(\"x\", \"y\")") {
    string validQuery =
        "procedure p;"
        "Select p such that Modifies(\"x\", \"y\")";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_P,
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

  SECTION("Valid Modifies(\"x\", v)") {
    string validQuery =
        "procedure p; variable v;"
        "Select p such that Modifies(\"x\", v)";

    // expected
    SynonymMap map = {
        {"p", DesignEntity::PROCEDURE},
        {"v", DesignEntity::VARIABLE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_P,
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

  SECTION("Valid Modifies(\"x\", _)") {
    string validQuery =
        "procedure p;"
        "Select p such that Modifies(\"x\", _)";

    // expected
    SynonymMap map = {{"p", DesignEntity::PROCEDURE}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_P,
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
