#include <Query/Parser/QueryParser.h>
#include <Query/Parser/QueryToken.h>

#include <unordered_map>
#include <vector>
#include <string>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

TEST_CASE("Valid synonym declarations is parsed successfully") {
  SECTION("Valid synonym declarations with repeated design entity") {
    string validQuery =
        "stmt s, s1; stmt s2;"
        "Select s";

    // expected
    SynonymMap map = {
        {"s", query::DesignEntity::STATEMENT},
        {"s1", query::DesignEntity::STATEMENT},
        {"s2", query::DesignEntity::STATEMENT},
    };
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};
    vector<query::ConditionClause> clauses;
    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid synonym declarations for all possible design entities") {
    string validQuery =
        "stmt s, s1, s2; read r; print pr; while w, w1; if ifs; assign a; "
        "variable v; "
        "constant c; procedure p;"
        "Select s";

    // expected
    SynonymMap map = {
        {"s", query::DesignEntity::STATEMENT},
        {"s1", query::DesignEntity::STATEMENT},
        {"s2", query::DesignEntity::STATEMENT},
        {"r", query::DesignEntity::READ},
        {"pr", query::DesignEntity::PRINT},
        {"w", query::DesignEntity::WHILE},
        {"w1", query::DesignEntity::WHILE},
        {"ifs", query::DesignEntity::IF},
        {"a", query::DesignEntity::ASSIGN},
        {"v", query::DesignEntity::VARIABLE},
        {"c", query::DesignEntity::CONSTANT},
        {"p", query::DesignEntity::PROCEDURE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};
    vector<query::ConditionClause> clauses;
    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid synonym declarations throws") {
  SECTION("Invalid call synonym throws") {
    string invalidQuery =
        "call c;"
        "Select c";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_DESIGN_ENTITY_MSG);
  }

  SECTION("Invalid design entity throws") {
    string invalidQuery =
        "stmtt s;"
        "Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_DESIGN_ENTITY_MSG);
  }

  SECTION("Invalid symbol throws") {
    string invalidQuery = "stmt s, Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid symbol throws") {
    string invalidQuery =
        "stmt s; s1;"
        "Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
  }

  SECTION("Invalid synonym name throws") {
    string invalidQuery =
        "stmt 1name;"
        "Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_NAME_MSG);
  }

  SECTION("Invalid duplicate synonym name throws") {
    string invalidQuery =
        "stmt s; print p; variable s;"
        "Select p";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_DUPLICATE_SYNONYM_MSG);
  }

  SECTION("Invalid usage of undefined synonym throws") {
    string invalidQuery =
        "stmt s;"
        "Select p";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_UNDECLARED_SYNONYM_MSG);
  }
}

TEST_CASE("Valid query with no such that or pattern clause succeeds") {
  string validQuery = "stmt s; Select s";
  // expected
  SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
  query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};
  vector<query::ConditionClause> clauses;
  tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

  // actual
  tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

  // test
  REQUIRE(get<0>(actual) == get<0>(expected));
  REQUIRE(get<1>(actual) == get<1>(expected));
}

TEST_CASE(
    "Valid queries with one such that clause for Follows relationship "
    "succeeds") {
  SECTION("Valid Follows(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::SYNONYM, "s");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::INTEGER_LITERAL, "1");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::WILDCARD, "_",
                                     query::ParamType::SYNONYM, "s");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::INTEGER_LITERAL, "1");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::WILDCARD, "_");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::SYNONYM, "s1");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
  }

  SECTION("Invalid Follows(1, \"x\")") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1, \"x\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_STMT_REF_MSG);
  }

  SECTION("Invalid Follows(v, 2)") {
    string invalidQuery =
        "variable v;"
        "Select v such that Follows(v, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
  }

  SECTION("Invalid Follows(1, v)") {
    string invalidQuery =
        "variable v;"
        "Select v such that Follows(1, v)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
  }

  SECTION("Invalid Follows(p, 1)") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Follows(p, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
  }

  SECTION("Invalid Follows query(1, p)") {
    string invalidQuery =
        "procedure p;"
        "Select p such that Follows(1, p)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
  }

  SECTION("Invalid Follows(con, 1)") {
    string invalidQuery =
        "constant con;"
        "Select con such that Follows(con, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
  }

  SECTION("Invalid Follows(1, con)") {
    string invalidQuery =
        "constant con;"
        "Select con such that Follows(1, con)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_MSG);
  }

  SECTION("Invalid Follows(1, 2") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1, 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_INSUFFICIENT_TOKENS_MSG);
  }

  SECTION("Invalid Follows(1 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
  }

  SECTION("Invalid Follows(1)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(1)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }
}

