#include "AffectsOnDemandEvaluator.h"

#include <Common/Global.h>

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace query;

AffectsOnDemandEvaluator::AffectsOnDemandEvaluator(PKB* pkb) {
  this->pkb = pkb;
}

/* Getter Methods -------------------------------------------------------- */
bool AffectsOnDemandEvaluator::isAffects(RelationshipType rsType, StmtNo a1,
                                         StmtNo a2) {
  return tableOfAffects[rsType].find(a1) != tableOfAffects[rsType].end() &&
         tableOfAffects[rsType].at(a1).find(a2) !=
             tableOfAffects[rsType].at(a1).end();
}

unordered_set<StmtNo> AffectsOnDemandEvaluator::getAffects(
    RelationshipType rsType, StmtNo a1) {
  if (tableOfAffects[rsType].find(a1) != tableOfAffects[rsType].end()) {
    return tableOfAffects[rsType].at(a1);
  }
  return {};
}

unordered_set<StmtNo> AffectsOnDemandEvaluator::getAffectsInv(
    RelationshipType rsType, StmtNo a2) {
  if (tableOfAffectsInv[rsType].find(a2) != tableOfAffectsInv[rsType].end()) {
    return tableOfAffectsInv[rsType].at(a2);
  }
  return {};
}

/* Evaluation Methods --------------------------------------------------- */
// Affects(1, 2), Affects(_, _), Affects(1, _), Affects(_, 2)
bool AffectsOnDemandEvaluator::evaluateBoolAffects(RelationshipType rsType,
                                                   const Param& left,
                                                   const Param& right) {
  vector<StmtNo> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

  if (left.type == ParamType::INTEGER_LITERAL &&
      right.type == ParamType::INTEGER_LITERAL) {
    int leftStmt = stoi(left.value);
    int rightStmt = stoi(right.value);
    if (isCompleteAffectsCache) {
      return isAffects(rsType, leftStmt, rightStmt);
    }
    // check incomplete cache
    if (isAffects(rsType, leftStmt, rightStmt)) {
      return true;
    }

    if (!pkb->isStmt(DesignEntity::ASSIGN, leftStmt) ||
        !pkb->isStmt(DesignEntity::ASSIGN, rightStmt)) {
      return false;
    }

    LastModifiedTable LMT = {};
    extractAffects(rsType, leftStmt, rightStmt, -1, &LMT,
                   BoolParamCombo::LITERALS);
    return isAffects(rsType, leftStmt, rightStmt);
  }

  if (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD) {
    if (isCompleteAffectsCache) {
      return !affectsStmtPairs[rsType].empty();
    }
    // check incomplete cache
    if (!affectsStmtPairs[rsType].empty()) {
      return true;
    }

    vector<StmtNo> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();
    for (auto firstStmt : firstStmtOfAllProcs) {
      LastModifiedTable LMT = {};
      extractAffects(rsType, firstStmt, -1, -1, &LMT,
                     BoolParamCombo::WILDCARDS);
      if (!affectsStmtPairs[rsType].empty()) {
        return true;
      }
    }
    return false;
  }

  if (left.type == ParamType::INTEGER_LITERAL) {
    StmtNo leftStmt = stoi(left.value);
    if (isCompleteAffectsCache) {
      return !getAffects(rsType, leftStmt).empty();
    }
    // check incomplete cache
    if (!getAffects(rsType, leftStmt).empty()) {
      return true;
    }

    if (!pkb->isStmt(DesignEntity::ASSIGN, leftStmt)) {
      return false;
    }

    LastModifiedTable LMT = {};
    extractAffects(rsType, leftStmt, -1, -1, &LMT,
                   BoolParamCombo::LITERAL_WILDCARD);
    return !getAffects(rsType, leftStmt).empty();
  }

  if (right.type == ParamType::INTEGER_LITERAL) {
    StmtNo rightStmt = stoi(right.value);
    if (isCompleteAffectsCache) {
      return !getAffectsInv(rsType, rightStmt).empty();
    }
    // check incomplete cache
    if (!getAffectsInv(rsType, rightStmt).empty()) {
      return true;
    }

    if (!pkb->isStmt(DesignEntity::ASSIGN, rightStmt)) {
      return false;
    }

    // check through all procs until a2 has been visited
    for (auto firstStmt : firstStmtOfAllProcs) {
      // skip proc if its first stmt is already larger than a2
      // means a2 is unreachable
      if (firstStmt > rightStmt) {
        continue;
      }

      LastModifiedTable LMT = {};
      extractAffects(rsType, firstStmt, rightStmt, -1, &LMT,
                     BoolParamCombo::WILDCARD_LITERAL);

      // if a2 visited, all Affects with a2 have been computed in this proc
      if (allVisitedStmts.find(rightStmt) != allVisitedStmts.end()) {
        break;
      }
    }
    return !getAffectsInv(rsType, rightStmt).empty();
  }

  DMOprintErrMsgAndExit("[AffectsOnDemandEvaluator] Invalid bool clause");
  return false;
}

