#pragma once

#include <PKB/PKB.h>
#include <Query/Common.h>

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef std::unordered_map<VarIdx, std::unordered_set<STMT_NO>>
    LastModifiedTable;
enum class BoolParamCombo {
  LITERALS,
  WILDCARDS,
  LITERAL_WILDCARD,
  WILDCARD_LITERAL
};

class AffectsEvaluator {
 public:
  explicit AffectsEvaluator(PKB*);

  bool isAffects(STMT_NO a1, STMT_NO a2);
  std::unordered_set<STMT_NO> getAffects(STMT_NO a1);
  std::unordered_set<STMT_NO> getAffectsInv(STMT_NO a2);

  bool evaluateBoolAffects(const query::Param& left, const query::Param& right);
  std::unordered_set<STMT_NO> evaluateStmtAffects(const query::Param& left,
                                                  const query::Param& right);
  std::unordered_set<STMT_NO> evaluateSynonymLiteral(const query::Param& left,
                                                     const query::Param& right);
  std::unordered_set<STMT_NO> evaluateSynonymWildcard(
      const query::Param& left, const query::Param& right);
  std::vector<std::vector<STMT_NO>> evaluatePairAffects();

 private:
  PKB* pkb;

  /* Affects Results Cache ------------------------------------------ */
  bool isCompleteCache = false;  // true when Affects(s1, _) or (_, s2)
                                 // or (s1, s2) have been computed before
  std::unordered_set<STMT_NO> allVisitedStmts = {};
  std::unordered_set<STMT_NO> affectsStmts = {};
  std::unordered_set<STMT_NO> affectsInvStmts = {};
  std::unordered_map<STMT_NO, std::unordered_set<STMT_NO>> tableOfAffects = {};
  std::unordered_map<STMT_NO, std::unordered_set<STMT_NO>> tableOfAffectsInv =
      {};
  // TODO(CS): change to unordered_set of vector later
  std::vector<std::vector<STMT_NO>> affectsStmtPairs = {};

  /* Extraction Methods ----------------------------------------------------- */
  void extractAffects(STMT_NO startStmt, STMT_NO endStmt,
                      STMT_NO stmtAfterIfOrWhile, LastModifiedTable* LMT,
                      BoolParamCombo paramCombo);

  void processAssignStmt(STMT_NO currStmt, LastModifiedTable* LMT);
  void processWhileLoop(STMT_NO firstStmtInWhile, STMT_NO endStmt,
                        STMT_NO whileStmt, LastModifiedTable* LMT,
                        BoolParamCombo paramCombo);
  void processThenElseBlocks(std::unordered_set<STMT_NO> thenElseStmts,
                             STMT_NO endStmt, STMT_NO nextStmtForIf,
                             LastModifiedTable* LMT, BoolParamCombo paramCombo);

  void updateLastModifiedVariables(STMT_NO currStmt, LastModifiedTable* LMT);
  void addAffectsRelationship(LastModifiedTable* LMT, STMT_NO LMTStmt,
                              STMT_NO currStmt);
  bool shouldTerminateBoolEarly(BoolParamCombo paramCombo, STMT_NO startStmt,
                                STMT_NO endStmt);
  LastModifiedTable mergeLMT(LastModifiedTable* firstLMT,
                             LastModifiedTable* secondLMT);
};
