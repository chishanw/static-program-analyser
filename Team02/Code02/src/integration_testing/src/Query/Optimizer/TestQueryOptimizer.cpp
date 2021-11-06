#include <Common/Common.h>
#include <PKB/PKB.h>
#include <Query/Common.h>
#include <Query/Optimizer/QueryOptimizer.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "../TestQueryUtil.h"
#include "catch.hpp"

using namespace std;
using namespace query;
using namespace optimizer;

// ================ Testing duplicates ================
TEST_CASE("Duplicate clauses are removed") {
  PKB* pkb = new PKB();
  SynonymCountsTable emptyCountTable = {};
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::IF, 5);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addStmt(DesignEntity::CALL, 7);
  pkb->addStmt(DesignEntity::WHILE, 8);
  pkb->addStmt(DesignEntity::ASSIGN, 9);
  pkb->insertAt(TableType::VAR_TABLE, "var1");
  pkb->insertAt(TableType::VAR_TABLE, "var2");
  pkb->insertAt(TableType::VAR_TABLE, "var3");
  pkb->insertAt(TableType::PROC_TABLE, "proc1");

  SECTION(
      "Duplicate literal clauses: with \"x\" = \"x\" > "
      "ModifiesP(\"proc\", \"x\") > Affects(1, 2)") {
    SynonymMap emptySynonymMap = {};
    ConditionClause withStr = TestQueryUtil::BuildWithClause(
        ParamType::NAME_LITERAL, "x", ParamType::NAME_LITERAL, "x");
    ConditionClause stModifiesP = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::MODIFIES_P, ParamType::NAME_LITERAL, "proc",
        ParamType::NAME_LITERAL, "x");
    ConditionClause stAffects = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::INTEGER_LITERAL, "1",
        ParamType::INTEGER_LITERAL, "2");
    vector<ConditionClause> givenClauses = {
        stAffects, stModifiesP, stAffects,   withStr,    withStr,
        stAffects, stAffects,   stModifiesP, stModifiesP};
    SelectClause givenSelectClause = {{}, SelectType::BOOLEAN, givenClauses};

    // expected
    optional<GroupDetails> optLiteralBoolDetails = {{true, {}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(emptySynonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == withStr);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == stModifiesP);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 3
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == stAffects);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "Duplicate synonym clauses: with s1.stmt# = ifs.stmt# > [with c.procName "
      "= p1.procName, ModifiesP(p1, \"x\")] > pattern w (\"x\", _) > [pattern "
      "a (\"x\", \"1\"), pattern a (v, _\"1\"_)] > Affects(s2, 2)") {
    SynonymMap synonymMap = {
        {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
        {"c", DesignEntity::CALL},       {"p1", DesignEntity::PROCEDURE},
        {"w", DesignEntity::WHILE},      {"a", DesignEntity::ASSIGN},
        {"v", DesignEntity::VARIABLE},
    };
    ConditionClause withS1Ifs =
        TestQueryUtil::BuildWithClause(ParamType::ATTRIBUTE_STMT_NUM, "s1",
                                       ParamType::ATTRIBUTE_STMT_NUM, "ifs");
    ConditionClause withCP1 =
        TestQueryUtil::BuildWithClause(ParamType::ATTRIBUTE_PROC_NAME, "c",
                                       ParamType::ATTRIBUTE_PROC_NAME, "p1");
    ConditionClause stModifiesPP1 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::MODIFIES_P, ParamType::SYNONYM, "p1",
        ParamType::NAME_LITERAL, "x");
    ConditionClause pattW = TestQueryUtil::BuildPatternClause(
        {DesignEntity::WHILE, "w"}, ParamType::NAME_LITERAL, "x", {});
    ConditionClause pattAExact = TestQueryUtil::BuildPatternClause(
        {DesignEntity::ASSIGN, "a"}, ParamType::NAME_LITERAL, "x",
        {MatchType::EXACT, "[1]"});
    ConditionClause pattAVSub = TestQueryUtil::BuildPatternClause(
        {DesignEntity::ASSIGN, "a"}, ParamType::SYNONYM, "v",
        {MatchType::SUB_EXPRESSION, "[1]"});
    ConditionClause stAffects = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::SYNONYM, "s2",
        ParamType::INTEGER_LITERAL, "2");
    vector<ConditionClause> givenClauses = {
        withS1Ifs, stModifiesPP1, pattAExact, stAffects,     withCP1,
        pattW,     pattAVSub,     withS1Ifs,  stModifiesPP1, pattAExact,
        stAffects, withCP1,       pattW,      pattAVSub};

    SelectClause givenSelectClause = {{}, SelectType::BOOLEAN, givenClauses};

    // expected
    optional<GroupDetails> optLiteralBoolDetails = {{true, {}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == withS1Ifs);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == withCP1);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == stModifiesPP1);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 3
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == pattW);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 4
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == pattAExact);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == pattAVSub);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 5
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == stAffects);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "Duplicate literal and synonym clauses: with 1 = 1 > pattern a(_, _) > "
      "[pattern ifs(_, _, _), Parent*(ifs, 3)] > Affects(1, 2)") {
    SynonymMap synonymMap = {
        {"a", DesignEntity::ASSIGN},
        {"ifs", DesignEntity::IF},
    };
    ConditionClause withLit = TestQueryUtil::BuildWithClause(
        ParamType::INTEGER_LITERAL, "1", ParamType::INTEGER_LITERAL, "2");
    ConditionClause pattAAny = TestQueryUtil::BuildPatternClause(
        {DesignEntity::ASSIGN, "a"}, ParamType::WILDCARD, "_",
        {MatchType::ANY, "_"});
    ConditionClause pattIfs = TestQueryUtil::BuildPatternClause(
        {DesignEntity::IF, "ifs"}, ParamType::WILDCARD, "_", {});
    ConditionClause stParentTIfs = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::PARENT_T, ParamType::SYNONYM, "ifs",
        ParamType::INTEGER_LITERAL, "3");
    ConditionClause affectsLit = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::INTEGER_LITERAL, "1",
        ParamType::INTEGER_LITERAL, "2");
    vector<ConditionClause> givenClauses = {
        affectsLit, pattIfs, pattIfs,    pattIfs,      withLit,
        pattAAny,   pattIfs, affectsLit, stParentTIfs, pattAAny};
    SelectClause givenSelectClause = {{}, SelectType::BOOLEAN, givenClauses};

    // expected
    optional<GroupDetails> optLiteralBoolDetails = {{true, {}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == withLit);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == pattAAny);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 3
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == stParentTIfs);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == pattIfs);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 4
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable) == affectsLit);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }
}