// synonym & integer literal - Affects(a1, 2), Affects(1, a2)
unordered_set<StmtNo> AffectsOnDemandEvaluator::evaluateStmtAffects(
    RelationshipType rsType, const Param& left, const Param& right) {
  if (left.type == ParamType::INTEGER_LITERAL) {
    StmtNo leftStmt = stoi(left.value);
    if (isCompleteAffectsCache) {
      return getAffects(rsType, leftStmt);
    }
    if (!pkb->isStmt(DesignEntity::ASSIGN, leftStmt)) {
      return {};
    }

    LastModifiedTable LMT = {};
    extractAffects(rsType, leftStmt, -1, -1, &LMT, {});
    return getAffects(rsType, leftStmt);

  } else {
    StmtNo rightStmt = stoi(right.value);
    if (isCompleteAffectsCache) {
      return getAffectsInv(rsType, rightStmt);
    }
    if (!pkb->isStmt(DesignEntity::ASSIGN, rightStmt)) {
      return {};
    }

    vector<StmtNo> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();
    // check through all procs until a2 has been visited
    for (auto firstStmt : firstStmtOfAllProcs) {
      // skip proc if its first stmt is already larger than a2
      // means a2 is unreachable
      if (firstStmt > rightStmt) {
        continue;
      }

      LastModifiedTable LMT = {};
      extractAffects(rsType, firstStmt, -1, -1, &LMT, {});

      // if a2 visited, all Affects with a2 have been computed in this proc
      if (allVisitedStmts.find(rightStmt) != allVisitedStmts.end()) {
        break;
      }
    }
    return getAffectsInv(rsType, rightStmt);
  }
}

// Affects(a1, _), Affects(_, a2)
ClauseIncomingResults AffectsOnDemandEvaluator::evaluateSynonymWildcard(
    RelationshipType rsType, const Param& left, const Param& right) {
  if (isCompleteAffectsCache) {
    if (left.type == ParamType::SYNONYM) {
      return affectsLeftStmtPairs[rsType];
    } else {
      return affectsRightStmtPairs[rsType];
    }
  }
  vector<StmtNo> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

  // get all Affects and return either a1 or a2
  for (auto firstStmt : firstStmtOfAllProcs) {
    LastModifiedTable LMT = {};
    extractAffects(rsType, firstStmt, -1, -1, &LMT, {});
  }
  isCompleteAffectsCache = true;
  if (left.type == ParamType::SYNONYM) {
    return affectsLeftStmtPairs[rsType];
  } else {
    return affectsRightStmtPairs[rsType];
  }
}

// synonym & synonym - Affects(a1, a2)
ClauseIncomingResults AffectsOnDemandEvaluator::evaluatePairAffects(
    RelationshipType rsType, const Param& left, const Param& right) {
  if (left.type == ParamType::WILDCARD || right.type == ParamType::WILDCARD) {
    return evaluateSynonymWildcard(rsType, left, right);
  }

  if (isCompleteAffectsCache) {
    return affectsStmtPairs[rsType];
  }
  vector<StmtNo> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

  // get all Affects and return (a1, a2)
  for (auto firstStmt : firstStmtOfAllProcs) {
    LastModifiedTable LMT = {};
    extractAffects(rsType, firstStmt, -1, -1, &LMT, {});
  }
  isCompleteAffectsCache = true;
  return affectsStmtPairs[rsType];
}

