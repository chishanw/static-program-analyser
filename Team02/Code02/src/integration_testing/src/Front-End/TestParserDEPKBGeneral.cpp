#include <DesignExtractor/DesignExtractor.h>
#include <PKB/PKB.h>
#include <Parser/Parser.h>
#include <Common/Tokenizer.h>

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
      "      cenX = cenX - 1; } } }\n"     // stmt 6
      "procedure b {\n"                    // procIdx 1
      "  read x;\n"                        // stmt 7
      "  call c;\n"                        // stmt 8
      "  x = x + 1; }\n"                   // stmt 9
      "procedure c {\n"                    // procIdx 2
      "  print y; }\n"                     // stmt 10
      "\n";

  ProgramAST* ast = Parser().Parse(Tokenizer::TokenizeProgramString(source));
  PKB* pkb = new PKB();
  DesignExtractor de = DesignExtractor(pkb);
  de.Extract(ast);

  SECTION("General info") {
    REQUIRE(pkb->getAllStmts() == UNO_SET_OF_STMT_NO({ 1, 2, 3, 4, 5, 6, 7, 8,
        9, 10 }));
    REQUIRE(pkb->getAllReadStmts() == UNO_SET_OF_STMT_NO({ 7 }));
    REQUIRE(pkb->getAllPrintStmts() == UNO_SET_OF_STMT_NO({ 10 }));
    REQUIRE(pkb->getAllCallStmts() == UNO_SET_OF_STMT_NO({ 5, 8 }));
    REQUIRE(pkb->getAllWhileStmts() == UNO_SET_OF_STMT_NO({ 2 }));
    REQUIRE(pkb->getAllIfStmts() == UNO_SET_OF_STMT_NO({ 4 }));
    REQUIRE(pkb->getAllAssignStmts() == UNO_SET_OF_STMT_NO({ 1, 3, 6, 9 }));
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

    REQUIRE(pkb->getProcsCalledBy("a") == UNO_SET_OF_STMT_NO({ 1 }));
    REQUIRE(pkb->getProcsCalledBy("b") == UNO_SET_OF_STMT_NO({ 2 }));
    REQUIRE(pkb->getProcsCalledBy("c") == UNO_SET_OF_STMT_NO({}));
    // invalid query
    REQUIRE(pkb->getProcsCalledBy("A") == UNO_SET_OF_STMT_NO({}));

    REQUIRE(pkb->getCallerProcs("a") == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getCallerProcs("b") == UNO_SET_OF_STMT_NO({ 0 }));
    REQUIRE(pkb->getCallerProcs("c") == UNO_SET_OF_STMT_NO({ 1 }));
    // invalid query
    REQUIRE(pkb->getCallerProcs("A") == UNO_SET_OF_STMT_NO({}));

    vector<PROC_IDX> calledByA({ 1 });
    pair<PROC_IDX, vector<PROC_IDX>> aRes(0, calledByA);
    vector<PROC_IDX> calledByB({ 2 });
    pair<PROC_IDX, vector<PROC_IDX>> bRes(1, calledByB);
    vector<pair<PROC_IDX, vector<PROC_IDX>>> result({ aRes, bRes });
    vector<pair<PROC_IDX, vector<PROC_IDX>>> output = pkb->getAllCallsPairs();
    REQUIRE(set<pair<PROC_IDX, vector<PROC_IDX>>>(output.begin(), output.end())
        == set<pair<PROC_IDX, vector<PROC_IDX>>>(result.begin(), result.end()));

    REQUIRE(pkb->isCallsT("a", "b"));
    REQUIRE(pkb->isCallsT("a", "c"));
    REQUIRE(pkb->isCallsT("b", "c"));
    // invalid query
    REQUIRE(!pkb->isCallsT("a", "B"));
    REQUIRE(!pkb->isCallsT("A", "B"));

    REQUIRE(pkb->getProcsCalledTBy("a") == UNO_SET_OF_STMT_NO({ 1, 2 }));
    REQUIRE(pkb->getProcsCalledTBy("b") == UNO_SET_OF_STMT_NO({ 2 }));
    REQUIRE(pkb->getProcsCalledTBy("c") == UNO_SET_OF_STMT_NO({}));
    // invalid query
    REQUIRE(pkb->getProcsCalledTBy("A") == UNO_SET_OF_STMT_NO({}));

    REQUIRE(pkb->getCallerTProcs("a") == UNO_SET_OF_STMT_NO({}));
    REQUIRE(pkb->getCallerTProcs("b") == UNO_SET_OF_STMT_NO({ 0 }));
    REQUIRE(pkb->getCallerTProcs("c") == UNO_SET_OF_STMT_NO({ 0, 1 }));
    // invalid query
    REQUIRE(pkb->getCallerTProcs("A") == UNO_SET_OF_STMT_NO({}));

    vector<PROC_IDX> calledTByA({ 1, 2 });
    pair<PROC_IDX, vector<PROC_IDX>> aTRes(0, calledTByA);
    vector<pair<PROC_IDX, vector<PROC_IDX>>> resultT({ aTRes, bRes });
    vector<pair<PROC_IDX, vector<PROC_IDX>>> outputT = pkb->getAllCallsTPairs();
    REQUIRE(set<pair<PROC_IDX, vector<PROC_IDX>>>(outputT.begin(),
        outputT.end()) == set<pair<PROC_IDX, vector<PROC_IDX>>>(resultT.begin(),
        resultT.end()));
  }

  SECTION("Follows/* extraction") {
    REQUIRE(pkb->isFollowsT(1, 2));
    REQUIRE(pkb->isFollowsT(7, 8));
    REQUIRE(pkb->isFollowsT(7, 9));
    REQUIRE(pkb->isFollowsT(8, 9));
    // invalid query
    REQUIRE(!pkb->isFollowsT(2, 3));

    REQUIRE(pkb->getFollows(1) == 2);
    REQUIRE(pkb->getFollows(3) == 4);
    REQUIRE(pkb->getFollows(7) == 8);
    REQUIRE(pkb->getFollows(8) == 9);
    // invalid query
    REQUIRE(pkb->getFollows(11) == -1);

    REQUIRE(pkb->getFollowedBy(2) == 1);
    REQUIRE(pkb->getFollowedBy(9) == 8);
    REQUIRE(pkb->getFollowedBy(8) == 7);
    // invalid query
    REQUIRE(pkb->getFollowedBy(1) == -1);

    REQUIRE(pkb->getFollowsT(1) == UNO_SET_OF_STMT_NO({ 2 }));
    REQUIRE(pkb->getFollowsT(3) == UNO_SET_OF_STMT_NO({ 4 }));
    REQUIRE(pkb->getFollowsT(7) == UNO_SET_OF_STMT_NO({ 8, 9 }));
    REQUIRE(pkb->getFollowsT(8) == UNO_SET_OF_STMT_NO({ 9 }));
    // invalid query
    REQUIRE(pkb->getFollowsT(11) == UNO_SET_OF_STMT_NO({}));

    REQUIRE(pkb->getFollowedTBy(2) == UNO_SET_OF_STMT_NO({ 1 }));
    REQUIRE(pkb->getFollowedTBy(4) == UNO_SET_OF_STMT_NO({ 3 }));
    REQUIRE(pkb->getFollowedTBy(9) == UNO_SET_OF_STMT_NO({ 7, 8 }));
    REQUIRE(pkb->getFollowedTBy(8) == UNO_SET_OF_STMT_NO({ 7 }));
    // invalid query
    REQUIRE(pkb->getFollowedTBy(1) == UNO_SET_OF_STMT_NO({}));

    vector<LIST_STMT_NO> output = pkb->getAllFollowsStmtPairs();
    vector<LIST_STMT_NO> answer = vector({
        LIST_STMT_NO({ 1, 2 }),
        LIST_STMT_NO({ 3, 4 }),
        LIST_STMT_NO({ 7, 8 }),
        LIST_STMT_NO({ 8, 9 })
    });
    REQUIRE(set<LIST_STMT_NO>(output.begin(), output.end()) ==
        set<LIST_STMT_NO>(answer.begin(), answer.end()));

    vector<pair<STMT_NO, LIST_STMT_NO>> outputT = pkb->
        getAllFollowsTStmtPairs();
    vector<pair<STMT_NO, LIST_STMT_NO>> answerT({
        pair(1, LIST_STMT_NO({ 2 })),
        pair(3, LIST_STMT_NO({ 4 })),
        pair(7, LIST_STMT_NO({ 8, 9 })),
        pair(8, LIST_STMT_NO({ 9 }))
    });
    REQUIRE(set< pair<STMT_NO, LIST_STMT_NO>>(outputT.begin(), outputT.end()) ==
        set<pair<STMT_NO, LIST_STMT_NO>>(answerT.begin(), answerT.end()));
  }

  SECTION("ModifiesS/P extraction") {
    REQUIRE(pkb->isModifiesS(1, "y"));
    REQUIRE(pkb->isModifiesS(3, "cenX"));
    REQUIRE(pkb->isModifiesS(6, "cenX"));
    REQUIRE(pkb->isModifiesS(7, "x"));
    REQUIRE(pkb->isModifiesS(9, "x"));
    // invalid query
    REQUIRE(!pkb->isModifiesS(1, "x"));
    REQUIRE(!pkb->isModifiesS(1, "Y"));
    REQUIRE(!pkb->isModifiesS(3, "x"));
    REQUIRE(!pkb->isModifiesS(11, "cenX"));

    REQUIRE(pkb->isModifiesP("a", "x"));
    REQUIRE(pkb->isModifiesP("a", "y"));
    REQUIRE(pkb->isModifiesP("a", "cenX"));
    REQUIRE(pkb->isModifiesP("b", "x"));
    // invalid query
    REQUIRE(!pkb->isModifiesP("b", "y"));
    REQUIRE(!pkb->isModifiesP("c", "y"));

    REQUIRE(pkb->getVarsModifiedS(1) == unordered_set<VAR_IDX>({ 0 }));
    REQUIRE(pkb->getVarsModifiedS(6) == unordered_set<VAR_IDX>({ 1 }));
    REQUIRE(pkb->getVarsModifiedS(7) == unordered_set<VAR_IDX>({ 2 }));
    // invalid query
    REQUIRE(pkb->getVarsModifiedS(10) == unordered_set<VAR_IDX>({}));
    REQUIRE(pkb->getVarsModifiedP("a") == unordered_set<VAR_IDX>({0, 1, 2}));
    REQUIRE(pkb->getVarsModifiedP("b") == unordered_set<VAR_IDX>({ 2 }));
    REQUIRE(pkb->getVarsModifiedP("c") == unordered_set<VAR_IDX>({ }));


    REQUIRE(pkb->getModifiesS("x") == unordered_set<STMT_NO>({ 2, 4, 5, 7,
        9 }));
    REQUIRE(pkb->getModifiesS("y") == unordered_set<STMT_NO>({ 1 }));
    REQUIRE(pkb->getModifiesS("cenX") == unordered_set<STMT_NO>({ 2, 3, 4,
        6 }));
    // invalid query
    REQUIRE(pkb->getModifiesS("X") == unordered_set<STMT_NO>({}));

    vector<pair<STMT_NO, vector<VAR_IDX>>> outputS = pkb->
        getAllModifiesSPairs();
    vector<pair<STMT_NO, vector<VAR_IDX>>> answerS({
        pair(1, vector({ 0 })),
        pair(2, vector({ 1, 2 })),
        pair(3, vector({ 1 })),
        pair(4, vector({ 2, 1 })),
        pair(5, vector({ 2 })),
        pair(6, vector({ 1 })),
        pair(7, vector({ 2 })),
        pair(9, vector({ 2 }))
    });
    REQUIRE(set<pair<STMT_NO, vector<int>>>(outputS.begin(), outputS.end())
        == set<pair<STMT_NO, vector<int>>>(answerS.begin(),
        answerS.end()));
  }

  SECTION("UsesS/P extraction") {
    REQUIRE(pkb->isUsesS(2, "x"));
    REQUIRE(pkb->isUsesS(2, "y"));
    REQUIRE(pkb->isUsesS(2, "cenX"));
    REQUIRE(pkb->isUsesS(3, "cenX"));
    REQUIRE(pkb->isUsesS(3, "x"));
    REQUIRE(pkb->isUsesS(3, "y"));
    REQUIRE(pkb->isUsesS(4, "cenX"));
    REQUIRE(pkb->isUsesS(4, "y"));
    REQUIRE(pkb->isUsesS(4, "x"));
    REQUIRE(pkb->isUsesS(5, "x"));
    REQUIRE(pkb->isUsesS(5, "y"));
    REQUIRE(pkb->isUsesS(6, "cenX"));
    REQUIRE(pkb->isUsesS(8, "y"));
    REQUIRE(pkb->isUsesS(9, "x"));
    REQUIRE(pkb->isUsesS(10, "y"));

    // invalid query
    REQUIRE(!pkb->isUsesS(1, "x"));
    REQUIRE(!pkb->isUsesS(7, "x"));

    REQUIRE(pkb->isUsesP("a", "x"));
    REQUIRE(pkb->isUsesP("a", "y"));
    REQUIRE(pkb->isUsesP("a", "cenX"));
    REQUIRE(pkb->isUsesP("b", "x"));
    REQUIRE(pkb->isUsesP("b", "y"));
    REQUIRE(pkb->isUsesP("c", "y"));

    // invalid query
    REQUIRE(!pkb->isUsesP("a", "cenY"));
    REQUIRE(!pkb->isUsesP("b", "cenX"));
    REQUIRE(!pkb->isUsesP("c", "x"));

    REQUIRE(pkb->getUsesS("x") == unordered_set<STMT_NO>({2, 3, 4, 5, 9}));
    REQUIRE(pkb->getUsesS("y") == unordered_set<STMT_NO>({2, 3, 4, 5, 8, 10}));
    REQUIRE(pkb->getUsesS("cenX") == unordered_set<STMT_NO>({2, 3, 4, 6}));
    REQUIRE(pkb->getUsesS("X") == unordered_set<STMT_NO>({}));

    REQUIRE(pkb->getVarsUsedS(2) == unordered_set<VAR_IDX>({ 0, 1, 2 }));
    REQUIRE(pkb->getVarsUsedS(3) == unordered_set<VAR_IDX>({ 0, 1, 2 }));
    REQUIRE(pkb->getVarsUsedP("a") == unordered_set<VAR_IDX>({0, 1, 2}));
    REQUIRE(pkb->getVarsUsedP("b") == unordered_set<VAR_IDX>({ 0, 2 }));
    REQUIRE(pkb->getVarsUsedP("c") == unordered_set<VAR_IDX>({ 0 }));
  }

  SECTION("Parent/* extraction") {
    REQUIRE(pkb->isParent(2, 3));
    REQUIRE(pkb->isParent(2, 4));
    REQUIRE(pkb->isParent(4, 5));
    REQUIRE(pkb->isParent(4, 6));
    // invalid query
    REQUIRE(!pkb->isParent(2, 6));
    REQUIRE(!pkb->isParent(4, 7));

    REQUIRE(pkb->isParentT(2, 3));
    REQUIRE(pkb->isParentT(2, 4));
    REQUIRE(pkb->isParentT(2, 5));
    REQUIRE(pkb->isParentT(2, 6));
    // invalid query
    REQUIRE(!pkb->isParentT(2, 7));

    vector<pair<STMT_NO, LIST_STMT_NO>> output = pkb->
      getAllParentsStmtPairs();
    vector<pair<STMT_NO, LIST_STMT_NO>> answer({
        pair(2, LIST_STMT_NO({ 3, 4 })),
        pair(4, LIST_STMT_NO({ 5, 6 })),
      });
    REQUIRE(set< pair<STMT_NO, LIST_STMT_NO>>(output.begin(), output.end()) ==
      set<pair<STMT_NO, LIST_STMT_NO>>(answer.begin(), answer.end()));
  }

  SECTION("Next/* extraction") {
    REQUIRE(pkb->getNextStmts(1) == unordered_set<int>{2});
    REQUIRE(pkb->getNextStmts(2) == unordered_set<int>{3});
    REQUIRE(pkb->getNextStmts(3) == unordered_set<int>{4});
    REQUIRE(pkb->getNextStmts(4) == unordered_set<int>{5, 6});
    REQUIRE(pkb->getNextStmts(5) == unordered_set<int>{2});
    REQUIRE(pkb->getNextStmts(6) == unordered_set<int>{2});

    vector<pair<STMT_NO, LIST_STMT_NO>> output = pkb->getAllNextStmtPairs();
    vector<pair<STMT_NO, LIST_STMT_NO>> answer({
        pair(1, LIST_STMT_NO({ 2 })),
        pair(2, LIST_STMT_NO({ 3 })),
        pair(3, LIST_STMT_NO({ 4 })),
        pair(4, LIST_STMT_NO({ 5, 6 })),
        pair(5, LIST_STMT_NO({ 2 })),
        pair(6, LIST_STMT_NO({ 2 })),
        pair(7, LIST_STMT_NO({ 8 })),
        pair(8, LIST_STMT_NO({ 9 })),
      });
    REQUIRE(set< pair<STMT_NO, LIST_STMT_NO>>(output.begin(), output.end()) ==
      set<pair<STMT_NO, LIST_STMT_NO>>(answer.begin(), answer.end()));
  }
}
