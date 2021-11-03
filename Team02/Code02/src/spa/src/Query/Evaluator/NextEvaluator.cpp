#include "NextEvaluator.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace query;

NextEvaluator::NextEvaluator(PKB* pkb) { this->pkb = pkb; }

bool NextEvaluator::evaluateBoolNext(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // if both literal - Next(1,2)
  // if one literal + underscore - Next(_, 2), Next(2, _)
  // if both underscore - Next(_, _)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    unordered_set<STMT_NO> nextStmts =
        pkb->getRight(RelationshipType::NEXT, stoi(left.value));
    return nextStmts.find(stoi(right.value)) != nextStmts.end();
  }

  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !pkb->getMappings(RelationshipType::NEXT, ParamPosition::BOTH)
                .empty();
  }

  if (leftType == ParamType::WILDCARD) {
    return !(pkb->getLeft(RelationshipType::NEXT, stoi(right.value)).empty());
  }

  return !(pkb->getRight(RelationshipType::NEXT, stoi(left.value)).empty());
}

// synonym & literal
unordered_set<int> NextEvaluator::evaluateNext(const Param& left,
                                               const Param& right) {
  // if one literal + synonym - Next(s, 2), Next(1, s)
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  if (leftType == ParamType::INTEGER_LITERAL) {
    return pkb->getRight(RelationshipType::NEXT, stoi(left.value));
  }

  // rightType == ParamType::LITERAL
  return pkb->getLeft(RelationshipType::NEXT, stoi(right.value));
}

// synonym & wildcard - Next(s, _) -> getAllNextStmtPairs()
// synonym & synonym - Next(s1, s2) -> getAllNextStmtPairs()
vector<vector<int>> NextEvaluator::evaluatePairNext(const Param& left,
                                                    const Param& right) {
  vector<vector<int>> formattedResults = {};

  if (left.type == ParamType::WILDCARD) {
    SetOfStmtLists results =
        pkb->getMappings(RelationshipType::NEXT, ParamPosition::RIGHT);
    formattedResults.insert(formattedResults.end(), results.begin(),
                            results.end());
    return formattedResults;
  }
  if (right.type == ParamType::WILDCARD) {
    SetOfStmtLists results =
        pkb->getMappings(RelationshipType::NEXT, ParamPosition::LEFT);
    formattedResults.insert(formattedResults.end(), results.begin(),
                            results.end());
    return formattedResults;
  }
  // both synonyms
  SetOfStmtLists results =
      pkb->getMappings(RelationshipType::NEXT, ParamPosition::BOTH);
  formattedResults.insert(formattedResults.end(), results.begin(),
                          results.end());
  return formattedResults;
}

bool NextEvaluator::evaluateBoolNextT(const Param& left, const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // Integer + Integer - e.g. NextT(1, 2)
  if (leftType == ParamType::INTEGER_LITERAL &&
      rightType == ParamType::INTEGER_LITERAL) {
    int leftStmtNum = stoi(left.value);
    int rightStmtNum = stoi(right.value);
    if (nextTCache.find(leftStmtNum) != nextTCache.end()) {
      // if result cached
      return nextTCache.at(leftStmtNum).find(rightStmtNum) !=
             nextTCache.at(leftStmtNum).end();
    } else if (reverseNextTCache.find(rightStmtNum) !=
               reverseNextTCache.end()) {
      return reverseNextTCache.at(rightStmtNum).find(leftStmtNum) !=
             reverseNextTCache.at(rightStmtNum).end();
    } else {
      return getIsNextT(leftStmtNum, rightStmtNum);
    }
  }

  // Wildcard + Wildcard - e.g. NextT(_, _)
  if (leftType == ParamType::WILDCARD && rightType == ParamType::WILDCARD) {
    return !pkb->getMappings(RelationshipType::NEXT, ParamPosition::BOTH)
                .empty();
  }

  // Wildcard + Integer - e.g. NextT(_, 2)
  if (leftType == ParamType::WILDCARD) {
    return !pkb->getLeft(RelationshipType::NEXT, stoi(right.value)).empty();
  }

  // Integer + Wildcard - e.g. NextT(1, _)
  return !pkb->getRight(RelationshipType::NEXT, stoi(left.value)).empty();
}

