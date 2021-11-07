#include <Common/Common.h>
#include <Query/Common.h>
#include <Query/Parser/QueryLexerParserCommon.h>
#include <Query/Parser/QueryParser.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "TestQueryUtil.h"
#include "catch.hpp"

using namespace std;
using namespace query;

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
        {"s", DesignEntity::STATEMENT},
        {"s1", DesignEntity::STATEMENT},
        {"s2", DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};
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
        {"s", DesignEntity::STATEMENT},  {"s1", DesignEntity::STATEMENT},
        {"s2", DesignEntity::STATEMENT}, {"r", DesignEntity::READ},
        {"pr", DesignEntity::PRINT},     {"w", DesignEntity::WHILE},
        {"w1", DesignEntity::WHILE},     {"ifs", DesignEntity::IF},
        {"a", DesignEntity::ASSIGN},     {"v", DesignEntity::VARIABLE},
        {"c", DesignEntity::CONSTANT},   {"p", DesignEntity::PROCEDURE},
        {"ca", DesignEntity::CALL},      {"n", DesignEntity::PROG_LINE},
    };
    query::Synonym selectSynonym = {DesignEntity::STATEMENT, "s"};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};
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
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
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

  SECTION("Invalid attribute name throws") {
    string invalidQuery = "stmt s.stmt#; Select s.stmt#";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SPECIFIC_CHAR_SYMBOL_MSG);
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

  SECTION("Invalid usage of undefined synonym in select clause throws") {
    string invalidQuery =
        "stmt s;"
        "Select p";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_UNDECLARED_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
  }

  SECTION("Invalid usage of undefined synonym in condition clauses throws") {
    string invalidQuery = "Select BOOLEAN such that Follows(1, s)";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_UNDECLARED_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticBooleanErrorException);
  }
}

