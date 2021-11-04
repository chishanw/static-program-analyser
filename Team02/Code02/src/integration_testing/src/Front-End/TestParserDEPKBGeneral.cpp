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
    REQUIRE(pkb->getAllCallStmts() == SetOfStmts({5, 8}));
    REQUIRE(pkb->getAllStmts(DesignEntity::WHILE) == SetOfStmts({2}));
    REQUIRE(pkb->getAllStmts(DesignEntity::IF) == SetOfStmts({4}));
    REQUIRE(pkb->getAllStmts(DesignEntity::ASSIGN) ==
            SetOfStmts({1, 3, 6, 9}));
  }

  SECTION("Calls/* extraction") {
    REQUIRE(pkb->isCallStmt(5));
    REQUIRE(pkb->isCallStmt(8));
    // invalid query
    REQUIRE(!pkb->isCallStmt(1));

    REQUIRE(pkb->isCalls("a", "b"));
    REQUIRE(pkb->isCalls("b", "c"));
    // invalid query
    REQUIRE(!pkb->isCalls("a", "c"));
    REQUIRE(!pkb->isCalls("a", "B"));
    REQUIRE(!pkb->isCalls("A", "B"));

    REQUIRE(pkb->getProcsCalledBy("a") == SetOfStmts({1}));
    REQUIRE(pkb->getProcsCalledBy("b") == SetOfStmts({2}));
    REQUIRE(pkb->getProcsCalledBy("c") == SetOfStmts({}));
    // invalid query
    REQUIRE(pkb->getProcsCalledBy("A") == SetOfStmts({}));

    REQUIRE(pkb->getCallerProcs("a") == SetOfStmts({}));
    REQUIRE(pkb->getCallerProcs("b") == SetOfStmts({0}));
    REQUIRE(pkb->getCallerProcs("c") == SetOfStmts({1}));
    // invalid query
    REQUIRE(pkb->getCallerProcs("A") == SetOfStmts({}));

    vector<PROC_IDX> calledByA({1});
    pair<PROC_IDX, vector<PROC_IDX>> aRes(0, calledByA);
    vector<PROC_IDX> calledByB({2});
    pair<PROC_IDX, vector<PROC_IDX>> bRes(1, calledByB);
    vector<pair<PROC_IDX, vector<PROC_IDX>>> result({aRes, bRes});
    vector<pair<PROC_IDX, vector<PROC_IDX>>> output = pkb->getAllCallsPairs();
    REQUIRE(
        set<pair<PROC_IDX, vector<PROC_IDX>>>(output.begin(), output.end()) ==
        set<pair<PROC_IDX, vector<PROC_IDX>>>(result.begin(), result.end()));

    REQUIRE(pkb->isCallsT("a", "b"));
    REQUIRE(pkb->isCallsT("a", "c"));
    REQUIRE(pkb->isCallsT("b", "c"));
    // invalid query
    REQUIRE(!pkb->isCallsT("a", "B"));
    REQUIRE(!pkb->isCallsT("A", "B"));

    REQUIRE(pkb->getProcsCalledTBy("a") == SetOfStmts({1, 2}));
    REQUIRE(pkb->getProcsCalledTBy("b") == SetOfStmts({2}));
    REQUIRE(pkb->getProcsCalledTBy("c") == SetOfStmts({}));
    // invalid query
    REQUIRE(pkb->getProcsCalledTBy("A") == SetOfStmts({}));

    REQUIRE(pkb->getCallerTProcs("a") == SetOfStmts({}));
    REQUIRE(pkb->getCallerTProcs("b") == SetOfStmts({0}));
    REQUIRE(pkb->getCallerTProcs("c") == SetOfStmts({0, 1}));
    // invalid query
    REQUIRE(pkb->getCallerTProcs("A") == SetOfStmts({}));

    vector<PROC_IDX> calledTByA({1, 2});
    pair<PROC_IDX, vector<PROC_IDX>> aTRes(0, calledTByA);
    vector<pair<PROC_IDX, vector<PROC_IDX>>> resultT({aTRes, bRes});
    vector<pair<PROC_IDX, vector<PROC_IDX>>> outputT = pkb->getAllCallsTPairs();
    REQUIRE(
        set<pair<PROC_IDX, vector<PROC_IDX>>>(outputT.begin(), outputT.end()) ==
        set<pair<PROC_IDX, vector<PROC_IDX>>>(resultT.begin(), resultT.end()));
  }

  SECTION("Follows/* extraction") {
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 1, 2));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 7, 8));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 7, 9));
    REQUIRE(pkb->isRs(RelationshipType::FOLLOWS_T, 8, 9));
    // invalid query
    REQUIRE(!pkb->isRs(RelationshipType::FOLLOWS_T, 2, 3));

    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 1) ==
            SetOfStmts({2}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 3) ==
            SetOfStmts({4}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 7) ==
            SetOfStmts({8}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 8) ==
            SetOfStmts({9}));
    // invalid query
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS, 11) ==
            SetOfStmts({}));

    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 2) ==
            SetOfStmts({1}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 9) ==
            SetOfStmts({8}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 8) ==
            SetOfStmts({7}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS, 1) ==
            SetOfStmts({}));

    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 1) ==
            SetOfStmts({2}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 3) ==
            SetOfStmts({4}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 7) ==
            SetOfStmts({8, 9}));
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 8) ==
            SetOfStmts({9}));
    // invalid query
    REQUIRE(pkb->getRight(RelationshipType::FOLLOWS_T, 11) ==
            SetOfStmts({}));

    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 2) ==
            SetOfStmts({1}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 4) ==
            SetOfStmts({3}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 9) ==
            SetOfStmts({7, 8}));
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 8) ==
            SetOfStmts({7}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::FOLLOWS_T, 1) ==
            SetOfStmts({}));

    auto output =
        pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::BOTH);
    auto answer = unordered_set<vector<int>, VectorHash>(
        {ListOfStmtNos({1, 2}), ListOfStmtNos({3, 4}), ListOfStmtNos({7, 8}),
         ListOfStmtNos({7, 9}), ListOfStmtNos({8, 9})});
    // REQUIRE(set<LIST_STMT_NO>(output.begin(), output.end()) ==
    //    set<LIST_STMT_NO>(answer.begin(), answer.end()));
    REQUIRE(output == answer);

    auto outputT =
        pkb->getMappings(RelationshipType::FOLLOWS_T, ParamPosition::BOTH);
    unordered_set<vector<int>, VectorHash> answerT(
        {ListOfStmtNos({1, 2}), ListOfStmtNos({3, 4}), ListOfStmtNos({7, 8}),
         ListOfStmtNos({7, 9}), ListOfStmtNos({8, 9})});
    // REQUIRE(set< pair<STMT_NO, LIST_STMT_NO>>(outputT.begin(), outputT.end())
    // ==
    //    set<pair<STMT_NO, LIST_STMT_NO>>(answerT.begin(), answerT.end()));
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
                         "x") == unordered_set<STMT_NO>({2, 4, 5, 7, 9}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "y") == unordered_set<STMT_NO>({1}));
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "cenX") == unordered_set<STMT_NO>({2, 3, 4, 6}));
    // invalid query
    REQUIRE(pkb->getLeft(RelationshipType::MODIFIES_S, TableType::VAR_TABLE,
                         "X") == unordered_set<STMT_NO>({}));

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
            unordered_set<STMT_NO>({2, 3, 4, 5, 9}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE, "y") ==
            unordered_set<STMT_NO>({2, 3, 4, 5, 8, 10}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE,
                         "cenX") == unordered_set<STMT_NO>({2, 3, 4, 6}));
    REQUIRE(pkb->getLeft(RelationshipType::USES_S, TableType::VAR_TABLE, "X") ==
            unordered_set<STMT_NO>({}));

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

    // vector<pair<STMT_NO, LIST_STMT_NO>> output = pkb->
    //  getAllParentsStmtPairs();
    // vector<pair<STMT_NO, LIST_STMT_NO>> answer({
    //    pair(2, LIST_STMT_NO({ 3, 4 })),
    //    pair(4, LIST_STMT_NO({ 5, 6 })),
    //  });

    // REQUIRE(set< pair<STMT_NO, LIST_STMT_NO>>(output.begin(), output.end())
    // ==
    //  set<pair<STMT_NO, LIST_STMT_NO>>(answer.begin(), answer.end()));

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
