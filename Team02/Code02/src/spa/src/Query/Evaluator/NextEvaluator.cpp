#include "NextEvaluator.h"

#include <Common/Global.h>

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace query;

NextEvaluator::NextEvaluator(PKB* pkb) {
  this->pkb = pkb;
  this->cache = new QueryEvaluatorCache();  // init here for unit tests
}

void NextEvaluator::attachCache(QueryEvaluatorCache* cache) {
  this->cache = cache;
}

bool NextEvaluator::evaluateBoolNextNextBip(RelationshipType rsType,
                                            const Param& left,
                                            const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // if both literal - Next(1,2)
  // if one literal + underscore - Next(_, 2), Next(2, _)
  // if both underscore - Next(_, _)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    unordered_set<STMT_NO> nextStmts = pkb->getRight(rsType, stoi(left.value));
    return nextStmts.find(stoi(right.value)) != nextStmts.end();
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !pkb->getMappings(rsType, ParamPosition::BOTH).empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !(pkb->getLeft(rsType, stoi(right.value)).empty());
  }

  return !(pkb->getRight(rsType, stoi(left.value)).empty());
}

// synonym & literal
unordered_set<int> NextEvaluator::evaluateNextNextBip(RelationshipType rsType,
                                                      const Param& left,
                                                      const Param& right) {
  // if one literal + synonym - Next(s, 2), Next(1, s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getRight(rsType, stoi(left.value));
  }

  // rightType == ParamType::LITERAL
  return pkb->getLeft(rsType, stoi(right.value));
}

// synonym & wildcard - Next(s, _) -> getAllNextStmtPairs()
// synonym & synonym - Next(s1, s2) -> getAllNextStmtPairs()
vector<vector<int>> NextEvaluator::evaluatePairNextNextBip(
    RelationshipType rsType, const Param& left, const Param& right) {
  vector<vector<int>> formattedResults = {};

  if (left.type == ParamType::WILDCARD) {
    SetOfStmtLists results = pkb->getMappings(rsType, ParamPosition::RIGHT);
    formattedResults.insert(formattedResults.end(), results.begin(),
                            results.end());
    return formattedResults;
  }
  if (right.type == ParamType::WILDCARD) {
    SetOfStmtLists results = pkb->getMappings(rsType, ParamPosition::LEFT);
    formattedResults.insert(formattedResults.end(), results.begin(),
                            results.end());
    return formattedResults;
  }
  // both synonyms
  SetOfStmtLists results = pkb->getMappings(rsType, ParamPosition::BOTH);
  formattedResults.insert(formattedResults.end(), results.begin(),
                          results.end());
  return formattedResults;
}

bool NextEvaluator::evaluateBoolNextTNextBipT(RelationshipType rsType,
                                              const Param& left,
                                              const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // Integer + Integer - e.g. NextT(1, 2)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    int leftStmtNum = stoi(left.value);
    int rightStmtNum = stoi(right.value);
    if (cache->isStmtInStmtsCache(rsType, leftStmtNum) ||
        cache->isStmtInInvStmtsCache(rsType, rightStmtNum)) {
      // if result cached
      return cache->isRelationship(rsType, leftStmtNum, rightStmtNum);
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

unordered_set<int> NextEvaluator::evaluateNextTNextBipT(RelationshipType rsType,
                                                        const Param& left,
                                                        const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // Integer + Synonym - e.g. NextT(1, s)
  if (leftType == ParamType::INTEGER_LITERAL) {
    int leftStmtNum = stoi(left.value);
    unordered_set<int> results;
    if (cache->isStmtInStmtsCache(rsType, leftStmtNum)) {
      // if results cached
      results = cache->getStmts(rsType, leftStmtNum);
    } else {
      results = getNextTNextBipTStmts(rsType, leftStmtNum);
      cache->addToStmtsCache(rsType, leftStmtNum, results);
    }
    return results;
  }

  // Synonym + Integer - e.g. NextT(s, 2)
  int rightStmtNum = stoi(right.value);
  unordered_set<int> results;
  if (cache->isStmtInInvStmtsCache(rsType, rightStmtNum)) {
    // if results cached
    results = cache->getInvStmts(rsType, rightStmtNum);
  } else {
    results = getInvNextTNextBipTStmts(rsType, rightStmtNum);
    cache->addToInvStmtsCache(rsType, rightStmtNum, results);
  }
  return results;
}

vector<vector<int>> NextEvaluator::evaluatePairNextTNextBipT(
    RelationshipType rsType, const Param& left, const Param& right) {
  vector<vector<int>> results = {};
  unordered_set<int> allStmts = pkb->getAllStmts(DesignEntity::STATEMENT);

  for (auto stmtNum : allStmts) {
    unordered_set<int> nextTNextBipTStmts;
    if (cache->isStmtInStmtsCache(rsType, stmtNum)) {
      // if results cached
      nextTNextBipTStmts = cache->getStmts(rsType, stmtNum);
    } else {
      nextTNextBipTStmts = getNextTNextBipTStmts(rsType, stmtNum);
      cache->addToStmtsCache(rsType, stmtNum, nextTNextBipTStmts);
    }

    for (auto nextTStmt : nextTNextBipTStmts) {
      if (left.type == ParamType::WILDCARD) {
        results.push_back({nextTStmt});
      } else if (right.type == ParamType::WILDCARD) {
        results.push_back({stmtNum});
      } else {
        results.push_back({stmtNum, nextTStmt});
      }
    }
  }
  return results;
}

bool NextEvaluator::getIsNextTNextBipT(RelationshipType rsType, int startStmt,
                                       int endStmt) {
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

unordered_set<int> NextEvaluator::getNextTNextBipTStmts(RelationshipType rsType,
                                                        int startStmt) {
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

unordered_set<int> NextEvaluator::getInvNextTNextBipTStmts(
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

RelationshipType NextEvaluator::getNonTransitiveRsType(
    RelationshipType rsType) {
  if (rsType == RelationshipType::NEXT_T) {
    return RelationshipType::NEXT;
  } else if (rsType == RelationshipType::NEXT_BIP_T) {
    return RelationshipType::NEXT_BIP;
  } else {
    DMOprintErrMsgAndExit("[NextEvaluator] Invalid transitive rs type");
    return {};
  }
}
