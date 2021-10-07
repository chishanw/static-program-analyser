#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;

typedef unordered_map<std::string, query::DesignEntity> SynonymMap;
typedef query::SelectClause SelectClause;

// ================ Testing semantically invalid tokens ================
TEST_CASE("Semantically invalid tokens") {
  SECTION("Invalid 0123 digit for synonym") {
    string invalidQuery =
        "stmt s;"
        "Select s such that Follows(0123, 4)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryLexer::INVALID_INTEGER_START_ZERO_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid 0123 digit for boolean") {
    string invalidQuery = "Select BOOLEAN such that Follows(0123, 4)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryLexer::INVALID_INTEGER_START_ZERO_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }
}

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
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s"}};
    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid synonym declarations for all possible design entities") {
    string validQuery =
        "stmt s, s1, s2; read r; print pr; while w, w1; if ifs; assign a; "
        "variable v; constant c; procedure p; call ca; prog_line n;"
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
        {"ca", query::DesignEntity::CALL},
        {"n", query::DesignEntity::PROG_LINE},
    };
    query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s"}};
    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid empty synonym declarations") {
    string validQuery = "Select BOOLEAN such that Follows(1, 2)";

    // expected
    SynonymMap map = {};
    std::vector<query::Synonym> resultSynonyms = {};
    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
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

TEST_CASE("Invalid synonym declarations throws") {
  SECTION("Invalid design entity throws") {
    string invalidQuery =
        "stmtt s;"
        "Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid symbol throws") {
    string invalidQuery = "stmt s, Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_CHAR_SYMBOL_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid symbol throws") {
    string invalidQuery =
        "stmt s; s1;"
        "Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid synonym name throws") {
    string invalidQuery =
        "stmt 1name;"
        "Select s";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_NAME_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid duplicate synonym name throws") {
    string invalidQuery =
        "stmt s; print p; variable s;"
        "Select p";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_DUPLICATE_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid usage of undefined synonym throws") {
    string invalidQuery =
        "stmt s;"
        "Select p";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_UNDECLARED_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }
}

