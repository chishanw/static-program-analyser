#include <Common/Tokenizer.h>
#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>

#include "catch.hpp"

using namespace std;

TEST_CASE("Whole frontend simple test") {
  string source =
      "procedure a {\n"                    // procIdx 0
      "  y = 0;\n"                         // stmt 1, yVarIndex 0
      "  while ((x != 0) || (y > 0)) {\n"  // stmt 2
      "    cenX = cenX + x + y;\n"         // stmt 3 cenXVarIndex 1 XVarIndex 2
      "    if (cenX > 100) then {\n"       // stmt 4
      "      call b; }\n"                  // stmt 5
      "    else {\n"
      "      cenX = cenX - 1; } } }\n"  // stmt 6
      "procedure b {\n"                 // procIdx 1
      "  read x;\n"                     // stmt 7
      "  call c;\n"                     // stmt 8
      "  x = x + 1; }\n"                // stmt 9
      "procedure c {\n"                 // procIdx 2
      "  print y; }\n"                  // stmt 10
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(source));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  SECTION("General info") {
    REQUIRE(pkb->getAllStmts(DesignEntity::STATEMENT) ==
            SetOfStmts({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
    REQUIRE(pkb->getAllStmts(DesignEntity::READ) == SetOfStmts({7}));
    REQUIRE(pkb->getAllStmts(DesignEntity::PRINT) == SetOfStmts({10}));
    REQUIRE(pkb->getAllStmts(DesignEntity::CALL) == SetOfStmts({5, 8}));
    REQUIRE(pkb->getAllStmts(DesignEntity::WHILE) == SetOfStmts({2}));
    REQUIRE(pkb->getAllStmts(DesignEntity::IF) == SetOfStmts({4}));
    REQUIRE(pkb->getAllStmts(DesignEntity::ASSIGN) == SetOfStmts({1, 3, 6, 9}));
  }

  SECTION("Calls/* extraction") {
    REQUIRE(pkb->isStmt(DesignEntity::CALL, 5));
    REQUIRE(pkb->isStmt(DesignEntity::CALL, 8));
    // invalid query
    REQUIRE(!pkb->isStmt(DesignEntity::CALL, 1));

    REQUIRE(pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "a",
                      TableType::PROC_TABLE, "b"));
    REQUIRE(pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "b",
                      TableType::PROC_TABLE, "c"));
    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "a",
                       TableType::PROC_TABLE, "c"));
    REQUIRE(!pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "a",
                       TableType::PROC_TABLE, "B"));
    REQUIRE(!pkb->isRs(RelationshipType::CALLS, TableType::PROC_TABLE, "A",
                       TableType::PROC_TABLE, "B"));

    REQUIRE(pkb->getRight(RelationshipType::CALLS, TableType::PROC_TABLE,
                          "a") == SetOfStmts({1}));
    REQUIRE(pkb->getRight(RelationshipType::CALLS, TableType::PROC_TABLE,
                          "b") == SetOfStmts({2}));
    REQUIRE(pkb->getRight(RelationshipType::CALLS, TableType::PROC_TABLE,
                          "c") == SetOfStmts({}));
    // invalid query
    REQUIRE(pkb->getRight(RelationshipType::CALLS, TableType::PROC_TABLE,
                          "A") == SetOfStmts({}));

    REQUIRE(pkb->getLeft(RelationshipType::CALLS, TableType::PROC_TABLE, "a") ==
            SetOfStmts({}));
    REQUIRE(pkb->getLeft(RelationshipType::CALLS, TableType::PROC_TABLE, "b") ==
            SetOfStmts({0}));
    REQUIRE(pkb->getLeft(RelationshipType::CALLS, TableType::PROC_TABLE, "c") ==
            SetOfStmts({1}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::CALLS, TableType::PROC_TABLE, "A") ==
            SetOfStmts({}));

    vector<ProcIdx> calledByA({1});
    pair<ProcIdx, vector<ProcIdx>> aRes(0, calledByA);
    vector<ProcIdx> calledByB({2});
    pair<ProcIdx, vector<ProcIdx>> bRes(1, calledByB);
    vector<pair<ProcIdx, vector<ProcIdx>>> result({aRes, bRes});

    REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                      TableType::PROC_TABLE, "b"));
    REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                      TableType::PROC_TABLE, "c"));
    REQUIRE(pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "b",
                      TableType::PROC_TABLE, "c"));
    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "a",
                       TableType::PROC_TABLE, "B"));
    REQUIRE(!pkb->isRs(RelationshipType::CALLS_T, TableType::PROC_TABLE, "A",
                       TableType::PROC_TABLE, "B"));

    REQUIRE(pkb->getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                          "a") == SetOfStmts({1, 2}));
    REQUIRE(pkb->getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                          "b") == SetOfStmts({2}));
    REQUIRE(pkb->getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                          "c") == SetOfStmts({}));
    // invalid query
    REQUIRE(pkb->getRight(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                          "A") == SetOfStmts({}));

    REQUIRE(pkb->getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                         "a") == SetOfStmts({}));
    REQUIRE(pkb->getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                         "b") == SetOfStmts({0}));
    REQUIRE(pkb->getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                         "c") == SetOfStmts({0, 1}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::CALLS_T, TableType::PROC_TABLE,
                         "A") == SetOfStmts({}));

    vector<ProcIdx> calledTByA({1, 2});
    pair<ProcIdx, vector<ProcIdx>> aTRes(0, calledTByA);
    vector<pair<ProcIdx, vector<ProcIdx>>> resultT({aTRes, bRes});
  }

  SECTION("Follows/* extraction") {
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 1, 2));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 7, 8));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 7, 9));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 8, 9));
    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::FOLLOWS_T, 2, 3));

    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 1) == SetOfStmts({2}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 3) == SetOfStmts({4}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 7) == SetOfStmts({8}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 8) == SetOfStmts({9}));
    // invalid query
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 11) == SetOfStmts({}));

    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 2) == SetOfStmts({1}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 9) == SetOfStmts({8}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 8) == SetOfStmts({7}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 1) == SetOfStmts({}));

    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 1) == SetOfStmts({2}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 3) == SetOfStmts({4}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 7) ==
            SetOfStmts({8, 9}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 8) == SetOfStmts({9}));
    // invalid query
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 11) == SetOfStmts({}));

    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 2) == SetOfStmts({1}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 4) == SetOfStmts({3}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 9) == SetOfStmts({7, 8}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 8) == SetOfStmts({7}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 1) == SetOfStmts({}));

    auto output =
        pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::BOTH);
    auto answer = unordered_set<vector<int>, VectorHash>(
        {ListOfStmtNos({1, 2}), ListOfStmtNos({3, 4}), ListOfStmtNos({7, 8}),
         ListOfStmtNos({7, 9}), ListOfStmtNos({8, 9})});
    REQUIRE(output == answer);

    auto outputT =
        pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::BOTH);
    unordered_set<vector<int>, VectorHash> answerT(
        {ListOfStmtNos({1, 2}), ListOfStmtNos({3, 4}), ListOfStmtNos({7, 8}),
         ListOfStmtNos({7, 9}), ListOfStmtNos({8, 9})});
    REQUIRE(outputT == answerT);
  }

  SECTION("ModifiesS/P extraction") {
    REQUIRE(
        pkb->isRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "y"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE,
                      "cenX"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_S, 6, TableType::VAR_TABLE,
                      "cenX"));
    REQUIRE(
        pkb->isRs(RelationshipType::MODIFIES_S, 7, TableType::VAR_TABLE, "x"));
    REQUIRE(
        pkb->isRs(RelationshipType::MODIFIES_S, 9, TableType::VAR_TABLE, "x"));
    // invalid query
    REQUIRE(
        !pkb->isRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "x"));
    REQUIRE(
        !pkb->isRs(RelationshipType::MODIFIES_S, 1, TableType::VAR_TABLE, "Y"));
    REQUIRE(
        !pkb->isRs(RelationshipType::MODIFIES_S, 3, TableType::VAR_TABLE, "x"));
    REQUIRE(!pkb->isRs(RelationshipType::MODIFIES_S, 11, TableType::VAR_TABLE,
                       "cenX"));

    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "a",
                      TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "a",
                      TableType::VAR_TABLE, "y"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "a",
                      TableType::VAR_TABLE, "cenX"));
    REQUIRE(pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "b",
                      TableType::VAR_TABLE, "x"));
    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "b",
                       TableType::VAR_TABLE, "y"));
    REQUIRE(!pkb->isRs(RelationshipType::MODIFIES_P, TableType::PROC_TABLE, "c",
                       TableType::VAR_TABLE, "y"));

    REQUIRE(pkb->getRight(RelationshipType::MODIFIES_S, 1) ==
            unordered_set<VarIdx>({0}));
    REQUIRE(pkb->getRight(RelationshipType::MODIFIES_S, 6) ==
            unordered_set<VarIdx>({1}));
    REQUIRE(pkb->getRight(RelationshipType::MODIFIES_S, 7) ==
            unordered_set<VarIdx>({2}));
    // invalid query
    REQUIRE(pkb->getRight(RelationshipType::MODIFIES_S, 10) ==
            unordered_set<VarIdx>({}));
    REQUIRE(pkb->getRight(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                          "a") == unordered_set<VarIdx>({0, 1, 2}));
    REQUIRE(pkb->getRight(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                          "b") == unordered_set<VarIdx>({2}));
    REQUIRE(pkb->getRight(RelationshipType::MODIFIES_P, TableType::PROC_TABLE,
                          "c") == unordered_set<VarIdx>({}));

    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "x") == unordered_set<StmtNo>({2, 4, 5, 7, 9}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "y") == unordered_set<StmtNo>({1}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "cenX") == unordered_set<StmtNo>({2, 3, 4, 6}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "X") == unordered_set<StmtNo>({}));

    auto outputS =
        pkb->getMappings(RelationshipType::MODIFIES_S, ParamPosition::BOTH);

    unordered_set<vector<int>, VectorHash> answerS({
        vector({1, 0}),
        vector({2, 1}),
        vector({2, 2}),
        vector({3, 1}),
        vector({4, 2}),
        vector({4, 1}),
        vector({5, 2}),
        vector({6, 1}),
        vector({7, 2}),
        vector({9, 2}),
    });
    REQUIRE(outputS == answerS);
  }

  SECTION("UsesS/P extraction") {
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "y"));
    REQUIRE(
        pkb->isRs(RelationshipType::USES_S, 2, TableType::VAR_TABLE, "cenX"));
    REQUIRE(
        pkb->isRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "cenX"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 3, TableType::VAR_TABLE, "y"));
    REQUIRE(
        pkb->isRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "cenX"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "y"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 4, TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 5, TableType::VAR_TABLE, "y"));
    REQUIRE(
        pkb->isRs(RelationshipType::USES_S, 6, TableType::VAR_TABLE, "cenX"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 8, TableType::VAR_TABLE, "y"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 9, TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::USES_S, 10, TableType::VAR_TABLE, "y"));

    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::USES_S, 1, TableType::VAR_TABLE, "x"));
    REQUIRE(!pkb->isRs(RelationshipType::USES_S, 7, TableType::VAR_TABLE, "x"));

    REQUIRE(pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "a",
                      TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "a",
                      TableType::VAR_TABLE, "y"));
    REQUIRE(pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "a",
                      TableType::VAR_TABLE, "cenX"));
    REQUIRE(pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "b",
                      TableType::VAR_TABLE, "x"));
    REQUIRE(pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "b",
                      TableType::VAR_TABLE, "y"));
    REQUIRE(pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "c",
                      TableType::VAR_TABLE, "y"));

    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "a",
                       TableType::VAR_TABLE, "cenY"));
    REQUIRE(!pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "b",
                       TableType::VAR_TABLE, "cenX"));
    REQUIRE(!pkb->isRs(RelationshipType::USES_P, TableType::PROC_TABLE, "c",
                       TableType::VAR_TABLE, "x"));

    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE, "x") ==
            unordered_set<StmtNo>({2, 3, 4, 5, 9}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE, "y") ==
            unordered_set<StmtNo>({2, 3, 4, 5, 8, 10}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "cenX") == unordered_set<StmtNo>({2, 3, 4, 6}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE, "X") ==
            unordered_set<StmtNo>({}));

    REQUIRE(pkb->getRight(RelationshipType::USES_S, 2) ==
            unordered_set<VarIdx>({0, 1, 2}));
    REQUIRE(pkb->getRight(RelationshipType::USES_S, 3) ==
            unordered_set<VarIdx>({0, 1, 2}));
    REQUIRE(pkb->getRight(RelationshipType::USES_P, TableType::PROC_TABLE,
                          "a") == unordered_set<VarIdx>({0, 1, 2}));
    REQUIRE(pkb->getRight(RelationshipType::USES_P, TableType::PROC_TABLE,
                          "b") == unordered_set<VarIdx>({0, 2}));
    REQUIRE(pkb->getRight(RelationshipType::USES_P, TableType::PROC_TABLE,
                          "c") == unordered_set<VarIdx>({0}));
  }

  SECTION("Parent/* extraction") {
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 2, 3));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 2, 4));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 4, 5));
    REQUIRE(pkb->isRs(RelationshipType::PARENT, 4, 6));
    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::PARENT, 2, 6));
    REQUIRE(!pkb->isRs(RelationshipType::PARENT, 4, 7));

    REQUIRE(pkb->isRs(RelationshipType::PARENT_T, 2, 3));
    REQUIRE(pkb->isRs(RelationshipType::PARENT_T, 2, 4));
    REQUIRE(pkb->isRs(RelationshipType::PARENT_T, 2, 5));
    REQUIRE(pkb->isRs(RelationshipType::PARENT_T, 2, 6));
    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::PARENT_T, 2, 7));

    auto output =
        pkb->getMappings(RelationshipType::PARENT, ParamPosition::BOTH);
    unordered_set<vector<int>, VectorHash> answer({
        ListOfStmtNos({2, 3}),
        ListOfStmtNos({2, 4}),
        ListOfStmtNos({4, 5}),
        ListOfStmtNos({4, 6}),
    });
    REQUIRE(output == answer);
  }

  SECTION("Next/* extraction") {
    REQUIRE(pkb->getRight(RelationshipType::NEXT, 1) == unordered_set<int>{2});
    REQUIRE(pkb->getRight(RelationshipType::NEXT, 2) == unordered_set<int>{3});
    REQUIRE(pkb->getRight(RelationshipType::NEXT, 3) == unordered_set<int>{4});
    REQUIRE(pkb->getRight(RelationshipType::NEXT, 4) ==
            unordered_set<int>{5, 6});
    REQUIRE(pkb->getRight(RelationshipType::NEXT, 5) == unordered_set<int>{2});
    REQUIRE(pkb->getRight(RelationshipType::NEXT, 6) == unordered_set<int>{2});

    SetOfStmtLists output =
        pkb->getMappings(RelationshipType::NEXT, ParamPosition::BOTH);
    SetOfStmtLists answer({
        {1, 2},
        {2, 3},
        {3, 4},
        {4, 5},
        {4, 6},
        {5, 2},
        {6, 2},
        {7, 8},
        {8, 9},
    });
    REQUIRE(output == answer);
  }
}