// ============= Test grouping + group sorting + clause sorting ==============
// === Tests group sorting ===
// prioritise groups with less expensive clauses, then literal groups,
// then higher num of efficient clauses, then groups with less clauses
// === Tests clause sorting ===
// prioritise common synonym using count table,
// then clauses that are not expensive, then clauses whose est size is
// smaller (using pkb and count table), then clauses that are efficient
TEST_CASE("Groups are grouped and sorted in the correct order") {
  PKB* pkb = new PKB();
  SynonymCountsTable emptyCountTable = {};
  pkb->addStmt(DesignEntity::STATEMENT, 1);
  pkb->addStmt(DesignEntity::STATEMENT, 2);
  pkb->addStmt(DesignEntity::STATEMENT, 3);
  pkb->addStmt(DesignEntity::STATEMENT, 4);
  pkb->addStmt(DesignEntity::IF, 5);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addStmt(DesignEntity::IF, 6);
  pkb->addStmt(DesignEntity::CALL, 7);
  pkb->addStmt(DesignEntity::WHILE, 8);
  pkb->insertAt(TableType::VAR_TABLE, "var1");
  pkb->insertAt(TableType::VAR_TABLE, "var2");
  pkb->insertAt(TableType::VAR_TABLE, "var3");
  pkb->insertAt(TableType::PROC_TABLE, "proc1");

  SECTION(
      "(Literals) Select boolean, with 1 = 2 > Follows(1, "
      "2) > ModifiesP(\"x\",\"y\") > Parent*(_, _) > Affects(1, 2) > Next*(_, "
      "_)") {
    SynonymMap emptySynonymMap = {};
    ConditionClause withInt = TestQueryUtil::BuildWithClause(
        ParamType::INTEGER_LITERAL, "1", ParamType::INTEGER_LITERAL, "2");
    ConditionClause stFollows = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::INTEGER_LITERAL, "1",
        ParamType::INTEGER_LITERAL, "2");
    ConditionClause stModifiesP = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::MODIFIES_P, ParamType::NAME_LITERAL, "x",
        ParamType::NAME_LITERAL, "y");
    ConditionClause stParentT = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::PARENT_T, ParamType::WILDCARD, "_",
        ParamType::WILDCARD, "_");
    ConditionClause stAffects = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::INTEGER_LITERAL, "1",
        ParamType::INTEGER_LITERAL, "2");
    ConditionClause stNextT = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::NEXT_T, ParamType::WILDCARD, "_", ParamType::WILDCARD,
        "_");
    vector<ConditionClause> givenClauses = {stFollows, stAffects, stNextT,
                                            stParentT, withInt,   stModifiesP};
    SelectClause givenSelectClause = {{}, SelectType::BOOLEAN, givenClauses};

    // expected
    optional<GroupDetails> optLiteralBoolDetails = {{true, {}}};
    ConditionClause clause1 = withInt;
    // clauses 2 3 4 may be either stFollows, stModifiesP or stParentT,
    // in any order, as they have the same weight
    unordered_set<ConditionClause, CLAUSE_HASH> clauses234 = {
        stFollows, stModifiesP, stParentT};
    unordered_set<ConditionClause, CLAUSE_HASH> clauses56 = {stAffects,
                                                             stNextT};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(emptySynonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == clause1);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    ConditionClause actualClause2 =
        optimizer.GetNextClause(emptyCountTable).value();
    REQUIRE(clauses234.find(actualClause2) != clauses234.end());
    clauses234.erase(actualClause2);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 3
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    ConditionClause actualClause3 =
        optimizer.GetNextClause(emptyCountTable).value();
    REQUIRE(clauses234.find(actualClause3) != clauses234.end());
    clauses234.erase(actualClause3);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 4
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    ConditionClause actualClause4 =
        optimizer.GetNextClause(emptyCountTable).value();
    REQUIRE(clauses234.find(actualClause4) != clauses234.end());
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 5
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    ConditionClause actualClause5 =
        optimizer.GetNextClause(emptyCountTable).value();
    REQUIRE(clauses56.find(actualClause5) != clauses56.end());
    clauses56.erase(actualClause5);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 6
    REQUIRE(optimizer.GetNextGroupDetails() == optLiteralBoolDetails);
    ConditionClause actualClause6 =
        optimizer.GetNextClause(emptyCountTable).value();
    REQUIRE(clauses56.find(actualClause6) != clauses56.end());
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "(No Common Synonyms) Select synonyms, with s1.stmt# = s2.stmt# > "
      "Follows(s3, s4) > Affects(s5, s6)") {
    SynonymMap synonymMap = {
        {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
        {"s3", DesignEntity::STATEMENT}, {"s4", DesignEntity::STATEMENT},
        {"s5", DesignEntity::STATEMENT}, {"s6", DesignEntity::STATEMENT},
    };
    vector<Synonym> selectSynonyms = {
        {DesignEntity::STATEMENT, "s1"},
        {DesignEntity::STATEMENT, "s4"},
    };
    ConditionClause withInt =
        TestQueryUtil::BuildWithClause(ParamType::ATTRIBUTE_STMT_NUM, "s1",
                                       ParamType::ATTRIBUTE_STMT_NUM, "s2");
    ConditionClause stFollows = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "s3", ParamType::SYNONYM,
        "s4");
    ConditionClause stAffects = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::SYNONYM, "s5", ParamType::SYNONYM,
        "s6");
    vector<ConditionClause> givenClauses = {stFollows, stAffects, withInt};
    SelectClause givenSelectClause = {selectSynonyms, SelectType::SYNONYMS,
                                      givenClauses};

    // expected
    optional<GroupDetails> optGroupDetails1 = {
        {false, {{DesignEntity::STATEMENT, "s1"}}}};
    optional<GroupDetails> optGroupDetails2 = {
        {false, {{DesignEntity::STATEMENT, "s4"}}}};
    optional<GroupDetails> optGroupDetails3 = {{true, {}}};
    ConditionClause clause1 = withInt;
    ConditionClause clause2 = stFollows;
    ConditionClause clause3 = stAffects;

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optGroupDetails1);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == clause1);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optGroupDetails2);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == clause2);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 3
    REQUIRE(optimizer.GetNextGroupDetails() == optGroupDetails3);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == clause3);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "(Common Synonyms - Group disjointed synonyms) Select attributes, [with "
      "s1.stmt# = s2.stmt#, Follows(s1, s3)] > [with 5 = ifs.stmt#, "
      "Follows(ifs, 5), pattern ifs (v1, _, _)], ModifiesP(s4, v1)") {
    SynonymMap synonymMap = {
        {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
        {"s3", DesignEntity::STATEMENT}, {"s4", DesignEntity::STATEMENT},
        {"v1", DesignEntity::VARIABLE},  {"ifs", DesignEntity::IF},
    };
    Synonym s1StmtNum = {DesignEntity::STATEMENT, "s1", true,
                         Attribute::STMT_NUM};
    Synonym s3 = {DesignEntity::STATEMENT, "s3"};
    Synonym v1VarName = {DesignEntity::VARIABLE, "v1", true,
                         Attribute::VAR_NAME};
    vector<Synonym> selectSynonyms = {s1StmtNum, s3, v1VarName};

    ConditionClause withS1S2 =
        TestQueryUtil::BuildWithClause(ParamType::ATTRIBUTE_STMT_NUM, "s1",
                                       ParamType::ATTRIBUTE_STMT_NUM, "s2");
    ConditionClause stFollowsS1S3 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "s1", ParamType::SYNONYM,
        "s3");
    ConditionClause stModifiesS4V1 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::MODIFIES_P, ParamType::SYNONYM, "s4",
        ParamType::SYNONYM, "v1");  // 5 * 5
    ConditionClause stFollowsIfs = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "ifs",
        ParamType::INTEGER_LITERAL, "5");
    ConditionClause withIfs = TestQueryUtil::BuildWithClause(
        ParamType::INTEGER_LITERAL, "5", ParamType::ATTRIBUTE_STMT_NUM, "ifs");
    ConditionClause patAssignIfsV1 = TestQueryUtil::BuildPatternClause(
        {DesignEntity::IF, "ifs"}, ParamType::SYNONYM, "v1", {});  // 10
    vector<ConditionClause> givenClauses = {stFollowsIfs, stFollowsS1S3,
                                            withIfs,      patAssignIfsV1,
                                            withS1S2,     stModifiesS4V1};
    SelectClause givenSelectClause = {selectSynonyms, SelectType::SYNONYMS,
                                      givenClauses};

    // expected
    optional<GroupDetails> optGroupDetails1 = {{false, {s1StmtNum, s3}}};
    optional<GroupDetails> optGroupDetails2 = {{false, {v1VarName}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optGroupDetails1);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == withS1S2);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stFollowsS1S3);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optGroupDetails2);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == withIfs);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stFollowsIfs);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == patAssignIfsV1);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stModifiesS4V1);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "(Common Synonyms - Test Clause Sorting) [with s1.stmt# = 2, with "
      "s1.stmt# = s2.stmt#, Follows(s2, 5), Follows(s2, ifs), Follows(s1, s3), "
      "Affects(s1, 5), Affects*(s3, s4)]") {
    SynonymMap synonymMap = {
        {"s1", DesignEntity::STATEMENT}, {"s2", DesignEntity::STATEMENT},
        {"s3", DesignEntity::STATEMENT}, {"s4", DesignEntity::STATEMENT},
        {"ifs", DesignEntity::IF},
    };
    vector<Synonym> selectSynonyms = {};

    ConditionClause withS1 = TestQueryUtil::BuildWithClause(
        ParamType::ATTRIBUTE_STMT_NUM, "s1", ParamType::INTEGER_LITERAL, "2");
    ConditionClause withS1S2 =
        TestQueryUtil::BuildWithClause(ParamType::ATTRIBUTE_STMT_NUM, "s1",
                                       ParamType::ATTRIBUTE_STMT_NUM, "s2");
    ConditionClause stFollowsS2 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "s2",
        ParamType::INTEGER_LITERAL, "5");
    ConditionClause stFollowsS2Ifs = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "s2", ParamType::SYNONYM,
        "ifs");
    ConditionClause stFollowsS1S3 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "s1", ParamType::SYNONYM,
        "s3");
    ConditionClause stAffectsS1 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::SYNONYM, "s1",
        ParamType::INTEGER_LITERAL, "5");
    ConditionClause stAffectsS3S4 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS_T, ParamType::SYNONYM, "s3",
        ParamType::SYNONYM, "s4");
    vector<ConditionClause> givenClauses = {
        stAffectsS1,   stFollowsS2, withS1,        stAffectsS3S4,
        stFollowsS1S3, withS1S2,    stFollowsS2Ifs};
    SelectClause givenSelectClause = {selectSynonyms, SelectType::BOOLEAN,
                                      givenClauses};

    SynonymCountsTable s1Table = {{"s1", 5}};
    SynonymCountsTable s1s2Table = {{"s1", 5}, {"s2", 5}};
    SynonymCountsTable s1s2IfsTable = {{"s1", 5}, {"s2", 5}, {"ifs", 5}};
    SynonymCountsTable s1s2s3IfsTable = {
        {"s1", 5}, {"s2", 5}, {"ifs", 5}, {"s3", 5}};

    // expected
    optional<GroupDetails> optBoolGroupDetails = {{true, {}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == withS1);
    REQUIRE(optimizer.GetNextClause(s1Table).value() == withS1S2);
    REQUIRE(optimizer.GetNextClause(s1s2Table).value() == stFollowsS2);
    REQUIRE(optimizer.GetNextClause(s1s2Table).value() == stFollowsS2Ifs);
    REQUIRE(optimizer.GetNextClause(s1s2IfsTable).value() == stFollowsS1S3);
    REQUIRE(optimizer.GetNextClause(s1s2s3IfsTable).value() == stAffectsS1);
    REQUIRE(optimizer.GetNextClause(s1s2s3IfsTable).value() == stAffectsS3S4);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "(Common Synonyms - Test Clause Sorting with Count Table) [with "
      "ifs.stmt# = n, Follows(n, 5), Follows(ifs, 5)]") {
    SynonymMap synonymMap = {
        {"ifs", DesignEntity::IF},
        {"n", DesignEntity::PROG_LINE},
    };
    vector<Synonym> selectSynonyms = {};

    ConditionClause withIfsN = TestQueryUtil::BuildWithClause(
        ParamType::ATTRIBUTE_STMT_NUM, "ifs", ParamType::SYNONYM, "n");
    ConditionClause stAffectsIfs = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "ifs",
        ParamType::INTEGER_LITERAL, "5");
    ConditionClause stAffectsN = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "n",
        ParamType::INTEGER_LITERAL, "5");
    vector<ConditionClause> givenClauses = {stAffectsN, stAffectsIfs, withIfsN};
    SelectClause givenSelectClause = {selectSynonyms, SelectType::BOOLEAN,
                                      givenClauses};

    SynonymCountsTable ifsNTable = {{"ifs", 2}, {"n", 1}};

    // expected
    optional<GroupDetails> optBoolGroupDetails = {{true, {}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == withIfsN);
    REQUIRE(optimizer.GetNextClause(ifsNTable).value() == stAffectsN);
    REQUIRE(optimizer.GetNextClause(ifsNTable).value() == stAffectsIfs);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "(Literal & Common Synonyms - Test Group Sorting) [with 1 = 2] > "
      "[Follows(5, 6)] > [pattern ifs ('x'), Follows(ifs, s1)] > [Affects(1, "
      "2)] > [with s2.stmt# = 5, Affects(s2, 3)] ") {
    SynonymMap synonymMap = {
        {"ifs", DesignEntity::IF},
        {"s1", DesignEntity::STATEMENT},
        {"s2", DesignEntity::STATEMENT},
    };
    vector<Synonym> selectSynonyms = {};

    ConditionClause withLit = TestQueryUtil::BuildWithClause(
        ParamType::INTEGER_LITERAL, "1", ParamType::INTEGER_LITERAL, "2");
    ConditionClause stFollowsLit = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::INTEGER_LITERAL, "5",
        ParamType::INTEGER_LITERAL, "6");
    ConditionClause pattIfs = TestQueryUtil::BuildPatternClause(
        {DesignEntity::IF, "ifs"}, ParamType::NAME_LITERAL, "x", {});
    ConditionClause stFollowsIfsS1 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::FOLLOWS, ParamType::SYNONYM, "ifs",
        ParamType::SYNONYM, "s1");
    ConditionClause stAffectsLit = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::INTEGER_LITERAL, "1",
        ParamType::INTEGER_LITERAL, "2");
    ConditionClause withS2 = TestQueryUtil::BuildWithClause(
        ParamType::ATTRIBUTE_STMT_NUM, "s2", ParamType::INTEGER_LITERAL, "5");
    ConditionClause stAffectsS2 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::SYNONYM, "s2",
        ParamType::INTEGER_LITERAL, "3");
    vector<ConditionClause> givenClauses = {
        stAffectsS2,    stAffectsLit, withS2,      withLit,
        stFollowsIfsS1, pattIfs,      stFollowsLit};
    SelectClause givenSelectClause = {selectSynonyms, SelectType::BOOLEAN,
                                      givenClauses};

    SynonymCountsTable ifsTable = {{"ifs", 1}};
    SynonymCountsTable s2Table = {{"s2", 1}};

    // expected
    optional<GroupDetails> optBoolGroupDetails = {{true, {}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == withLit);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stFollowsLit);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 3
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == pattIfs);
    REQUIRE(optimizer.GetNextClause(ifsTable).value() == stFollowsIfsS1);
    REQUIRE_FALSE(optimizer.GetNextClause(ifsTable).has_value());
    // group 4
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stAffectsLit);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 5
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == withS2);
    REQUIRE(optimizer.GetNextClause(s2Table).value() == stAffectsS2);
    REQUIRE_FALSE(optimizer.GetNextClause(s2Table).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }

  SECTION(
      "(Literal & Common Synonyms - Test Expensive Clauses) [Next(1, 2)] > "
      "[Affects(_, _)] > "
      "[Affects(s1, s2)] > [NextBip*(s3, 4), Next*(s3, s4)]") {
    SynonymMap synonymMap = {
        {"s1", DesignEntity::STATEMENT},
        {"s2", DesignEntity::STATEMENT},
        {"s3", DesignEntity::STATEMENT},
        {"s4", DesignEntity::STATEMENT},
    };
    vector<Synonym> selectSynonyms = {};

    // Next and Next* is not expensive
    ConditionClause stNextLit = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::NEXT, ParamType::INTEGER_LITERAL, "1",
        ParamType::INTEGER_LITERAL, "2");
    ConditionClause stAffectsLit = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::WILDCARD, "_",
        ParamType::WILDCARD, "_");
    ConditionClause stAffectsS1S2 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::AFFECTS, ParamType::SYNONYM, "s1", ParamType::SYNONYM,
        "s2");
    ConditionClause stNextBipTS3 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::NEXT_BIP_T, ParamType::SYNONYM, "s3",
        ParamType::INTEGER_LITERAL, "4");
    ConditionClause stNextTS3S4 = TestQueryUtil::BuildSuchThatClause(
        RelationshipType::NEXT_T, ParamType::SYNONYM, "s3", ParamType::SYNONYM,
        "s4");

    vector<ConditionClause> givenClauses = {
        stNextTS3S4, stAffectsS1S2, stNextBipTS3, stAffectsLit, stNextLit};
    SelectClause givenSelectClause = {selectSynonyms, SelectType::BOOLEAN,
                                      givenClauses};

    SynonymCountsTable s3Table = {{"s3", 1}};

    // expected
    optional<GroupDetails> optBoolGroupDetails = {{true, {}}};

    // actual + test
    QueryOptimizer optimizer(pkb);
    optimizer.PreprocessClauses(synonymMap, givenSelectClause);
    // group 1
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stNextLit);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 2
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stAffectsLit);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 3
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stAffectsS1S2);
    REQUIRE_FALSE(optimizer.GetNextClause(emptyCountTable).has_value());
    // group 4
    REQUIRE(optimizer.GetNextGroupDetails() == optBoolGroupDetails);
    REQUIRE(optimizer.GetNextClause(emptyCountTable).value() == stNextBipTS3);
    REQUIRE(optimizer.GetNextClause(s3Table).value() == stNextTS3S4);
    REQUIRE_FALSE(optimizer.GetNextClause(s3Table).has_value());
    // optional empty group
    REQUIRE_FALSE(optimizer.GetNextGroupDetails().has_value());
  }
}
