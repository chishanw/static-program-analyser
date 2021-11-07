#include "AffectsEvaluator.h"

#include <Common/Global.h>

#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace query;

AffectsEvaluator::AffectsEvaluator(PKB* pkb) { this->pkb = pkb; }

/* Getter Methods -------------------------------------------------------- */
bool AffectsEvaluator::isAffects(RelationshipType rsType, STMT_NO a1,
                                 STMT_NO a2) {
  return tableOfAffects[rsType].find(a1) != tableOfAffects[rsType].end() &&
         tableOfAffects[rsType].at(a1).find(a2) !=
             tableOfAffects[rsType].at(a1).end();
}

unordered_set<STMT_NO> AffectsEvaluator::getAffects(RelationshipType rsType,
                                                    STMT_NO a1) {
  if (tableOfAffects[rsType].find(a1) != tableOfAffects[rsType].end()) {
    return tableOfAffects[rsType].at(a1);
  }
  return {};
}

unordered_set<STMT_NO> AffectsEvaluator::getAffectsInv(RelationshipType rsType,
                                                       STMT_NO a2) {
  if (tableOfAffectsInv[rsType].find(a2) != tableOfAffectsInv[rsType].end()) {
    return tableOfAffectsInv[rsType].at(a2);
  }
  return {};
}

/* Evaluation Methods --------------------------------------------------- */
// Affects(1, 2), Affects(_, _), Affects(1, _), Affects(_, 2)
bool AffectsEvaluator::evaluateBoolAffects(RelationshipType rsType,
                                           const Param& left,
                                           const Param& right) {
  vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

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

    vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();
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
    STMT_NO leftStmt = stoi(left.value);
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
    STMT_NO rightStmt = stoi(right.value);
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

  DMOprintErrMsgAndExit("[AffectsEvaluator] Invalid bool clause");
  return false;
}