TEST_CASE(
    "Valid queries with one such that clause for Follows* relationship "
    "succeeds") {
  SECTION("Valid Follows*(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Follows*(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses,
                                     query::RelationshipType::FOLLOWS_T,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
  }
}

TEST_CASE(
    "Valid queries with one such that clause for Parent relationship "
    "succeeds") {
  SECTION("Valid Parent(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Parent(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::PARENT,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE(
    "Valid queries with one such that clause for Parent* relationship "
    "succeeds") {
  SECTION("Valid Parent*(1, 2)") {
    string validQuery =
        "stmt s;"
        "Select s such that Parent*(1, 2)";

    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::PARENT_T,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid queries for one such that clause for Parent* throws") {
  SECTION("Invalid Parent *(1, 2)") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Parent *(\"x\", 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
  }
}

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

TEST_CASE(
    "Valid queries with one such that clause for ModifiesS relationship "
    "succeeds") {
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
}

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

  SECTION("Valid pattern a(_, \"x\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, \"x\")";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    query::MatchType::EXACT, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(_, \"1\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, \"1\")";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    query::MatchType::EXACT, "1");

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

  SECTION("Valid pattern a(\"x\", \"x\")") {
    string validQuery =
        "assign a;"
        "Select a pattern a(\"x\", \"x\")";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::NAME_LITERAL, "x",
                                    query::MatchType::EXACT, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::NAME_LITERAL, "x",
                                    query::MatchType::EXACT, "1");

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

  SECTION("Valid pattern a(v, \"x\")") {
    string validQuery =
        "assign a; variable v;"
        "Select a pattern a(v, \"x\")";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN},
                      {"v", query::DesignEntity::VARIABLE}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::SYNONYM, "v",
                                    query::MatchType::EXACT, "x");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

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
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN},
                      {"v", query::DesignEntity::VARIABLE}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::SYNONYM, "v",
                                    query::MatchType::EXACT, "1");

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

TEST_CASE("Valid queries with one such that clause and one pattern clause") {
  SECTION("Valid such that Follows(1, 2) pattern a(_, _)") {
    string validQuery =
        "assign a;"
        "Select a such that Follows(1, 2) pattern a(_, _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");

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
}

TEST_CASE(
    "Invalid queries with incorrect number or sequence of such-that/pattern "
    "clause") {
  SECTION("Invalid pattern a(_, _) such that Follows(1, 2)") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, _) such that Follows(1, 2)";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_NUM_MSG);
  }

  SECTION("Invalid such that Follows(1, 2) such that Follows(2, 3)") {
    string invalidQuery =
        "assign a;"
        "Select a such that Follows(1, 2) such that Follows(2, 3))";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
  }

  SECTION("Invalid such that Follows(1, 2) and Follows(2, 3)") {
    string invalidQuery =
        "assign a;"
        "Select a such that Follows(1, 2) and Follows(2, 3))";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
  }

  SECTION("Invalid pattern a(_, _) pattern a(_, \"x\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, _) pattern a(_, \"x\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_NUM_MSG);
  }

  SECTION("Invalid pattern a(_, _) and pattern a(_, \"x\")") {
    string invalidQuery =
        "assign a;"
        "Select a pattern a(_, _) and pattern a(_, \"x\")";

    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_NUM_MSG);
  }
}