// ====================== Testing Select clause ======================
TEST_CASE("Valid query using different result clauses succeeds") {
  SECTION("Valid Select BOOLEAN") {
    string validQuery =
        "stmt s;"
        "Select BOOLEAN";
    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};

    tuple<SynonymMap, SelectClause> expected = {
        map, {{}, query::SelectType::BOOLEAN, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select s") {
    string validQuery =
        "stmt s;"
        "Select s";
    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s"}};

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select <s>") {
    string validQuery =
        "stmt s;"
        "Select <s>";
    // expected
    SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s"}};

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select <s1, s2>") {
    string validQuery =
        "stmt s1, s2;"
        "Select <s1, s2>";
    // expected
    SynonymMap map = {
        {"s1", query::DesignEntity::STATEMENT},
        {"s2", query::DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s1"},
        {query::DesignEntity::STATEMENT, "s2"},
    };

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select <s1, v, p, n, s2>") {
    string validQuery =
        "stmt s1, s2; variable v; prog_line n; procedure p;"
        "Select <s1, v, p, n, s2>";
    // expected
    SynonymMap map = {
        {"s1", query::DesignEntity::STATEMENT},
        {"s2", query::DesignEntity::STATEMENT},
        {"v", query::DesignEntity::VARIABLE},
        {"p", query::DesignEntity::PROCEDURE},
        {"n", query::DesignEntity::PROG_LINE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s1"},
        {query::DesignEntity::VARIABLE, "v"},
        {query::DesignEntity::PROCEDURE, "p"},
        {query::DesignEntity::PROG_LINE, "n"},
        {query::DesignEntity::STATEMENT, "s2"},
    };

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select <     s1 ,   s2 >") {
    string validQuery =
        "stmt s1, s2;"
        "Select <s1, s2>";
    // expected
    SynonymMap map = {
        {"s1", query::DesignEntity::STATEMENT},
        {"s2", query::DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s1"},
        {query::DesignEntity::STATEMENT, "s2"},
    };

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select <s1, s2> with condition clause") {
    string validQuery =
        "stmt s1, s2;"
        "Select <s1, s2> such that Follows*(s1, s2)";
    // expected
    SynonymMap map = {
        {"s1", query::DesignEntity::STATEMENT},
        {"s2", query::DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {query::DesignEntity::STATEMENT, "s1"},
        {query::DesignEntity::STATEMENT, "s2"},
    };

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::FOLLOWS_T, query::ParamType::SYNONYM,
        "s1", query::ParamType::SYNONYM, "s2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Invalid query for result clauses throws") {
  SECTION("Invalid Select boolean") {
    string invalidQuery = "Select boolean";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_UNDECLARED_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid Select s1, s2") {
    string invalidQuery = "stmt s1, s2; Select s1, s2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_KEYWORD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Select <s1") {
    string invalidQuery = "stmt s1; Select <s1";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_INSUFFICIENT_TOKENS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Select <>") {
    string invalidQuery = "Select <>";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_NAME_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Select") {
    string invalidQuery = "Select";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_INSUFFICIENT_TOKENS_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Select _") {
    string invalidQuery = "Select _";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_RESULT_TYPE_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
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
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::SYNONYM, "Parent",
                                     query::ParamType::SYNONYM, "Follows");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::MODIFIES_S, query::ParamType::SYNONYM,
        "s", query::ParamType::NAME_LITERAL, "Follows");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

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
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid query using a KEYWORD as a NAME throws") {
    string invalidQuery =
        "stmt validIdent;"
        "Select validIdent such that Modifies(s, \"Follows*\")";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_NAME_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
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
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid 'suchthat' keyword") {
    string invalidQuery =
        "stmt s;"
        "Select s suchthat Parent(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_KEYWORD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid 'such' keyword with missing 'that'") {
    string invalidQuery =
        "stmt s;"
        "Select s such Parent(1, 2)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_KEYWORD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid pattern keyword") {
    string invalidQuery =
        "stmt s;"
        "Select s patterna(_, _)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ST_P_KEYWORD_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}

// ============== Testing condition clauses - Empty ==============
TEST_CASE("Valid query with no such that or pattern clause succeeds") {
  string validQuery = "stmt s; Select s";
  // expected
  SynonymMap map = {{"s", query::DesignEntity::STATEMENT}};
  query::Synonym selectSynonym = {query::DesignEntity::STATEMENT, "s"};
  std::vector<query::Synonym> resultSynonyms = {selectSynonym};
  tuple<SynonymMap, SelectClause> expected = {
      map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

  // actual
  tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

  // test
  REQUIRE(get<0>(actual) == get<0>(expected));
  REQUIRE(get<1>(actual) == get<1>(expected));
}

// ========= Testing condition clauses - Multiple =========
TEST_CASE("Valid queries with many such that/pattern clause") {
  SECTION("Valid such that Follows(1, 2) pattern a(_, _)") {
    string validQuery =
        "assign a;"
        "Select a such that Follows(1, 2) pattern a(_, _)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(_, _) such that Follows(1, 2)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _) such that Follows(1, 2)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
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

  SECTION(
      "Valid such that Follows(1, 2) such that Follows(2, 3) pattern a(_,_) "
      "such that Follows*(a, 2)") {
    string validQuery =
        "assign a;"
        "Select a such that Follows(1, 2) such that Follows(2, 3) pattern "
        "a(_,_) such that Follows*(a, 2)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "2",
                                     query::ParamType::INTEGER_LITERAL, "3");
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddSuchThatClause(
        clauses, query::RelationshipType::FOLLOWS_T, query::ParamType::SYNONYM,
        "a", query::ParamType::INTEGER_LITERAL, "2");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}

TEST_CASE("Valid queries with many clauses connected with and") {
  SECTION("Valid such that Follows(1, 2) and Follows(2, 3)") {
    string validQuery =
        "assign a;"
        "Select a such that Follows(1, 2) and Follows(2, 3)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "2",
                                     query::ParamType::INTEGER_LITERAL, "3");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(_, _) and a(_, _\"x\"_)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _) and a(_, _\"x\"_)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::SUB_EXPRESSION, "x"});

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid pattern a(_, _) and a(_, _\"x\"_) such that Follows(2, 3)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _) and a(_, _\"x\"_) such that Follows(2, 3)";

    // expected
    SynonymMap map = {{"a", query::DesignEntity::ASSIGN}};
    query::Synonym selectSynonym = {query::DesignEntity::ASSIGN, "a"};
    std::vector<query::Synonym> resultSynonyms = {selectSynonym};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddPatternClause(clauses, {query::DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::SUB_EXPRESSION, "x"});
    TestQueryUtil::AddSuchThatClause(clauses, query::RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "2",
                                     query::ParamType::INTEGER_LITERAL, "3");

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, clauses}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }
}