bool AffectsOnDemandEvaluator::evaluateBoolAffectsT(const query::Param& left,
                                                    const query::Param& right) {
  if (left.type == ParamType::WILDCARD || right.type == ParamType::WILDCARD) {
    return evaluateBoolAffects(RelationshipType::AFFECTS, left, right);
  }

  // literals only
  if (isCompleteAffectsTCache) {
    return tableOfAffects[RelationshipType::AFFECTS_T]
               .at(stoi(left.value))
               .count(stoi(right.value)) > 0;
  }
  unordered_set<StmtNo> visited;
  return evalBoolLitAffectsT(left, right, &visited);
}

bool AffectsOnDemandEvaluator::evalBoolLitAffectsT(
    const query::Param& left, const query::Param& right,
    unordered_set<StmtNo>* visited) {
  bool hasFoundAffectsT =
      evaluateBoolAffects(RelationshipType::AFFECTS, left, right);
  bool allCombosTested = false;
  while (!hasFoundAffectsT && !allCombosTested) {
    unordered_set<StmtNo> affectedStmts;
    unordered_set<StmtNo> visitedStmts;
    try {
      affectedStmts =
          tableOfAffects[RelationshipType::AFFECTS].at(stoi(left.value));
    } catch (const out_of_range& e) {
      break;
    }
    for (StmtNo affectedStmt : affectedStmts) {
      if (affectedStmt == stoi(left.value) ||
          visited->count(affectedStmt) > 0) {
        continue;
      }
      visited->insert(affectedStmt);
      Param newParam{ParamType::INTEGER_LITERAL, to_string(affectedStmt)};
      hasFoundAffectsT = evalBoolLitAffectsT(newParam, right, visited);
      if (hasFoundAffectsT) {
        break;
      }
    }
    if (!hasFoundAffectsT) {
      allCombosTested = true;
    }
  }
  return hasFoundAffectsT;
}

// Affects(a, 2), (1, a)
unordered_set<StmtNo> AffectsOnDemandEvaluator::evaluateStmtAffectsT(
    const query::Param& left, const query::Param& right) {
  if (isCompleteAffectsTCache) {
    if (left.type == ParamType::INTEGER_LITERAL) {
      return tableOfAffects[RelationshipType::AFFECTS_T].at(stoi(left.value));
    } else {
      return tableOfAffectsInv[RelationshipType::AFFECTS_T].at(
          stoi(right.value));
    }
  }
  unordered_set<StmtNo> visited;
  return evaluateStmtAffectsTHelper(left, right, &visited);
}

unordered_set<StmtNo> AffectsOnDemandEvaluator::evaluateStmtAffectsTHelper(
    const query::Param& left, const query::Param& right,
    unordered_set<StmtNo>* visited) {
  unordered_set<StmtNo> result;
  unordered_set<StmtNo> firstLayer =
      evaluateStmtAffects(RelationshipType::AFFECTS, left, right);
  for (StmtNo affected : firstLayer) {
    if (visited->count(affected) > 0) {
      continue;
    }
    result.insert(affected);
    visited->insert(affected);
    Param newParam = {ParamType::INTEGER_LITERAL, to_string(affected)};
    if (left.type == ParamType::INTEGER_LITERAL) {
      if (stoi(left.value) == affected) {
        continue;
      }
      result.merge(evaluateStmtAffectsTHelper(newParam, right, visited));
    } else {
      if (stoi(right.value) == affected) {
        continue;
      }
      result.merge(evaluateStmtAffectsTHelper(left, newParam, visited));
    }
  }
  return result;
}