// ====================== Testing Select result clause ======================
TEST_CASE("Valid query using different result clauses succeeds") {
  SECTION("Valid Select BOOLEAN") {
    string validQuery =
        "stmt s;"
        "Select BOOLEAN";
    // expected
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};

    tuple<SynonymMap, SelectClause> expected = {
        map, {{}, query::SelectType::BOOLEAN, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select BOOLEAN where BOOLEAN is a synonym") {
    string validQuery =
        "stmt BOOLEAN;"
        "Select BOOLEAN";
    // expected
    SynonymMap map = {{"BOOLEAN", DesignEntity::STATEMENT}};

    tuple<SynonymMap, SelectClause> expected = {
        map,
        {{{DesignEntity::STATEMENT, "BOOLEAN"}},
         query::SelectType::SYNONYMS,
         {}}};

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
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

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
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

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
        {"s1", DesignEntity::STATEMENT},
        {"s2", DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s1", false, {}},
        {DesignEntity::STATEMENT, "s2", false, {}},
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
        {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
        {"v", DesignEntity::VARIABLE},   {"p", DesignEntity::PROCEDURE},
        {"n", DesignEntity::PROG_LINE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s1", false, {}},
        {DesignEntity::VARIABLE, "v", false, {}},
        {DesignEntity::PROCEDURE, "p", false, {}},
        {DesignEntity::PROG_LINE, "n", false, {}},
        {DesignEntity::STATEMENT, "s2", false, {}},
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
        {"s1", DesignEntity::STATEMENT},
        {"s2", DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s1", false, {}},
        {DesignEntity::STATEMENT, "s2", false, {}},
    };

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select p.procName") {
    string validQuery =
        "procedure p;"
        "Select p.procName";
    // expected
    SynonymMap map = {
        {"p", DesignEntity::PROCEDURE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", true, query::Attribute::PROC_NAME},
    };

    tuple<SynonymMap, SelectClause> expected = {
        map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

    // actual
    tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

    // test
    REQUIRE(get<0>(actual) == get<0>(expected));
    REQUIRE(get<1>(actual) == get<1>(expected));
  }

  SECTION("Valid Select synonyms with all possible attributes") {
    string validQuery =
        "procedure p; call c; variable v; read r1, r2; print pr; constant con;"
        "stmt s; while w123; if ifs; assign a;"
        "Select <p, p.procName, c.procName, v.varName, r1.varName, pr.varName, "
        "con.value, s.stmt#, r2.stmt#, c, pr.stmt#, c.stmt#, w123.stmt#, "
        "ifs.stmt#, a.stmt#, a, r2>";
    // expected
    SynonymMap map = {
        {"p", DesignEntity::PROCEDURE},  {"c", DesignEntity::CALL},
        {"v", DesignEntity::VARIABLE},   {"r1", DesignEntity::READ},
        {"r2", DesignEntity::READ},      {"pr", DesignEntity::PRINT},
        {"con", DesignEntity::CONSTANT}, {"s", DesignEntity::STATEMENT},
        {"w123", DesignEntity::WHILE},   {"ifs", DesignEntity::IF},
        {"a", DesignEntity::ASSIGN},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::PROCEDURE, "p", false, {}},
        {DesignEntity::PROCEDURE, "p", true, query::Attribute::PROC_NAME},
        {DesignEntity::CALL, "c", true, query::Attribute::PROC_NAME},
        {DesignEntity::VARIABLE, "v", true, query::Attribute::VAR_NAME},
        {DesignEntity::READ, "r1", true, query::Attribute::VAR_NAME},
        {DesignEntity::PRINT, "pr", true, query::Attribute::VAR_NAME},
        {DesignEntity::CONSTANT, "con", true, query::Attribute::VALUE},
        {DesignEntity::STATEMENT, "s", true, query::Attribute::STMT_NUM},
        {DesignEntity::READ, "r2", true, query::Attribute::STMT_NUM},
        {DesignEntity::CALL, "c", false, {}},
        {DesignEntity::PRINT, "pr", true, query::Attribute::STMT_NUM},
        {DesignEntity::CALL, "c", true, query::Attribute::STMT_NUM},
        {DesignEntity::WHILE, "w123", true, query::Attribute::STMT_NUM},
        {DesignEntity::IF, "ifs", true, query::Attribute::STMT_NUM},
        {DesignEntity::ASSIGN, "a", true, query::Attribute::STMT_NUM},
        {DesignEntity::ASSIGN, "a", false, {}},
        {DesignEntity::READ, "r2", false, {}},
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
        {"s1", DesignEntity::STATEMENT},
        {"s2", DesignEntity::STATEMENT},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s1", false, {}},
        {DesignEntity::STATEMENT, "s2", false, {}},
    };

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS_T,
                                     query::ParamType::SYNONYM, "s1",
                                     query::ParamType::SYNONYM, "s2");

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

  SECTION("Invalid Select con.valueName") {
    string invalidQuery = "constant con; Select con.valueName";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ATTRIBUTE);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }

  SECTION("Invalid Select p.varName") {
    string invalidQuery = "procedure p; Select <p.procName, p.varName>";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_SYNONYM_ATTRIBUTE_MATCH);
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

  SECTION("Invalid Select undeclared synonym") {
    string invalidQuery = "Select s1";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_UNDECLARED_SYNONYM_MSG);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SemanticSynonymErrorException);
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
    SynonymMap map = {{"Parent", DesignEntity::STATEMENT},
                      {"Follows", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "Parent", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
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
    SynonymMap map = {{"s", DesignEntity::STATEMENT}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::STATEMENT, "s", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::MODIFIES_S,
                                     query::ParamType::SYNONYM, "s",
                                     query::ParamType::NAME_LITERAL, "Follows");

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

  SECTION("Invalid attribute keyword") {
    string invalidQuery =
        "stmt s;"
        "Select s with a.invalidAttri = 2";
    // test
    REQUIRE_THROWS_WITH(QueryParser().Parse(invalidQuery),
                        QueryParser::INVALID_ATTRIBUTE);
    REQUIRE_THROWS_AS(QueryParser().Parse(invalidQuery),
                      qpp::SyntacticErrorException);
  }
}

// ============== Testing condition clauses - Empty ==============
TEST_CASE("Valid query with no such that or pattern clause succeeds") {
  string validQuery = "stmt s; Select s";
  // expected
  SynonymMap map = {{"s", DesignEntity::STATEMENT}};
  std::vector<query::Synonym> resultSynonyms = {
      {DesignEntity::STATEMENT, "s", false, {}}};
  tuple<SynonymMap, SelectClause> expected = {
      map, {resultSynonyms, query::SelectType::SYNONYMS, {}}};

  // actual
  tuple<SynonymMap, SelectClause> actual = QueryParser().Parse(validQuery);

  // test
  REQUIRE(get<0>(actual) == get<0>(expected));
  REQUIRE(get<1>(actual) == get<1>(expected));
}

// ========= Testing condition clauses - Multiple =========
TEST_CASE("Valid queries with many such that/pattern/with clause") {
  SECTION("Valid such that Follows(1, 2) pattern a(_, _)") {
    string validQuery =
        "assign a;"
        "Select a such that Follows(1, 2) pattern a(_, _)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
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

  SECTION("Valid pattern a(_, _) with 1 = 1 such that Follows(1, 2)") {
    string validQuery =
        "assign a;"
        "Select a pattern a(_, _) with 1 = 1 such that Follows(1, 2)";

    // expected
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddWithClause(clauses, query::ParamType::INTEGER_LITERAL,
                                 "1", query::ParamType::INTEGER_LITERAL, "1");
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
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
      "Valid such that Follows(1, 2) with a.stmt# = 5 such that Follows(2, 3) "
      "pattern a(_,_) "
      "such that Follows*(a, 2) with n1 = n2") {
    string validQuery =
        "assign a; prog_line n1, n2;"
        "Select <a, n1, n2> such that Follows(1, 2) with a.stmt# = 5 such that "
        "Follows(2, 3) pattern "
        "a(_,_) such that Follows*(a, 2) with n1 = n2";

    // expected
    SynonymMap map = {
        {"a", DesignEntity::ASSIGN},
        {"n1", DesignEntity::PROG_LINE},
        {"n2", DesignEntity::PROG_LINE},
    };
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}},
        {DesignEntity::PROG_LINE, "n1", false, {}},
        {DesignEntity::PROG_LINE, "n2", false, {}},
    };

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");
    TestQueryUtil::AddWithClause(clauses, query::ParamType::ATTRIBUTE_STMT_NUM,
                                 "a", query::ParamType::INTEGER_LITERAL, "5");
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "2",
                                     query::ParamType::INTEGER_LITERAL, "3");
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS_T,
                                     query::ParamType::SYNONYM, "a",
                                     query::ParamType::INTEGER_LITERAL, "2");
    TestQueryUtil::AddWithClause(clauses, query::ParamType::SYNONYM, "n1",
                                 query::ParamType::SYNONYM, "n2");

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
                                     query::ParamType::INTEGER_LITERAL, "1",
                                     query::ParamType::INTEGER_LITERAL, "2");
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::SUB_EXPRESSION, "[x]"});

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
    SynonymMap map = {{"a", DesignEntity::ASSIGN}};
    std::vector<query::Synonym> resultSynonyms = {
        {DesignEntity::ASSIGN, "a", false, {}}};

    vector<query::ConditionClause> clauses;
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::ANY, "_"});
    TestQueryUtil::AddPatternClause(clauses, {DesignEntity::ASSIGN, "a"},
                                    query::ParamType::WILDCARD, "_",
                                    {query::MatchType::SUB_EXPRESSION, "[x]"});
    TestQueryUtil::AddSuchThatClause(clauses, RelationshipType::FOLLOWS,
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