unordered_set<int> NextEvaluator::evaluateNextT(const Param& left,
                                                const Param& right) {
  ParamType leftType = left.type;
  ParamType rightType = right.type;

  // Integer + Synonym - e.g. NextT(1, s)
  if (leftType == ParamType::INTEGER_LITERAL) {
    int leftStmtNum = stoi(left.value);
    unordered_set<int> results;
    if (nextTCache.find(leftStmtNum) != nextTCache.end()) {
      // if results cached
      results = nextTCache.at(leftStmtNum);
    } else {
      results = getNextTStmts(leftStmtNum);
      nextTCache.insert({leftStmtNum, results});
    }
    return results;
  }

  // Synonym + Integer - e.g. NextT(s, 2)
  int rightStmtNum = stoi(right.value);
  unordered_set<int> results;
  if (reverseNextTCache.find(rightStmtNum) != reverseNextTCache.end()) {
    // if results cached
    results = reverseNextTCache.at(rightStmtNum);
  } else {
    results = getPrevTStmts(rightStmtNum);
    reverseNextTCache.insert({rightStmtNum, results});
  }
  return results;
}

vector<vector<int>> NextEvaluator::evaluatePairNextT(const Param& left,
                                                     const Param& right) {
  vector<vector<int>> allNextTResults = {};
  unordered_set<int> allStmts = pkb->getAllStmts(DesignEntity::STATEMENT);

  for (auto stmtNum : allStmts) {
    unordered_set<int> nextTStmts;
    if (nextTCache.find(stmtNum) != nextTCache.end()) {
      // if results cached
      nextTStmts = nextTCache.at(stmtNum);
    } else {
      nextTStmts = getNextTStmts(stmtNum);
      nextTCache.insert({stmtNum, nextTStmts});
    }

    for (auto nextTStmt : nextTStmts) {
      if (left.type == ParamType::WILDCARD) {
        allNextTResults.push_back({nextTStmt});
      } else if (right.type == ParamType::WILDCARD) {
        allNextTResults.push_back({stmtNum});
      } else {
        allNextTResults.push_back({stmtNum, nextTStmt});
      }
    }
  }
  return allNextTResults;
}

bool NextEvaluator::getIsNextT(int startStmt, int endStmt) {
  queue<int> stmtQueue = {};
  unordered_map<int, unordered_set<int>> visited = {};

  // initialization
  unordered_set<int> allNextStmtsFromStart =
      pkb->getRight(RelationshipType::NEXT, startStmt);
  for (auto nextStmt : allNextStmtsFromStart) {
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
        pkb->getRight(RelationshipType::NEXT, currStmt);

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

unordered_set<int> NextEvaluator::getNextTStmts(int startStmt) {
  unordered_set<int> nextTStmts = {};
  queue<int> stmtQueue = {};
  unordered_map<int, unordered_set<int>> visited = {};

  // initialization
  unordered_set<int> allNextStmtsFromStart =
      pkb->getRight(RelationshipType::NEXT, startStmt);
  for (auto nextStmt : allNextStmtsFromStart) {
    stmtQueue.push(nextStmt);
    visited.insert({nextStmt, {startStmt}});
    nextTStmts.insert(nextStmt);
  }

  // dequeue and process reachable/ Next stmts
  while (!stmtQueue.empty()) {
    int currStmt = stmtQueue.front();
    stmtQueue.pop();
    unordered_set<int> allNextStmts =
        pkb->getRight(RelationshipType::NEXT, currStmt);

    for (int nextStmt : allNextStmts) {
      nextTStmts.insert(nextStmt);

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

  return nextTStmts;
}

unordered_set<int> NextEvaluator::getPrevTStmts(int endStmt) {
  unordered_set<int> prevTStmts = {};
  queue<int> stmtQueue = {};
  unordered_map<int, unordered_set<int>> visited = {};

  // initialization
  unordered_set<int> allPrevStmtsToEnd =
      pkb->getLeft(RelationshipType::NEXT, endStmt);
  for (auto prevStmt : allPrevStmtsToEnd) {
    stmtQueue.push(prevStmt);
    visited.insert({prevStmt, {endStmt}});
    prevTStmts.insert(prevStmt);
  }

  // dequeue and process backwards reachable/ previous stmts
  while (!stmtQueue.empty()) {
    int currStmt = stmtQueue.front();
    stmtQueue.pop();
    unordered_set<int> allPrevStmts =
        pkb->getLeft(RelationshipType::NEXT, currStmt);

    for (int prevStmt : allPrevStmts) {
      prevTStmts.insert(prevStmt);

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

  return prevTStmts;
}