ClauseIncomingResults AffectsOnDemandEvaluator::evaluatePairAffectsT(
    const Param& left, const Param& right) {
  // Affects(a, _) or (_, a)
  if (left.type == ParamType::WILDCARD || right.type == ParamType::WILDCARD) {
    return evaluateSynonymWildcard(RelationshipType::AFFECTS, left, right);
  }

  // Affects(a1, a2)
  if (isCompleteAffectsTCache) {
    return affectsStmtPairs[RelationshipType::AFFECTS_T];
  }

  populateAffectsTCache(left, right);
  ClauseIncomingResults result;
  for (auto it : tableOfAffects[RelationshipType::AFFECTS_T]) {
    StmtNo curr = it.first;
    for (StmtNo affectedT : it.second) {
      vector<StmtNo> currPair{curr, affectedT};
      result.insert(currPair);
    }
  }
  affectsStmtPairs[RelationshipType::AFFECTS_T] = result;
  return result;
}

void AffectsOnDemandEvaluator::populateAffectsTCache(const Param& left,
                                                     const Param& right) {
  if (!isCompleteAffectsCache) {
    // ensure affects cache is populated first
    evaluatePairAffects(RelationshipType::AFFECTS, left, right);
  }
  populateAffectsTTable(&tableOfAffects[RelationshipType::AFFECTS_T],
                        &tableOfAffects[RelationshipType::AFFECTS]);
  populateAffectsTTable(&tableOfAffectsInv[RelationshipType::AFFECTS_T],
                        &tableOfAffectsInv[RelationshipType::AFFECTS]);
  isCompleteAffectsTCache = true;
}

void AffectsOnDemandEvaluator::populateAffectsTTable(
    unordered_map<StmtNo, unordered_set<StmtNo>>* target,
    unordered_map<StmtNo, unordered_set<StmtNo>>* base) {
  for (auto it : *base) {
    StmtNo currStmt = it.first;
    unordered_set<StmtNo> affectedStmts = it.second;
    unordered_set<StmtNo> visitedForCurr;
    for (StmtNo affectedStmt : affectedStmts) {
      populateAffectsTTableHelper(currStmt, affectedStmt, &visitedForCurr,
                                  base);
    }
    target->insert({currStmt, visitedForCurr});
  }
}

void AffectsOnDemandEvaluator::populateAffectsTTableHelper(
    StmtNo orig, StmtNo affected, unordered_set<StmtNo>* visited,
    unordered_map<StmtNo, unordered_set<StmtNo>>* base) {
  if (visited->count(affected) > 0) {
    return;
  }
  visited->insert(affected);
  if (orig != affected) {
    unordered_set<StmtNo> nextLayer;
    if (base->find(affected) != base->end()) {
      nextLayer = base->at(affected);
      for (StmtNo nextLayerAffects : nextLayer) {
        if (affected != nextLayerAffects) {
          populateAffectsTTableHelper(orig, nextLayerAffects, visited, base);
        }
      }
    }
  }
}

