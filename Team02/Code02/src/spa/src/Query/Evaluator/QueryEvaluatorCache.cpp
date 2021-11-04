#include "QueryEvaluatorCache.h"

#include <Common/Global.h>

#include <unordered_map>

using namespace std;
using namespace query;

QueryEvaluatorCache::QueryEvaluatorCache() {
  stmtToStmtsCache.insert({{RelationshipType::NEXT_T, {}},
                           {RelationshipType::NEXT_BIP_T, {}},
                           {RelationshipType::AFFECTS, {}},
                           {RelationshipType::AFFECTS_T, {}},
                           {RelationshipType::AFFECTS_BIP, {}},
                           {RelationshipType::AFFECTS_BIP_T, {}}});
  invStmtToStmtsCache.insert({{RelationshipType::NEXT_T, {}},
                              {RelationshipType::NEXT_BIP_T, {}},
                              {RelationshipType::AFFECTS, {}},
                              {RelationshipType::AFFECTS_T, {}},
                              {RelationshipType::AFFECTS_BIP, {}},
                              {RelationshipType::AFFECTS_BIP_T, {}}});
}

bool QueryEvaluatorCache::isStmtInStmtsCache(RelationshipType rsType,
                                             StmtNo leftStmt) {
  return stmtToStmtsCache[rsType].find(leftStmt) !=
         stmtToStmtsCache[rsType].end();
}

bool QueryEvaluatorCache::isRelationship(RelationshipType rsType, StmtNo left,
                                         StmtNo right) {
  return stmtToStmtsCache[rsType][left].find(right) !=
             stmtToStmtsCache[rsType][left].end() ||
         invStmtToStmtsCache[rsType][right].find(left) !=
             stmtToStmtsCache[rsType][right].end();
}

void QueryEvaluatorCache::addToStmtsCache(RelationshipType rsType,
                                          StmtNo leftStmt,
                                          SetOfInts rightStmts) {
  stmtToStmtsCache[rsType].insert({leftStmt, rightStmts});
}

bool QueryEvaluatorCache::isStmtInInvStmtsCache(RelationshipType rsType,
                                                StmtNo rightStmt) {
  return invStmtToStmtsCache[rsType].find(rightStmt) !=
         invStmtToStmtsCache[rsType].end();
}

void QueryEvaluatorCache::addToInvStmtsCache(RelationshipType rsType,
                                             StmtNo rightStmt,
                                             SetOfInts leftStmts) {
  invStmtToStmtsCache[rsType].insert({rightStmt, leftStmts});
}

SetOfInts& QueryEvaluatorCache::getStmts(RelationshipType rsType, StmtNo stmt) {
  return stmtToStmtsCache[rsType][stmt];
}

SetOfInts& QueryEvaluatorCache::getInvStmts(RelationshipType rsType,
                                            StmtNo stmt) {
  return invStmtToStmtsCache[rsType][stmt];
}
