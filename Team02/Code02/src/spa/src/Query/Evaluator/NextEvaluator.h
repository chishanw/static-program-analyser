#pragma once

#include <Common/Common.h>
#include <PKB/PKB.h>
#include <Query/Common.h>

#include <unordered_set>
#include <vector>

class NextEvaluator {
 public:
  explicit NextEvaluator(PKB* pkb);

  bool evaluateBoolNextNextBip(RelationshipType rsType,
                               const query::Param& left,
                               const query::Param& right);
  std::unordered_set<int> evaluateNextNextBip(RelationshipType rsType,
                                              const query::Param& left,
                                              const query::Param& right);
  std::vector<std::vector<int>> evaluatePairNextNextBip(
      RelationshipType rsType, const query::Param& left,
      const query::Param& right);

  bool evaluateBoolNextTNextBipT(RelationshipType rsType,
                                 const query::Param& left,
                                 const query::Param& right);
  std::unordered_set<int> evaluateNextTNextBipT(RelationshipType rsType,
                                                const query::Param& left,
                                                const query::Param& right);
  std::vector<std::vector<int>> evaluatePairNextTNextBipT(
      RelationshipType rsType, const query::Param& left,
      const query::Param& right);

 private:
  PKB* pkb;

  TablesRs stmtToStmtsCache;
  TablesRs invStmtToStmtsCache;

  bool isStmtInStmtsCache(RelationshipType rsType, StmtNo leftStmt);
  bool isStmtInInvStmtsCache(RelationshipType rsType, StmtNo leftStmt);
  bool isRelationship(RelationshipType rsType, StmtNo left, StmtNo right);
  SetOfInts& getStmts(RelationshipType rsType, StmtNo stmt);
  SetOfInts& getInvStmts(RelationshipType rsType, StmtNo stmt);

  void addToStmtsCache(RelationshipType rsType, StmtNo leftStmt,
                       SetOfInts rightStmts);
  void addToInvStmtsCache(RelationshipType rsType, StmtNo leftStmt,
                          SetOfInts rightStmts);

  RelationshipType getNonTransitiveRsType(RelationshipType rsType);
  bool getIsNextTNextBipT(RelationshipType rsType, int startStmt, int endStmt);
  std::unordered_set<int> getNextTNextBipTStmts(RelationshipType rsType,
                                                int startStmt);
  std::unordered_set<int> getInvNextTNextBipTStmts(RelationshipType rsType,
                                                   int endStmt);
};