/* Affects Extraction Method ---------------------------------------------- */
// affects & affects bip?
void AffectsOnDemandEvaluator::extractAffects(RelationshipType rsType,
                                              StmtNo startStmt,
                                              StmtNo endStmt,
                                              StmtNo stmtAfterIfOrWhile,
                                              LastModifiedTable* LMT,
                                              BoolParamCombo paramCombo) {
  queue<int> stmtQueue = {};
  stmtQueue.push(startStmt);

  while (!stmtQueue.empty()) {
    int currStmt = stmtQueue.front();
    stmtQueue.pop();
    unordered_set<StmtNo> visitedIfAndWhile = {};

    if (currStmt == stmtAfterIfOrWhile || currStmt == -1) {
      // stop if finished processing of if/while block
      continue;
    }
    allVisitedStmts.insert(currStmt);

    if (pkb->isStmt(DesignEntity::READ, currStmt) ||
        pkb->isStmt(DesignEntity::CALL, currStmt)) {
      updateLastModifiedVariables(currStmt, LMT);
    }

    if (pkb->isStmt(DesignEntity::ASSIGN, currStmt)) {
      processAssignStmt(rsType, currStmt, LMT);
    }

    if (pkb->isStmt(DesignEntity::WHILE, currStmt)) {
      StmtNo firstStmtInWhile = currStmt + 1;
      processWhileLoop(rsType, firstStmtInWhile, endStmt, currStmt, LMT,
                       paramCombo);
      visitedIfAndWhile.insert(firstStmtInWhile);
    }

    if (pkb->isStmt(DesignEntity::IF, currStmt)) {
      StmtNo nextStmtForIf = pkb->getNextStmtForIfStmt(currStmt);
      unordered_set<StmtNo> thenElseStmts =
          pkb->getRight(getCFGRsType(rsType), currStmt);
      processThenElseBlocks(rsType, thenElseStmts, endStmt, nextStmtForIf, LMT,
                            paramCombo);
      for (StmtNo stmt : thenElseStmts) {
        visitedIfAndWhile.insert(stmt);
      }
      stmtQueue.push(nextStmtForIf);
    }

    unordered_set<int> allNextStmts =
        pkb->getRight(getCFGRsType(rsType), currStmt);
    for (int nextStmt : allNextStmts) {
      // prevent infinite loop in CFG - don't process if/while blocks again
      if (visitedIfAndWhile.find(nextStmt) == visitedIfAndWhile.end()) {
        stmtQueue.push(nextStmt);
      }
    }

    if (shouldTerminateBoolEarly(rsType, paramCombo, startStmt, endStmt)) {
      return;
    }
  }
}

/* Helper Methods For Affects Extraction ---------------------------- */
void AffectsOnDemandEvaluator::processAssignStmt(RelationshipType rsType,
                                                 StmtNo currStmt,
                                                 LastModifiedTable* LMT) {
  unordered_set<VarIdx> usedVars =
      pkb->getRight(RelationshipType::USES_S, currStmt);
  for (VarIdx usedVar : usedVars) {
    if (LMT->find(usedVar) != LMT->end()) {
      unordered_set<StmtNo> LMTStmts = LMT->at(usedVar);
      for (StmtNo LMTStmt : LMTStmts) {
        if (pkb->isStmt(DesignEntity::ASSIGN, LMTStmt)) {
          addAffectsRelationship(rsType, LMT, LMTStmt, currStmt);
        }
      }
    }
  }
  updateLastModifiedVariables(currStmt, LMT);
}

void AffectsOnDemandEvaluator::processWhileLoop(
    RelationshipType rsType, StmtNo firstStmtInWhile, StmtNo endStmt,
    StmtNo whileStmt, LastModifiedTable* LMT, BoolParamCombo paramCombo) {
  LastModifiedTable originalLMT = *LMT;
  LastModifiedTable beforeLMT;
  while (LMT->empty() || beforeLMT != *LMT) {
    beforeLMT = *LMT;
    extractAffects(rsType, firstStmtInWhile, endStmt, whileStmt, LMT,
                   paramCombo);
  }
  *LMT = mergeLMT(&originalLMT, LMT);
}

void AffectsOnDemandEvaluator::processThenElseBlocks(
    RelationshipType rsType, unordered_set<StmtNo> thenElseStmts,
    StmtNo endStmt, StmtNo nextStmtForIf, LastModifiedTable* LMT,
    BoolParamCombo paramCombo) {
  vector<StmtNo> stmts(thenElseStmts.begin(), thenElseStmts.end());
  vector<LastModifiedTable> thenElseLMTs = {*LMT, *LMT};

  // traverse else and then blocks
  for (int i = 0; i < stmts.size(); i++) {
    extractAffects(rsType, stmts[i], endStmt, nextStmtForIf, &thenElseLMTs[i],
                   paramCombo);
  }
  *LMT = mergeLMT(&thenElseLMTs.front(), &thenElseLMTs.back());
}

