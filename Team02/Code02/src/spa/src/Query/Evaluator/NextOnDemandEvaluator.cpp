#include "NextOnDemandEvaluator.h"

#include <Common/Global.h>

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace query;

NextOnDemandEvaluator::NextOnDemandEvaluator(PKB* pkb) {
  this->pkb = pkb;
  stmtToStmtsCache.insert(
      {{RelationshipType::NEXT_T, {}}, {RelationshipType::NEXT_BIP_T, {}}});
  invStmtToStmtsCache.insert(
      {{RelationshipType::NEXT_T, {}}, {RelationshipType::NEXT_BIP_T, {}}});
}

bool NextOnDemandEvaluator::evaluateBoolNextTNextBipT(RelationshipType rsType,
                                                      const Param& left,
                                                      const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // Integer + Integer - e.g. NextT(1, 2)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    int leftStmtNum = stoi(left.value);
    int rightStmtNum = stoi(right.value);
    if (isStmtInStmtsCache(rsType, leftStmtNum) ||
        isStmtInInvStmtsCache(rsType, rightStmtNum)) {
      // if result cached
      return isRelationship(rsType, leftStmtNum, rightStmtNum);
    } else {
      return getIsNextTNextBipT(rsType, leftStmtNum, rightStmtNum);
    }
  }

  RelationshipType nonTransitiveRsType = getNonTransitiveRsType(rsType);
  // Wildcard + Wildcard - e.g. NextT(_, _)
  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !pkb->getMappings(nonTransitiveRsType, ParamPosition::BOTH).empty();
  }

  // Wildcard + Integer - e.g. NextT(_, 2)
  if (leftType == ParamType::WILDCARD) {
    return !pkb->getLeft(nonTransitiveRsType, stoi(right.value)).empty();
  }

  // Integer + Wildcard - e.g. NextT(1, _)
  return !pkb->getRight(nonTransitiveRsType, stoi(left.value)).empty();
}

unordered_set<int> NextOnDemandEvaluator::evaluateNextTNextBipT(
    RelationshipType rsType, const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // Integer + Synonym - e.g. NextT(1, s)
  if (leftType == ParamType::INTEGER_LITERAL) {
    int leftStmtNum = stoi(left.value);
    unordered_set<int> results;
    if (isStmtInStmtsCache(rsType, leftStmtNum)) {
      // if results cached
      results = getStmts(rsType, leftStmtNum);
    } else {
      results = getNextTNextBipTStmts(rsType, leftStmtNum);
      addToStmtsCache(rsType, leftStmtNum, results);
    }
    return results;
  }

  // Synonym + Integer - e.g. NextT(s, 2)
  int rightStmtNum = stoi(right.value);
  unordered_set<int> results;
  if (isStmtInInvStmtsCache(rsType, rightStmtNum)) {
    // if results cached
    results = getInvStmts(rsType, rightStmtNum);
  } else {
    results = getInvNextTNextBipTStmts(rsType, rightStmtNum);
    addToInvStmtsCache(rsType, rightStmtNum, results);
  }
  return results;
}

ClauseIncomingResults NextOnDemandEvaluator::evaluatePairNextTNextBipT(
    RelationshipType rsType, const Param& left, const Param& right) {
  ClauseIncomingResults results = {};
  unordered_set<int> allStmts = pkb->getAllStmts(DesignEntity::STATEMENT);

  for (auto stmtNum : allStmts) {
    unordered_set<int> nextTNextBipTStmts;
    if (isStmtInStmtsCache(rsType, stmtNum)) {
      // if results cached
      nextTNextBipTStmts = getStmts(rsType, stmtNum);
    } else {
      nextTNextBipTStmts = getNextTNextBipTStmts(rsType, stmtNum);
      addToStmtsCache(rsType, stmtNum, nextTNextBipTStmts);
    }

    for (auto nextTStmt : nextTNextBipTStmts) {
      if (left.type == ParamType::WILDCARD) {
        results.insert({nextTStmt});
      } else if (right.type == ParamType::WILDCARD) {
        results.insert({stmtNum});
      } else {
        results.insert({stmtNum, nextTStmt});
      }
    }
  }
  return results;
}

bool NextOnDemandEvaluator::getIsNextTNextBipT(RelationshipType rsType,
                                               int startStmt, int endStmt) {
  queue<int> stmtQueue = {};
  unordered_map<int, unordered_set<int>> visited = {};
  RelationshipType nonTransitiveRsType = getNonTransitiveRsType(rsType);

  // initialization
  unordered_set<int> nextNextBipStmtsFromStart =
      pkb->getRight(nonTransitiveRsType, startStmt);
  for (auto nextStmt : nextNextBipStmtsFromStart) {
    stmtQueue.push(nextStmt);
    visited.insert({nextStmt, {startStmt}});
  }

  // dequeue and process reachable (Next) stmts
  while (!stmtQueue.empty()) {
    int currStmt = stmtQueue.front();
    stmtQueue.pop();
    if (currStmt == endStmt) {
      return true;
    }
    unordered_set<int> allNextStmts =
        pkb->getRight(nonTransitiveRsType, currStmt);

    for (int nextStmt : allNextStmts) {
      bool hasBeenVisited = visited.find(nextStmt) != visited.end();
      bool hasBeenVisitedByCurr =
          hasBeenVisited &&
          (visited.at(nextStmt).find(currStmt) != visited.at(nextStmt).end());

      if (!hasBeenVisitedByCurr) {
        stmtQueue.push(nextStmt);

        if (!hasBeenVisited) {
          visited.insert({nextStmt, {currStmt}});
        } else {
          visited.at(nextStmt).insert(currStmt);
        }
      }
    }
  }

  return false;
}