// synonym & integer literal - Affects(a1, 2), Affects(1, a2)
unordered_set<STMT_NO> AffectsEvaluator::evaluateStmtAffects(
    RelationshipType rsType, const Param& left, const Param& right) {
  if (left.type == ParamType::INTEGER_LITERAL) {
    STMT_NO leftStmt = stoi(left.value);
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
    STMT_NO rightStmt = stoi(right.value);
    if (isCompleteAffectsCache) {
      return getAffectsInv(rsType, rightStmt);
    }
    if (!pkb->isStmt(DesignEntity::ASSIGN, rightStmt)) {
      return {};
    }

    vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();
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
ClauseIncomingResults AffectsEvaluator::evaluateSynonymWildcard(
    RelationshipType rsType, const Param& left, const Param& right) {
  if (isCompleteAffectsCache) {
    if (left.type == ParamType::SYNONYM) {
      return affectsLeftStmtPairs[rsType];
    } else {
      return affectsRightStmtPairs[rsType];
    }
  }
  vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

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
ClauseIncomingResults AffectsEvaluator::evaluatePairAffects(
    RelationshipType rsType, const Param& left, const Param& right) {
  if (left.type == ParamType::WILDCARD || right.type == ParamType::WILDCARD) {
    return evaluateSynonymWildcard(rsType, left, right);
  }

  if (isCompleteAffectsCache) {
    return affectsStmtPairs[rsType];
  }
  vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

  // get all Affects and return (a1, a2)
  for (auto firstStmt : firstStmtOfAllProcs) {
    LastModifiedTable LMT = {};
    extractAffects(rsType, firstStmt, -1, -1, &LMT, {});
  }
  isCompleteAffectsCache = true;
  return affectsStmtPairs[rsType];
}

bool AffectsEvaluator::evaluateBoolAffectsT(const query::Param& left,
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
  unordered_set<STMT_NO> visited;
  return evalBoolLitAffectsT(left, right, &visited);
}

bool AffectsEvaluator::evalBoolLitAffectsT(const query::Param& left,
                                           const query::Param& right,
                                           unordered_set<STMT_NO>* visited) {
  bool hasFoundAffectsT =
      evaluateBoolAffects(RelationshipType::AFFECTS, left, right);
  bool allCombosTested = false;
  while (!hasFoundAffectsT && !allCombosTested) {
    unordered_set<STMT_NO> affectedStmts;
    unordered_set<STMT_NO> visitedStmts;
    try {
      affectedStmts =
          tableOfAffects[RelationshipType::AFFECTS].at(stoi(left.value));
    } catch (const out_of_range& e) {
      break;
    }
    for (STMT_NO affectedStmt : affectedStmts) {
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
unordered_set<STMT_NO> AffectsEvaluator::evaluateStmtAffectsT(
    const query::Param& left, const query::Param& right) {
  if (isCompleteAffectsTCache) {
    if (left.type == ParamType::INTEGER_LITERAL) {
      return tableOfAffects[RelationshipType::AFFECTS_T].at(stoi(left.value));
    } else {
      return tableOfAffectsInv[RelationshipType::AFFECTS_T].at(
          stoi(right.value));
    }
  }
  unordered_set<STMT_NO> visited;
  return evaluateStmtAffectsTHelper(left, right, &visited);
}

unordered_set<STMT_NO> AffectsEvaluator::evaluateStmtAffectsTHelper(
    const query::Param& left, const query::Param& right,
    unordered_set<STMT_NO>* visited) {
  unordered_set<STMT_NO> result;
  unordered_set<STMT_NO> firstLayer =
      evaluateStmtAffects(RelationshipType::AFFECTS, left, right);
  for (STMT_NO affected : firstLayer) {
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

ClauseIncomingResults AffectsEvaluator::evaluatePairAffectsT(
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
    STMT_NO curr = it.first;
    for (STMT_NO affectedT : it.second) {
      vector<STMT_NO> currPair{curr, affectedT};
      result.insert(currPair);
    }
  }
  affectsStmtPairs[RelationshipType::AFFECTS_T] = result;
  return result;
}

void AffectsEvaluator::populateAffectsTCache(const Param& left,
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

void AffectsEvaluator::populateAffectsTTable(
    unordered_map<STMT_NO, unordered_set<STMT_NO>>* target,
    unordered_map<STMT_NO, unordered_set<STMT_NO>>* base) {
  for (auto it : *base) {
    STMT_NO currStmt = it.first;
    unordered_set<STMT_NO> affectedStmts = it.second;
    unordered_set<STMT_NO> visitedForCurr;
    for (STMT_NO affectedStmt : affectedStmts) {
      populateAffectsTTableHelper(currStmt, affectedStmt, &visitedForCurr,
                                  base);
    }
    target->insert({currStmt, visitedForCurr});
  }
}

void AffectsEvaluator::populateAffectsTTableHelper(
    STMT_NO orig, STMT_NO affected, unordered_set<STMT_NO>* visited,
    unordered_map<STMT_NO, unordered_set<STMT_NO>>* base) {
  if (visited->count(affected) > 0) {
    return;
  }
  visited->insert(affected);
  if (orig != affected) {
    unordered_set<STMT_NO> nextLayer;
    if (base->find(affected) != base->end()) {
      nextLayer = base->at(affected);
      for (STMT_NO nextLayerAffects : nextLayer) {
        if (affected != nextLayerAffects) {
          populateAffectsTTableHelper(orig, nextLayerAffects, visited, base);
        }
      }
    }
  }
}

/* Affects Extraction Method ---------------------------------------------- */
// affects & affects bip?
void AffectsEvaluator::extractAffects(RelationshipType rsType,
                                      STMT_NO startStmt, STMT_NO endStmt,
                                      STMT_NO stmtAfterIfOrWhile,
                                      LastModifiedTable* LMT,
                                      BoolParamCombo paramCombo) {
  queue<int> stmtQueue = {};
  stmtQueue.push(startStmt);

  while (!stmtQueue.empty()) {
    int currStmt = stmtQueue.front();
    stmtQueue.pop();
    unordered_set<STMT_NO> visitedIfAndWhile = {};

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
      STMT_NO firstStmtInWhile = currStmt + 1;
      processWhileLoop(rsType, firstStmtInWhile, endStmt, currStmt, LMT,
                       paramCombo);
      visitedIfAndWhile.insert(firstStmtInWhile);
    }

    if (pkb->isStmt(DesignEntity::IF, currStmt)) {
      STMT_NO nextStmtForIf = pkb->getNextStmtForIfStmt(currStmt);
      unordered_set<STMT_NO> thenElseStmts =
          pkb->getRight(getCFGRsType(rsType), currStmt);
      processThenElseBlocks(rsType, thenElseStmts, endStmt, nextStmtForIf, LMT,
                            paramCombo);
      for (STMT_NO stmt : thenElseStmts) {
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
void AffectsEvaluator::processAssignStmt(RelationshipType rsType,
                                         STMT_NO currStmt,
                                         LastModifiedTable* LMT) {
  unordered_set<VarIdx> usedVars =
      pkb->getRight(RelationshipType::USES_S, currStmt);
  for (VarIdx usedVar : usedVars) {
    if (LMT->find(usedVar) != LMT->end()) {
      unordered_set<STMT_NO> LMTStmts = LMT->at(usedVar);
      for (STMT_NO LMTStmt : LMTStmts) {
        if (pkb->isStmt(DesignEntity::ASSIGN, LMTStmt)) {
          addAffectsRelationship(rsType, LMT, LMTStmt, currStmt);
        }
      }
    }
  }
  updateLastModifiedVariables(currStmt, LMT);
}

void AffectsEvaluator::processWhileLoop(RelationshipType rsType,
                                        STMT_NO firstStmtInWhile,
                                        STMT_NO endStmt, STMT_NO whileStmt,
                                        LastModifiedTable* LMT,
                                        BoolParamCombo paramCombo) {
  LastModifiedTable originalLMT = *LMT;
  LastModifiedTable beforeLMT;
  while (LMT->empty() || beforeLMT != *LMT) {
    beforeLMT = *LMT;
    extractAffects(rsType, firstStmtInWhile, endStmt, whileStmt, LMT,
                   paramCombo);
  }
  *LMT = mergeLMT(&originalLMT, LMT);
}

void AffectsEvaluator::processThenElseBlocks(
    RelationshipType rsType, unordered_set<STMT_NO> thenElseStmts,
    STMT_NO endStmt, STMT_NO nextStmtForIf, LastModifiedTable* LMT,
    BoolParamCombo paramCombo) {
  vector<STMT_NO> stmts(thenElseStmts.begin(), thenElseStmts.end());
  vector<LastModifiedTable> thenElseLMTs = {*LMT, *LMT};

  // traverse else and then blocks
  for (int i = 0; i < stmts.size(); i++) {
    extractAffects(rsType, stmts[i], endStmt, nextStmtForIf, &thenElseLMTs[i],
                   paramCombo);
  }
  *LMT = mergeLMT(&thenElseLMTs.front(), &thenElseLMTs.back());
}

void AffectsEvaluator::updateLastModifiedVariables(STMT_NO currStmt,
                                                   LastModifiedTable* LMT) {
  unordered_set<VarIdx> modifiedVars =
      pkb->getRight(RelationshipType::MODIFIES_S, currStmt);
  for (auto modifiedVar : modifiedVars) {
    if (LMT->find(modifiedVar) == LMT->end()) {
      LMT->insert({modifiedVar, {}});
    }
    LMT->at(modifiedVar) = {currStmt};
  }
}

void AffectsEvaluator::addAffectsRelationship(RelationshipType rsType,
                                              LastModifiedTable* LMT,
                                              STMT_NO LMTStmt,
                                              STMT_NO currStmt) {
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

bool AffectsEvaluator::shouldTerminateBoolEarly(RelationshipType rsType,
                                                BoolParamCombo paramCombo,
                                                STMT_NO startStmt,
                                                STMT_NO endStmt) {
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

LastModifiedTable AffectsEvaluator::mergeLMT(LastModifiedTable* firstLMT,
                                             LastModifiedTable* secondLMT) {
  LastModifiedTable finalLMT = {};
  unordered_set<VarIdx> allVariables = {};
  for (auto varStmtSetPair : *firstLMT) {
    allVariables.insert(varStmtSetPair.first);
  }
  for (auto varStmtSetPair : *secondLMT) {
    allVariables.insert(varStmtSetPair.first);
  }

  for (auto varIdx : allVariables) {
    unordered_set<STMT_NO> stmtSet = {};
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

RelationshipType AffectsEvaluator::getCFGRsType(RelationshipType rsType) {
  if (rsType == RelationshipType::AFFECTS) {
    return RelationshipType::NEXT;
  } else if (rsType == RelationshipType::AFFECTS_BIP) {
    return RelationshipType::NEXT_BIP;
  } else {
    DMOprintErrMsgAndExit("[AffectsEvaluator][getCFGRsType] Invalid rsType");
    return {};
  }
}