void AffectsOnDemandEvaluator::updateLastModifiedVariables(
    StmtNo currStmt, LastModifiedTable* LMT) {
  unordered_set<VarIdx> modifiedVars =
      pkb->getRight(RelationshipType::MODIFIES_S, currStmt);
  for (auto modifiedVar : modifiedVars) {
    if (LMT->find(modifiedVar) == LMT->end()) {
      LMT->insert({modifiedVar, {}});
    }
    LMT->at(modifiedVar) = {currStmt};
  }
}

void AffectsOnDemandEvaluator::addAffectsRelationship(RelationshipType rsType,
                                                      LastModifiedTable* LMT,
                                                      StmtNo LMTStmt,
                                                      StmtNo currStmt) {
  affectsStmts[rsType].insert(LMTStmt);
  affectsInvStmts[rsType].insert(currStmt);
  affectsLeftStmtPairs[rsType].insert({LMTStmt});
  affectsRightStmtPairs[rsType].insert({currStmt});
  affectsStmtPairs[rsType].insert({LMTStmt, currStmt});

  if (tableOfAffects[rsType].find(LMTStmt) == tableOfAffects[rsType].end()) {
    tableOfAffects[rsType].insert({LMTStmt, {}});
  }
  tableOfAffects[rsType].at(LMTStmt).insert(currStmt);

  if (tableOfAffectsInv[rsType].find(currStmt) ==
      tableOfAffectsInv[rsType].end()) {
    tableOfAffectsInv[rsType].insert({currStmt, {}});
  }
  tableOfAffectsInv[rsType].at(currStmt).insert(LMTStmt);
}

bool AffectsOnDemandEvaluator::shouldTerminateBoolEarly(
    RelationshipType rsType, BoolParamCombo paramCombo, StmtNo startStmt,
    StmtNo endStmt) {
  switch (paramCombo) {
    case BoolParamCombo::LITERALS:
      return tableOfAffects[rsType].find(startStmt) !=
                 tableOfAffects[rsType].end() &&
             tableOfAffects[rsType].at(startStmt).find(endStmt) !=
                 tableOfAffects[rsType].at(startStmt).end();
    case BoolParamCombo::WILDCARDS:
      return !affectsStmtPairs[rsType].empty();
    case BoolParamCombo::LITERAL_WILDCARD:
      return tableOfAffects[rsType].find(startStmt) !=
             tableOfAffects[rsType].end();
    case BoolParamCombo::WILDCARD_LITERAL:
      return tableOfAffectsInv[rsType].find(endStmt) !=
             tableOfAffectsInv[rsType].end();
    default:
      // early termination here only for boolean clauses
      return false;
  }
}

LastModifiedTable AffectsOnDemandEvaluator::mergeLMT(
    LastModifiedTable* firstLMT, LastModifiedTable* secondLMT) {
  LastModifiedTable finalLMT = {};
  unordered_set<VarIdx> allVariables = {};
  for (auto varStmtSetPair : *firstLMT) {
    allVariables.insert(varStmtSetPair.first);
  }
  for (auto varStmtSetPair : *secondLMT) {
    allVariables.insert(varStmtSetPair.first);
  }

  for (auto varIdx : allVariables) {
    unordered_set<StmtNo> stmtSet = {};
    if (firstLMT->find(varIdx) != firstLMT->end()) {
      stmtSet.merge(firstLMT->at(varIdx));
    }
    if (secondLMT->find(varIdx) != secondLMT->end()) {
      stmtSet.merge(secondLMT->at(varIdx));
    }
    finalLMT.insert({varIdx, stmtSet});
  }
  return finalLMT;
}

RelationshipType AffectsOnDemandEvaluator::getCFGRsType(
    RelationshipType rsType) {
  if (rsType == RelationshipType::AFFECTS) {
    return RelationshipType::NEXT;
  } else if (rsType == RelationshipType::AFFECTS_BIP) {
    return RelationshipType::NEXT_BIP;
  } else {
    DMOprintErrMsgAndExit(
        "[AffectsOnDemandEvaluator][getCFGRsType] Invalid rsType");
    return {};
  }
}
