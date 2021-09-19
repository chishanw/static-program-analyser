#include <Query/Parser/QueryParser.h>
#include <Query/Parser/QueryToken.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ====================== Testing synonym declarations ======================
TEST_CASE("Valid synonym declarations succeeds") {
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
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
  }

  SECTION("Invalid design entity throws") {
    string invalidQuery =
        "stmtt s;"
        "Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
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

// ====================== Testing empty Select clause ======================
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

// ================== Testing parsing of NAME and KEYWORD ==================
// NAME_OR_KEYWORD = Follows, Parent, varName, ...
// KEYWORD = Follows*, Parent*

TEST_CASE("Valid query using a NAME_OR_KEYWORD as a IDENT or NAME succeeds") {
  SECTION("Valid query using a NAME_OR_KEYWORD as synonym ident") {
    string validQuery =
        "stmt Parent; assign Follows;"
        "Select Parent such that Follows(Parent, Follows)";
    // expected
    SynonymMap map = {{"Parent", query::DesignEntity::STATEMENT},
                      {"Follows", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "Parent"};
    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "Parent",
                                     query::ParamType::SYNONYM, "Follows");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid query using a NAME_OR_KEYWORD as literal name succeeds") {
    string validQuery =
        "stmt s;"
        "Select s such that Modifies(s, \"Follows\")";
    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};
    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "s", query::ParamType::NAME_LITERAL, "Follows");

    tuple<SynonymMap, SelectClause> expected = {map, {selectSynonym, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid query using a KEYWORD as a IDENT or NAME throws") {
  SECTION("Invalid query using a KEYWORD as a IDENT throws") {
    string invalidQuery =
        "stmt Parent*;"
        "Select s such that Modifies(s, \"validName\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_NAME_MSG);
  }

  SECTION("Invalid query using a KEYWORD as a NAME throws") {
    string invalidQuery =
        "stmt validIdent;"
        "Select validIdent such that Modifies(s, \"Follows*\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_NAME_MSG);
  }
}

// ================== Testing syntactically invalid keywords ==================
TEST_CASE("Invalid keywords throws") {
  SECTION("Invalid 'Select' keyword") {
    string invalidQuery =
        "stmt s;"
        "Sel ect s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
  }

  SECTION("Invalid 'suchthat' keyword") {
    string invalidQuery =
        "stmt s;"
        "Select s suchthat Parent(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_NUM_MSG);
  }

  SECTION("Invalid 'such' keyword with missing 'that'") {
    string invalidQuery =
        "stmt s;"
        "Select s such Parent(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
  }

  SECTION("Invalid pattern keyword") {
    string invalidQuery =
        "stmt s;"
        "Select s patterna(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_NUM_MSG);
  }
}

// ========= Testing num and sequence of suchthat-cl and pattern-cl =========
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
    "Invalid queries with invalid number of or sequence of such-that "
    "/ pattern clause") {
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
