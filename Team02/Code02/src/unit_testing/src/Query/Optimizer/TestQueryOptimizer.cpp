#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Optimizer/QueryOptimizer.h>

#include <functional>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;
using namespace query;
using namespace Catch::Matchers;

namespace optimizer_unit_test {
struct OptimizerTester {
  // to test important private methods
  static Groups groupClauses(QueryOptimizer* optimizer,
                             vector<CLAUSE> clauses) {
    return optimizer->groupClauses(clauses);
  }
  static GroupDetailAndGroupPairs extractGroupDetails(
      QueryOptimizer* optimizer, SelectType t, std::vector<Synonym> synonyms,
      Groups group) {
    return optimizer->extractGroupDetails(t, synonyms, group);
  }
};
}  // namespace optimizer_unit_test

// ================ Testing duplicates ================
TEST_CASE("Duplicate clauses are removed") {
  PKB* pkb = new PKB();
  QueryOptimizer optimizer(pkb);

  ConditionClause stLiteral = {{RelationshipType::FOLLOWS_T,
                                {ParamType::INTEGER_LITERAL, "1"},
                                {ParamType::INTEGER_LITERAL, "2"}},
                               {},
                               {},
                               ConditionClauseType::SUCH_THAT};

  ConditionClause stSynonym1 = {{RelationshipType::FOLLOWS_T,
                                 {ParamType::SYNONYM, "s1"},
                                 {ParamType::SYNONYM, "s2"}},
                                {},
                                {},
                                ConditionClauseType::SUCH_THAT};

  ConditionClause stSynonym2 = {{RelationshipType::CALLS,
                                 {ParamType::SYNONYM, "p"},
                                 {ParamType::NAME_LITERAL, "procName"}},
                                {},
                                {},
                                ConditionClauseType::SUCH_THAT};

  SECTION("Duplicate literal such that clauses") {
    vector<ConditionClause> givenClauses = {stLiteral, stLiteral};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup1 = {stLiteral};
    Groups expectedGroups = {expectedGroup1};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION("Duplicate synonym with literal such that clauses") {
    vector<ConditionClause> givenClauses = {stSynonym1, stLiteral, stSynonym1};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup1 = {stLiteral};
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup2 = {stSynonym1};
    Groups expectedGroups = {expectedGroup1, expectedGroup2};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups.size() == expectedGroups.size());
    REQUIRE_THAT(actualGroups, Contains(expectedGroups));
  }

  SECTION("Duplicate synonym with synonym such that clauses") {
    vector<ConditionClause> givenClauses = {stSynonym1, stSynonym2, stSynonym1};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup2 = {stSynonym1};
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup3 = {stSynonym2};
    Groups expectedGroups = {expectedGroup2, expectedGroup3};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups.size() == expectedGroups.size());
    REQUIRE_THAT(actualGroups, Contains(expectedGroups));
  }
}

