#pragma once

#include <Common/Common.h>
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

  bool isAffects(RelationshipType rsType, STMT_NO a1, STMT_NO a2);
  std::unordered_set<STMT_NO> getAffects(RelationshipType rsType, STMT_NO a1);
  std::unordered_set<STMT_NO> getAffectsInv(RelationshipType rsType,
                                            STMT_NO a2);

  bool evaluateBoolAffects(RelationshipType rsType, const query::Param& left,
                           const query::Param& right);
  std::unordered_set<STMT_NO> evaluateStmtAffects(RelationshipType rsType,
                                                  const query::Param& left,
                                                  const query::Param& right);
  std::vector<std::vector<STMT_NO>> evaluateSynonymWildcard(
      RelationshipType rsType, const query::Param& left,
      const query::Param& right);
  std::vector<std::vector<STMT_NO>> evaluatePairAffects(
      RelationshipType rsType, const query::Param& left,
      const query::Param& right);

  bool evaluateBoolAffectsT(const query::Param& left,
                            const query::Param& right);
  std::unordered_set<STMT_NO> evaluateStmtAffectsT(const query::Param& left,
                                                   const query::Param& right);
  std::unordered_set<STMT_NO> evaluateStmtAffectsTHelper(
      const query::Param& left, const query::Param& right,
      std::unordered_set<STMT_NO>* visited);
  std::vector<std::vector<STMT_NO>> evaluatePairAffectsT(
      const query::Param& left, const query::Param& right);

 private:
  PKB* pkb;

  /* Affects Results Cache ------------------------------------------ */
  bool isCompleteAffectsCache = false;  // true when Affects(s1, _) or (_, s2)
                                 // or (s1, s2) have been computed before
  bool isCompleteAffectsTCache = false;
  std::unordered_set<STMT_NO> allVisitedStmts = {};
  std::unordered_map<RelationshipType, std::unordered_set<STMT_NO>>
      affectsStmts = {{RelationshipType::AFFECTS, {}}};
  std::unordered_map<RelationshipType, std::unordered_set<STMT_NO>>
      affectsInvStmts = {{RelationshipType::AFFECTS, {}}};
  TablesRs tableOfAffects = {{RelationshipType::AFFECTS, {}},
                             {RelationshipType::AFFECTS_T, {}},
                             {RelationshipType::AFFECTS_BIP, {}}};
  TablesRs tableOfAffectsInv = {{RelationshipType::AFFECTS, {}},
                                {RelationshipType::AFFECTS_T, {}},
                                {RelationshipType::AFFECTS_BIP, {}}};
  // TODO(CS): change to unordered_set of vector later
  std::unordered_map<RelationshipType, std::vector<std::vector<STMT_NO>>>
      affectsLeftStmtPairs = {{RelationshipType::AFFECTS, {}},
                              {RelationshipType::AFFECTS_T, {}},
                              {RelationshipType::AFFECTS_BIP, {}}};
  std::unordered_map<RelationshipType, std::vector<std::vector<STMT_NO>>>
      affectsRightStmtPairs = {{RelationshipType::AFFECTS, {}},
                               {RelationshipType::AFFECTS_T, {}},
                               {RelationshipType::AFFECTS_BIP, {}}};
  std::unordered_map<RelationshipType, std::vector<std::vector<STMT_NO>>>
      affectsStmtPairs = {{RelationshipType::AFFECTS, {}},
                          {RelationshipType::AFFECTS_T, {}},
                          {RelationshipType::AFFECTS_BIP, {}}};
  /* Extraction Methods ----------------------------------------------------- */
  void extractAffects(RelationshipType rsType, STMT_NO startStmt,
                      STMT_NO endStmt, STMT_NO stmtAfterIfOrWhile,
                      LastModifiedTable* LMT, BoolParamCombo paramCombo);

  void processAssignStmt(RelationshipType rsType, STMT_NO currStmt,
                         LastModifiedTable* LMT);
  void processWhileLoop(RelationshipType rsType, STMT_NO firstStmtInWhile,
                        STMT_NO endStmt, STMT_NO whileStmt,
                        LastModifiedTable* LMT, BoolParamCombo paramCombo);
  void processThenElseBlocks(RelationshipType rsType,
                             std::unordered_set<STMT_NO> thenElseStmts,
                             STMT_NO endStmt, STMT_NO nextStmtForIf,
                             LastModifiedTable* LMT, BoolParamCombo paramCombo);

  void updateLastModifiedVariables(STMT_NO currStmt, LastModifiedTable* LMT);
  void addAffectsRelationship(RelationshipType rsType, LastModifiedTable* LMT,
                              STMT_NO LMTStmt, STMT_NO currStmt);
  bool shouldTerminateBoolEarly(RelationshipType rsType,
                                BoolParamCombo paramCombo, STMT_NO startStmt,
                                STMT_NO endStmt);
  LastModifiedTable mergeLMT(LastModifiedTable* firstLMT,
                             LastModifiedTable* secondLMT);
  RelationshipType getCFGRsType(RelationshipType rsType);
  bool evalBoolLitAffectsT(const query::Param& left, const query::Param& right,
                           std::unordered_set<STMT_NO>* visited);
  void populateAffectsTCache(const query::Param& left,
                             const query::Param& right);
  void populateAffectsTTable(
      std::unordered_map<STMT_NO, std::unordered_set<STMT_NO>>* target,
      std::unordered_map<STMT_NO, std::unordered_set<STMT_NO>>* base);
  void populateAffectsTTableHelper(
      STMT_NO orig, STMT_NO affected, std::unordered_set<STMT_NO>* visited,
      std::unordered_map<STMT_NO, std::unordered_set<STMT_NO>>* base);
};
