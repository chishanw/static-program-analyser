#pragma once

#include <Common/Common.h>
#include <PKB/PKB.h>
#include <Query/Common.h>

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef std::unordered_map<VarIdx, std::unordered_set<StmtNo>>
    LastModifiedTable;
enum class BoolParamCombo {
  LITERALS,
  WILDCARDS,
  LITERAL_WILDCARD,
  WILDCARD_LITERAL
};

class AffectsOnDemandEvaluator {
 public:
  explicit AffectsOnDemandEvaluator(PKB*);

  bool isAffects(RelationshipType rsType, StmtNo a1, StmtNo a2);
  std::unordered_set<StmtNo> getAffects(RelationshipType rsType, StmtNo a1);
  std::unordered_set<StmtNo> getAffectsInv(RelationshipType rsType,
                                            StmtNo a2);

  bool evaluateBoolAffects(RelationshipType rsType, const query::Param& left,
                           const query::Param& right);
  std::unordered_set<StmtNo> evaluateStmtAffects(RelationshipType rsType,
                                                  const query::Param& left,
                                                  const query::Param& right);
  query::ClauseIncomingResults evaluateSynonymWildcard(
      RelationshipType rsType, const query::Param& left,
      const query::Param& right);
  query::ClauseIncomingResults evaluatePairAffects(RelationshipType rsType,
                                                   const query::Param& left,
                                                   const query::Param& right);

  bool evaluateBoolAffectsT(const query::Param& left,
                            const query::Param& right);
  std::unordered_set<StmtNo> evaluateStmtAffectsT(const query::Param& left,
                                                   const query::Param& right);
  std::unordered_set<StmtNo> evaluateStmtAffectsTHelper(
      const query::Param& left, const query::Param& right,
      std::unordered_set<StmtNo>* visited);
  query::ClauseIncomingResults evaluatePairAffectsT(const query::Param& left,
                                                    const query::Param& right);

 private:
  PKB* pkb;

  /* Affects Results Cache ------------------------------------------ */
  bool isCompleteAffectsCache = false;  // true when Affects(s1, _) or (_, s2)
                                        // or (s1, s2) have been computed before
  bool isCompleteAffectsTCache = false;
  std::unordered_set<StmtNo> allVisitedStmts = {};
  std::unordered_map<RelationshipType, std::unordered_set<StmtNo>>
      affectsStmts = {{RelationshipType::AFFECTS, {}}};
  std::unordered_map<RelationshipType, std::unordered_set<StmtNo>>
      affectsInvStmts = {{RelationshipType::AFFECTS, {}}};
  TablesRs tableOfAffects = {{RelationshipType::AFFECTS, {}},
                             {RelationshipType::AFFECTS_T, {}},
                             {RelationshipType::AFFECTS_BIP, {}}};
  TablesRs tableOfAffectsInv = {{RelationshipType::AFFECTS, {}},
                                {RelationshipType::AFFECTS_T, {}},
                                {RelationshipType::AFFECTS_BIP, {}}};
  std::unordered_map<RelationshipType, query::ClauseIncomingResults>
      affectsLeftStmtPairs = {{RelationshipType::AFFECTS, {}},
                              {RelationshipType::AFFECTS_T, {}},
                              {RelationshipType::AFFECTS_BIP, {}}};
  std::unordered_map<RelationshipType, query::ClauseIncomingResults>
      affectsRightStmtPairs = {{RelationshipType::AFFECTS, {}},
                               {RelationshipType::AFFECTS_T, {}},
                               {RelationshipType::AFFECTS_BIP, {}}};
  std::unordered_map<RelationshipType, query::ClauseIncomingResults>
      affectsStmtPairs = {{RelationshipType::AFFECTS, {}},
                          {RelationshipType::AFFECTS_T, {}},
                          {RelationshipType::AFFECTS_BIP, {}}};
  /* Extraction Methods ----------------------------------------------------- */
  void extractAffects(RelationshipType rsType, StmtNo startStmt,
                      StmtNo endStmt, StmtNo stmtAfterIfOrWhile,
                      LastModifiedTable* LMT, BoolParamCombo paramCombo);

  void processAssignStmt(RelationshipType rsType, StmtNo currStmt,
                         LastModifiedTable* LMT);
  void processWhileLoop(RelationshipType rsType, StmtNo firstStmtInWhile,
                        StmtNo endStmt, StmtNo whileStmt,
                        LastModifiedTable* LMT, BoolParamCombo paramCombo);
  void processThenElseBlocks(RelationshipType rsType,
                             std::unordered_set<StmtNo> thenElseStmts,
                             StmtNo endStmt, StmtNo nextStmtForIf,
                             LastModifiedTable* LMT, BoolParamCombo paramCombo);

  void updateLastModifiedVariables(StmtNo currStmt, LastModifiedTable* LMT);
  void addAffectsRelationship(RelationshipType rsType, LastModifiedTable* LMT,
                              StmtNo LMTStmt, StmtNo currStmt);
  bool shouldTerminateBoolEarly(RelationshipType rsType,
                                BoolParamCombo paramCombo, StmtNo startStmt,
                                StmtNo endStmt);
  LastModifiedTable mergeLMT(LastModifiedTable* firstLMT,
                             LastModifiedTable* secondLMT);
  RelationshipType getCFGRsType(RelationshipType rsType);
  bool evalBoolLitAffectsT(const query::Param& left, const query::Param& right,
                           std::unordered_set<StmtNo>* visited);
  void populateAffectsTCache(const query::Param& left,
                             const query::Param& right);
  void populateAffectsTTable(
      std::unordered_map<StmtNo, std::unordered_set<StmtNo>>* target,
      std::unordered_map<StmtNo, std::unordered_set<StmtNo>>* base);
  void populateAffectsTTableHelper(
      StmtNo orig, StmtNo affected, std::unordered_set<StmtNo>* visited,
      std::unordered_map<StmtNo, std::unordered_set<StmtNo>>* base);
};