// ================ Testing grouping method ================
TEST_CASE("Clauses are grouped as expected") {
  PKB* pkb = new PKB();
  QueryOptimizer optimizer(pkb);

  // group 1 - literal clause
  ConditionClause g1stLiteral1 = {{RelationshipType::FOLLOWS_T,
                                   {ParamType::INTEGER_LITERAL, "1"},
                                   {ParamType::INTEGER_LITERAL, "2"}},
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  // group 2 - one synonym clause
  ConditionClause g2stSynonym2 = {{RelationshipType::CALLS,
                                   {ParamType::SYNONYM, "grp2p1"},
                                   {ParamType::NAME_LITERAL, "procName"}},
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  // group 3 - continuous common synonyms
  ConditionClause g3stSynonym1 = {{RelationshipType::CALLS,
                                   {ParamType::SYNONYM, "grp3p1"},
                                   {ParamType::NAME_LITERAL, "procName"}},
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  ConditionClause g3stSynonym2 = {{RelationshipType::USES_P,
                                   {ParamType::SYNONYM, "grp3p1"},
                                   {ParamType::SYNONYM, "grp3v1"}},
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  ConditionClause g3withSynonym3 = {
      {},
      {},
      {{ParamType::NAME_LITERAL, "hello"},
       {ParamType::ATTRIBUTE_PROC_NAME, "grp3p1"}},
      ConditionClauseType::WITH};

  // group 4 - disjointed common synonyms
  ConditionClause g4stSynonym1 = {{RelationshipType::FOLLOWS_T,
                                   {ParamType::SYNONYM, "grp4s1"},
                                   {ParamType::SYNONYM, "grp4s2"}},
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  ConditionClause g4stSynonym2 = {{RelationshipType::USES_P,
                                   {ParamType::SYNONYM, "grp4s3"},
                                   {ParamType::NAME_LITERAL, "varName"}},
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  ConditionClause g4withSynonym3 = {{},
                                    {},
                                    {{ParamType::ATTRIBUTE_STMT_NUM, "grp4s1"},
                                     {ParamType::ATTRIBUTE_STMT_NUM, "grp4s3"}},
                                    ConditionClauseType::WITH};

  // group 5 - disjointed common synonyms 2
  ConditionClause g5pattSynonym1 = {
      {},
      {{DesignEntity::ASSIGN, "grp5a1", false, {}},
       {ParamType::SYNONYM, "grp5v1"},
       {MatchType::SUB_EXPRESSION, "[[1]+[2]]"}},
      {},
      ConditionClauseType::PATTERN};

  ConditionClause g5stSynonym2 = {{
                                      RelationshipType::USES_P,
                                      {ParamType::SYNONYM, "grp5s1"},
                                      {ParamType::SYNONYM, "grp5s2"},
                                  },
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  ConditionClause g5withSynonym3 = {
      {},
      {},
      {{ParamType::ATTRIBUTE_STMT_NUM, "grp5ifs1"},
       {ParamType::SYNONYM, "grp5n1"}},
      ConditionClauseType::WITH};

  ConditionClause g5pattSynonym4 = {{},
                                    {{DesignEntity::IF, "grp5ifs1", false, {}},
                                     {ParamType::SYNONYM, "grp5v1"},
                                     {}},
                                    {},
                                    ConditionClauseType::PATTERN};

  ConditionClause g5stSynonym5 = {{
                                      RelationshipType::PARENT,
                                      {ParamType::SYNONYM, "grp5s2"},
                                      {ParamType::SYNONYM, "grp5a1"},
                                  },
                                  {},
                                  {},
                                  ConditionClauseType::SUCH_THAT};

  // Group 6 - with literal
  ConditionClause g6withLiteral1 = {
      {},
      {},
      {{ParamType::INTEGER_LITERAL, "1"}, {ParamType::INTEGER_LITERAL, "2"}},
      ConditionClauseType::WITH};

  SECTION("Group 1: 1 Literal such that clause group") {
    vector<ConditionClause> givenClauses = {g1stLiteral1};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup1 = {g1stLiteral1};
    Groups expectedGroups = {expectedGroup1};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION("Group 6: 1 Literal with clause group") {
    vector<ConditionClause> givenClauses = {g6withLiteral1};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup6 = {
        g6withLiteral1};
    Groups expectedGroups = {expectedGroup6};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION("Group 2: 1 Synonym clause group") {
    vector<ConditionClause> givenClauses = {g2stSynonym2};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup2 = {g2stSynonym2};
    Groups expectedGroups = {expectedGroup2};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION(
      "Partial Group 3: 2 Synonym clauses group (Continuous common synonyms)") {
    vector<ConditionClause> givenClauses = {g3stSynonym1, g3withSynonym3};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup3 = {
        g3stSynonym1, g3withSynonym3};
    Groups expectedGroups = {expectedGroup3};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION(
      "Full Group 3: 3 Synonym clauses group (Continuous common synonyms)") {
    vector<ConditionClause> givenClauses = {g3stSynonym1, g3stSynonym2,
                                            g3withSynonym3};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup3 = {
        g3stSynonym1, g3stSynonym2, g3withSynonym3};
    Groups expectedGroups = {expectedGroup3};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION(
      "Group 4: 3 Synonym group (Disjointed common synonyms requiring 1 "
      "merge)") {
    vector<ConditionClause> givenClauses = {g4stSynonym1, g4stSynonym2,
                                            g4withSynonym3};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup4 = {
        g4stSynonym1, g4stSynonym2, g4withSynonym3};
    Groups expectedGroups = {expectedGroup4};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION(
      "Group 5: 5 Synonym group (Disjointed common synonyms requiring 2 "
      "merges)") {
    vector<ConditionClause> givenClauses = {g5pattSynonym1, g5stSynonym2,
                                            g5withSynonym3, g5pattSynonym4,
                                            g5stSynonym5};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup5 = {
        g5pattSynonym1, g5stSynonym2, g5withSynonym3, g5pattSynonym4,
        g5stSynonym5};
    Groups expectedGroups = {expectedGroup5};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test
    REQUIRE(actualGroups == expectedGroups);
  }

  SECTION("All groups") {
    vector<ConditionClause> givenClauses = {
        g5pattSynonym1, g3stSynonym1,   g4stSynonym2, g2stSynonym2,
        g5pattSynonym4, g1stLiteral1,   g5stSynonym2, g4withSynonym3,
        g5withSynonym3, g6withLiteral1, g5stSynonym5, g3withSynonym3,
        g4stSynonym1,   g3stSynonym2};

    // expected
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup1 = {g1stLiteral1};
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup2 = {g2stSynonym2};
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup3 = {
        g3stSynonym1, g3stSynonym2, g3withSynonym3};
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup4 = {
        g4stSynonym1, g4stSynonym2, g4withSynonym3};
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup5 = {
        g5pattSynonym1, g5stSynonym2, g5withSynonym3, g5pattSynonym4,
        g5stSynonym5};
    unordered_set<ConditionClause, CLAUSE_HASH> expectedGroup6 = {
        g6withLiteral1};
    Groups expectedGroups = {expectedGroup1, expectedGroup2, expectedGroup3,
                             expectedGroup4, expectedGroup5, expectedGroup6};

    // actual
    Groups actualGroups = optimizer_unit_test::OptimizerTester::groupClauses(
        &optimizer, givenClauses);

    // test group regardless of order
    REQUIRE(actualGroups.size() == expectedGroups.size());
    REQUIRE_THAT(actualGroups, Contains(expectedGroups));
  }
}

// ================ Testing group detail extraction ================
TEST_CASE("Group details are extracted correctly") {
  PKB* pkb = new PKB();
  QueryOptimizer optimizer(pkb);

  ConditionClause stLiteral = {{RelationshipType::FOLLOWS_T,
                                {ParamType::INTEGER_LITERAL, "1"},
                                {ParamType::INTEGER_LITERAL, "2"}},
                               {},
                               {},
                               ConditionClauseType::SUCH_THAT};

  ConditionClause stSynonym = {{RelationshipType::FOLLOWS_T,
                                {ParamType::SYNONYM, "s1"},
                                {ParamType::SYNONYM, "s2"}},
                               {},
                               {},
                               ConditionClauseType::SUCH_THAT};

  ConditionClause wSynonym = {{},
                              {},
                              {{ParamType::ATTRIBUTE_STMT_NUM, "s1"},
                               {ParamType::SYNONYM, "n"}},
                              ConditionClauseType::WITH};

  ConditionClause pattSynonym = {{},
                                 {{DesignEntity::IF, "ifs", false, {}},
                                  {ParamType::NAME_LITERAL, "varName"},
                                  {}},
                                 {},
                                 ConditionClauseType::PATTERN};

  unordered_set<CLAUSE, CLAUSE_HASH> grp1 = {stLiteral};
  unordered_set<CLAUSE, CLAUSE_HASH> grp2 = {stSynonym, wSynonym};
  unordered_set<CLAUSE, CLAUSE_HASH> grp3 = {pattSynonym};

  SECTION(
      "Select BOOLEAN for groups [(1, 2)] [(s1, s2) (s1, n)] [(ifs, "
      "'varName')]") {
    SelectType givenSelectType = SelectType::BOOLEAN;
    vector<Synonym> givenSelectSynonyms = {};
    Groups givenGroups = {grp1, grp2, grp3};

    // expected
    GroupDetailAndGroupPairs expected = {
        {{true, {}}, grp1},
        {{true, {}}, grp2},
        {{true, {}}, grp3},
    };

    // actual
    GroupDetailAndGroupPairs actual =
        optimizer_unit_test::OptimizerTester::extractGroupDetails(
            &optimizer, givenSelectType, givenSelectSynonyms, givenGroups);

    // test
    REQUIRE(actual == expected);
  }

  SECTION(
      "Select s1 for groups [(1, 2)] [(s1, s2) (s1, n)] [(ifs, 'varName')]") {
    SelectType givenSelectType = SelectType::SYNONYMS;
    Synonym s1 = {DesignEntity::STATEMENT, "s1"};
    vector<Synonym> givenSelectSynonyms = {s1};
    Groups givenGroups = {grp1, grp2, grp3};

    // expected
    GroupDetailAndGroupPairs expected = {
        {{true, {}}, grp1},
        {{false, {s1}}, grp2},
        {{true, {}}, grp3},
    };

    // actual
    GroupDetailAndGroupPairs actual =
        optimizer_unit_test::OptimizerTester::extractGroupDetails(
            &optimizer, givenSelectType, givenSelectSynonyms, givenGroups);
    // test
    REQUIRE(actual == expected);
  }

  SECTION(
      "Select <s1, ifs> for groups [(1, 2)] [(s1, s2) (s1, n)] [(ifs, "
      "'varName')]") {
    SelectType givenSelectType = SelectType::SYNONYMS;
    Synonym s1 = {DesignEntity::STATEMENT, "s1"};
    Synonym ifs = {DesignEntity::IF, "ifs"};
    vector<Synonym> givenSelectSynonyms = {s1, ifs};
    Groups givenGroups = {grp1, grp2, grp3};

    // expected
    GroupDetailAndGroupPairs expected = {
        {{true, {}}, grp1},
        {{false, {s1}}, grp2},
        {{false, {ifs}}, grp3},
    };

    // actual
    GroupDetailAndGroupPairs actual =
        optimizer_unit_test::OptimizerTester::extractGroupDetails(
            &optimizer, givenSelectType, givenSelectSynonyms, givenGroups);
    // test
    REQUIRE(actual == expected);
  }

  SECTION(
      "Select <s1, ifs, s2, n, s3> for groups [(1, 2)] [(s1, s2) (s1, n)] "
      "[(ifs, 'varName')]") {
    SelectType givenSelectType = SelectType::SYNONYMS;
    Synonym s1 = {DesignEntity::STATEMENT, "s1"};
    Synonym s2 = {DesignEntity::STATEMENT, "s2"};
    Synonym s3 = {DesignEntity::STATEMENT, "s3"};
    Synonym ifs = {DesignEntity::IF, "ifs"};
    Synonym n = {DesignEntity::PROG_LINE, "n"};
    vector<Synonym> givenSelectSynonyms = {s1, ifs, s2, n, s3};
    Groups givenGroups = {grp1, grp2, grp3};

    // expected
    GroupDetailAndGroupPairs expected = {
        {{true, {}}, grp1},
        {{false, {s1, s2, n}}, grp2},
        {{false, {ifs}}, grp3},
    };

    // actual
    GroupDetailAndGroupPairs actual =
        optimizer_unit_test::OptimizerTester::extractGroupDetails(
            &optimizer, givenSelectType, givenSelectSynonyms, givenGroups);
    // test
    REQUIRE(actual == expected);
  }
}