unordered_set<int> NextOnDemandEvaluator::getNextTNextBipTStmts(
    RelationshipType rsType, int startStmt) {
  unordered_set<int> results = {};
  queue<int> stmtQueue = {};
  unordered_map<int, unordered_set<int>> visited = {};
  RelationshipType nonTransitiveRsType = getNonTransitiveRsType(rsType);

  // initialization
  unordered_set<int> nextNextBipStmtsFromStart =
      pkb->getRight(nonTransitiveRsType, startStmt);
  for (auto nextStmt : nextNextBipStmtsFromStart) {
    stmtQueue.push(nextStmt);
    visited.insert({nextStmt, {startStmt}});
    results.insert(nextStmt);
  }

  // dequeue and process reachable/ Next stmts
  while (!stmtQueue.empty()) {
    int currStmt = stmtQueue.front();
    stmtQueue.pop();
    unordered_set<int> allNextStmts =
        pkb->getRight(nonTransitiveRsType, currStmt);

    for (int nextStmt : allNextStmts) {
      results.insert(nextStmt);

      bool hasBeenVisited = visited.find(nextStmt) != visited.end();
      bool hasBeenVisitedByCurr =
          hasBeenVisited &&
          (visited.at(nextStmt).find(currStmt) != visited.at(nextStmt).end());

      if (!hasBeenVisitedByCurr) {
        stmtQueue.push(nextStmt);

        if (!hasBeenVisited) {
          visited.insert({nextStmt, {currStmt}});
        } else {
          visited.at(nextStmt).insert(currStmt);
        }
      }
    }
  }

  return results;
}

unordered_set<int> NextOnDemandEvaluator::getInvNextTNextBipTStmts(
    RelationshipType rsType, int endStmt) {
  unordered_set<int> results = {};
  queue<int> stmtQueue = {};
  unordered_map<int, unordered_set<int>> visited = {};
  RelationshipType nonTransitiveRsType = getNonTransitiveRsType(rsType);

  // initialization
  unordered_set<int> allPrevStmtsToEnd =
      pkb->getLeft(nonTransitiveRsType, endStmt);
  for (auto prevStmt : allPrevStmtsToEnd) {
    stmtQueue.push(prevStmt);
    visited.insert({prevStmt, {endStmt}});
    results.insert(prevStmt);
  }

  // dequeue and process backwards reachable/ previous stmts
  while (!stmtQueue.empty()) {
    int currStmt = stmtQueue.front();
    stmtQueue.pop();
    unordered_set<int> allPrevStmts =
        pkb->getLeft(nonTransitiveRsType, currStmt);

    for (int prevStmt : allPrevStmts) {
      results.insert(prevStmt);

      bool hasBeenVisited = visited.find(prevStmt) != visited.end();
      bool hasBeenVisitedByCurr =
          hasBeenVisited &&
          (visited.at(prevStmt).find(currStmt) != visited.at(prevStmt).end());

      if (!hasBeenVisitedByCurr) {
        stmtQueue.push(prevStmt);

        if (!hasBeenVisited) {
          visited.insert({prevStmt, {currStmt}});
        } else {
          visited.at(prevStmt).insert(currStmt);
        }
      }
    }
  }

  return results;
}

RelationshipType NextOnDemandEvaluator::getNonTransitiveRsType(
    RelationshipType rsType) {
  if (rsType == RelationshipType::NEXT_T) {
    return RelationshipType::NEXT;
  } else if (rsType == RelationshipType::NEXT_BIP_T) {
    return RelationshipType::NEXT_BIP;
  } else {
    DMOprintErrMsgAndExit("[NextOnDemandEvaluator] Invalid transitive rs type");
    return {};
  }
}

/* Cache getter methods -------------------------------------- */
bool NextOnDemandEvaluator::isStmtInStmtsCache(RelationshipType rsType,
                                               StmtNo leftStmt) {
  return stmtToStmtsCache[rsType].find(leftStmt) !=
         stmtToStmtsCache[rsType].end();
}

bool NextOnDemandEvaluator::isStmtInInvStmtsCache(RelationshipType rsType,
                                                  StmtNo rightStmt) {
  return invStmtToStmtsCache[rsType].find(rightStmt) !=
         invStmtToStmtsCache[rsType].end();
}

bool NextOnDemandEvaluator::isRelationship(RelationshipType rsType, StmtNo left,
                                           StmtNo right) {
  return stmtToStmtsCache[rsType][left].find(right) !=
             stmtToStmtsCache[rsType][left].end() ||
         invStmtToStmtsCache[rsType][right].find(left) !=
             stmtToStmtsCache[rsType][right].end();
}

SetOfInts& NextOnDemandEvaluator::getStmts(RelationshipType rsType,
                                           StmtNo stmt) {
  return stmtToStmtsCache[rsType][stmt];
}

SetOfInts& NextOnDemandEvaluator::getInvStmts(RelationshipType rsType,
                                              StmtNo stmt) {
  return invStmtToStmtsCache[rsType][stmt];
}

/* Cache setter methods --------------------------------------- */
void NextOnDemandEvaluator::addToStmtsCache(RelationshipType rsType,
                                            StmtNo leftStmt,
                                            SetOfInts rightStmts) {
  stmtToStmtsCache[rsType].insert({leftStmt, rightStmts});
}

void NextOnDemandEvaluator::addToInvStmtsCache(RelationshipType rsType,
                                               StmtNo rightStmt,
                                               SetOfInts leftStmts) {
  invStmtToStmtsCache[rsType].insert({rightStmt, leftStmts});
}
