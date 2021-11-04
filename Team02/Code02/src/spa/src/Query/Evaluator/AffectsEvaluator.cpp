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
bool AffectsEvaluator::isAffects(STMT_NO a1, STMT_NO a2) {
  return tableOfAffects.find(a1) != tableOfAffects.end() &&
         tableOfAffects.at(a1).find(a2) != tableOfAffects.at(a1).end();
}

unordered_set<STMT_NO> AffectsEvaluator::getAffects(STMT_NO a1) {
  if (tableOfAffects.find(a1) != tableOfAffects.end()) {
    return tableOfAffects.at(a1);
  }
  return {};
}

unordered_set<STMT_NO> AffectsEvaluator::getAffectsInv(STMT_NO a2) {
  if (tableOfAffectsInv.find(a2) != tableOfAffectsInv.end()) {
    return tableOfAffectsInv.at(a2);
  }
  return {};
}

/* Evaluation Methods --------------------------------------------------- */
// Affects(1, 2), Affects(_, _), Affects(1, _), Affects(_, 2)
bool AffectsEvaluator::evaluateBoolAffects(const Param& left,
                                           const Param& right) {
  vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

  if (left.type == ParamType::INTEGER_LITERAL &&
      right.type == ParamType::INTEGER_LITERAL) {
    int leftStmt = stoi(left.value);
    int rightStmt = stoi(right.value);
    if (isCompleteCache) {
      return isAffects(leftStmt, rightStmt);
    }
    // check incomplete cache
    if (isAffects(leftStmt, rightStmt)) {
      return true;
    }

    if (!pkb->isStmt(DesignEntity::ASSIGN, leftStmt) ||
        !pkb->isStmt(DesignEntity::ASSIGN, rightStmt)) {
      return false;
    }

    LastModifiedTable LMT = {};
    extractAffects(leftStmt, rightStmt, -1, &LMT, BoolParamCombo::LITERALS);
    return isAffects(leftStmt, rightStmt);
  }

  if (left.type == ParamType::WILDCARD && right.type == ParamType::WILDCARD) {
    if (isCompleteCache) {
      return !affectsStmtPairs.empty();
    }
    // check incomplete cache
    if (!affectsStmtPairs.empty()) {
      return true;
    }

    vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();
    for (auto firstStmt : firstStmtOfAllProcs) {
      LastModifiedTable LMT = {};
      extractAffects(firstStmt, -1, -1, &LMT, BoolParamCombo::WILDCARDS);
      if (!affectsStmtPairs.empty()) {
        return true;
      }
    }
    return false;
  }

  if (left.type == ParamType::INTEGER_LITERAL) {
    STMT_NO leftStmt = stoi(left.value);
    if (isCompleteCache) {
      return !getAffects(leftStmt).empty();
    }
    // check incomplete cache
    if (!getAffects(leftStmt).empty()) {
      return true;
    }

    if (!pkb->isStmt(DesignEntity::ASSIGN, leftStmt)) {
      return false;
    }

    LastModifiedTable LMT = {};
    extractAffects(leftStmt, -1, -1, &LMT, BoolParamCombo::LITERAL_WILDCARD);
    return !getAffects(leftStmt).empty();
  }

  if (right.type == ParamType::INTEGER_LITERAL) {
    STMT_NO rightStmt = stoi(right.value);
    if (isCompleteCache) {
      return !getAffectsInv(rightStmt).empty();
    }
    // check incomplete cache
    if (!getAffectsInv(rightStmt).empty()) {
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
      extractAffects(firstStmt, rightStmt, -1, &LMT,
                     BoolParamCombo::WILDCARD_LITERAL);

      // if a2 visited, all Affects with a2 have been computed in this proc
      if (allVisitedStmts.find(rightStmt) != allVisitedStmts.end()) {
        break;
      }
    }
    return !getAffectsInv(rightStmt).empty();
  }

  DMOprintErrMsgAndExit("[AffectsEvaluator] Invalid bool clause");
  return false;
}

unordered_set<STMT_NO> AffectsEvaluator::evaluateStmtAffects(
    const Param& left, const Param& right) {
  if (left.type == ParamType::INTEGER_LITERAL ||
      right.type == ParamType::INTEGER_LITERAL) {
    return evaluateSynonymLiteral(left, right);
  } else {
    return evaluateSynonymWildcard(left, right);
  }
}

// synonym & integer literal - Affects(a1, 2), Affects(1, a2)
unordered_set<STMT_NO> AffectsEvaluator::evaluateSynonymLiteral(
    const Param& left, const Param& right) {
  if (left.type == ParamType::INTEGER_LITERAL) {
    STMT_NO leftStmt = stoi(left.value);
    if (isCompleteCache) {
      return getAffects(leftStmt);
    }
    if (!pkb->isStmt(DesignEntity::ASSIGN, leftStmt)) {
      return {};
    }

    LastModifiedTable LMT = {};
    extractAffects(leftStmt, -1, -1, &LMT, {});
    return getAffects(leftStmt);

  } else {
    STMT_NO rightStmt = stoi(right.value);
    if (isCompleteCache) {
      return getAffectsInv(rightStmt);
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
      extractAffects(firstStmt, -1, -1, &LMT, {});

      // if a2 visited, all Affects with a2 have been computed in this proc
      if (allVisitedStmts.find(rightStmt) != allVisitedStmts.end()) {
        break;
      }
    }
    return getAffectsInv(rightStmt);
  }
}

// Affects(a1, _), Affects(_, a2)
unordered_set<STMT_NO> AffectsEvaluator::evaluateSynonymWildcard(
    const Param& left, const Param& right) {
  if (isCompleteCache) {
    if (left.type == ParamType::SYNONYM) {
      return affectsStmts;
    } else {
      return affectsInvStmts;
    }
  }
  vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

  // get all Affects and return either a1 or a2
  for (auto firstStmt : firstStmtOfAllProcs) {
    LastModifiedTable LMT = {};
    extractAffects(firstStmt, -1, -1, &LMT, {});
  }
  isCompleteCache = true;
  if (left.type == ParamType::SYNONYM) {
    return affectsStmts;
  } else {
    return affectsInvStmts;
  }
}

// synonym & synonym - Affects(a1, a2)
vector<vector<STMT_NO>> AffectsEvaluator::evaluatePairAffects() {
  if (isCompleteCache) {
    return affectsStmtPairs;
  }
  vector<STMT_NO> firstStmtOfAllProcs = pkb->getFirstStmtOfAllProcs();

  // get all Affects and return (a1, a2)
  for (auto firstStmt : firstStmtOfAllProcs) {
    LastModifiedTable LMT = {};
    extractAffects(firstStmt, -1, -1, &LMT, {});
  }
  isCompleteCache = true;
  return affectsStmtPairs;
}

bool AffectsEvaluator::evaluateBoolAffectsT(const query::Param& left,
                                            const query::Param& right) {
  if (left.type == ParamType::WILDCARD || right.type == ParamType::WILDCARD) {
    return evaluateBoolAffects(left, right);
  }

  // literals only
  if (isCompleteCacheT) {
    return tableOfAffectsT.at(stoi(left.value)).count(stoi(right.value)) > 0;
  }
  unordered_set<STMT_NO> visited;
  return evalBoolLitAffectsT(left, right, &visited);
}

bool AffectsEvaluator::evalBoolLitAffectsT(const query::Param& left,
                                           const query::Param& right,
                                           unordered_set<STMT_NO>* visited) {
  bool hasFoundAffectsT = evaluateBoolAffects(left, right);
  bool allCombosTested = false;
  while (!hasFoundAffectsT && !allCombosTested) {
    unordered_set<STMT_NO> affectedStmts;
    unordered_set<STMT_NO> visitedStmts;
    try {
      affectedStmts = tableOfAffects.at(stoi(left.value));
    }
    catch (const out_of_range& e) {
      break;
    }
    for (STMT_NO affectedStmt : affectedStmts) {
      if (affectedStmt == stoi(left.value) ||
          visited->count(affectedStmt) > 0) {
        continue;
      }
      visited->insert(affectedStmt);
      Param newParam{ ParamType::INTEGER_LITERAL, to_string(affectedStmt) };
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

unordered_set<STMT_NO> AffectsEvaluator::evaluateStmtAffectsT(
    const query::Param& left, const query::Param& right) {
  if (left.type == ParamType::WILDCARD || right.type == ParamType::WILDCARD) {
    return evaluateSynonymWildcard(left, right);
  }
  if (isCompleteCacheT) {
    if (left.type == ParamType::INTEGER_LITERAL) {
      return tableOfAffectsT.at(stoi(left.value));
    } else {
      return tableOfAffectsTInv.at(stoi(right.value));
    }
  }
  unordered_set<STMT_NO> visited;
  return evaluateSynonymLiteralT(left, right, &visited);
}

unordered_set<STMT_NO> AffectsEvaluator::evaluateSynonymLiteralT(
    const query::Param& left, const query::Param& right,
    unordered_set<STMT_NO>* visited) {
  unordered_set<STMT_NO> result;
  unordered_set<STMT_NO> firstLayer = evaluateSynonymLiteral(left, right);
  for (STMT_NO affected : firstLayer) {
    if (visited->count(affected) > 0) {
      continue;
    }
    result.insert(affected);
    visited->insert(affected);
    Param newParam = { ParamType::INTEGER_LITERAL, to_string(affected) };
    if (left.type == ParamType::INTEGER_LITERAL) {
      if (stoi(left.value) == affected) {
        continue;
      }
      result.merge(evaluateSynonymLiteralT(newParam, right, visited));
    } else {
      if (stoi(right.value) == affected) {
        continue;
      }
      result.merge(evaluateSynonymLiteralT(left, newParam, visited));
    }
  }
  return result;
}

vector<vector<STMT_NO>> AffectsEvaluator::evaluatePairAffectsT() {
  populateCacheT();
  vector<vector<STMT_NO>> result;
  for (auto it : tableOfAffectsT) {
    STMT_NO curr = it.first;
    for (STMT_NO affectedT : it.second) {
      vector<STMT_NO> currPair{ curr, affectedT };
      result.push_back(currPair);
    }
  }
  return result;
}

void AffectsEvaluator::populateCacheT() {
  evaluatePairAffects();
  populateTable(&tableOfAffectsT, &tableOfAffects);
  populateTable(&tableOfAffectsTInv, &tableOfAffectsInv);
  isCompleteCacheT = true;
}

void AffectsEvaluator::populateTable(
    unordered_map<STMT_NO, unordered_set<STMT_NO>>* target,
    unordered_map<STMT_NO, unordered_set<STMT_NO>>* base) {
  for (auto it : *base) {
    STMT_NO currStmt = it.first;
    unordered_set<STMT_NO> affectedStmts = it.second;
    unordered_set<STMT_NO> visitedForCurr;
    for (STMT_NO affectedStmt : affectedStmts) {
      populateTableHelper(currStmt, affectedStmt, &visitedForCurr, base);
    }
    target->insert({currStmt, visitedForCurr});
  }
}

void AffectsEvaluator::populateTableHelper(STMT_NO orig, STMT_NO affected,
    unordered_set<STMT_NO>* visited,
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
          populateTableHelper(orig, nextLayerAffects, visited, base);
        }
      }
    }
  }
}

/* Affects Extraction Method ---------------------------------------------- */
void AffectsEvaluator::extractAffects(STMT_NO startStmt, STMT_NO endStmt,
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
      processAssignStmt(currStmt, LMT);
    }

    if (pkb->isStmt(DesignEntity::WHILE, currStmt)) {
      STMT_NO firstStmtInWhile = currStmt + 1;
      processWhileLoop(firstStmtInWhile, endStmt, currStmt, LMT, paramCombo);
      visitedIfAndWhile.insert(firstStmtInWhile);
    }

    if (pkb->isStmt(DesignEntity::IF, currStmt)) {
      STMT_NO nextStmtForIf = pkb->getNextStmtForIfStmt(currStmt);
      unordered_set<STMT_NO> thenElseStmts =
          pkb->getRight(RelationshipType::NEXT, currStmt);
      processThenElseBlocks(thenElseStmts, endStmt, nextStmtForIf, LMT,
                            paramCombo);
      for (STMT_NO stmt : thenElseStmts) {
        visitedIfAndWhile.insert(stmt);
      }
      stmtQueue.push(nextStmtForIf);
    }

    unordered_set<int> allNextStmts =
        pkb->getRight(RelationshipType::NEXT, currStmt);
    for (int nextStmt : allNextStmts) {
      // prevent infinite loop in CFG - don't process if/while blocks again
      if (visitedIfAndWhile.find(nextStmt) == visitedIfAndWhile.end()) {
        stmtQueue.push(nextStmt);
      }
    }

    if (shouldTerminateBoolEarly(paramCombo, startStmt, endStmt)) {
      return;
    }
  }
}

/* Helper Methods For Affects Extraction ---------------------------- */
void AffectsEvaluator::processAssignStmt(STMT_NO currStmt,
                                         LastModifiedTable* LMT) {
  unordered_set<VarIdx> usedVars =
      pkb->getRight(RelationshipType::USES_S, currStmt);
  for (VarIdx usedVar : usedVars) {
    if (LMT->find(usedVar) != LMT->end()) {
      unordered_set<STMT_NO> LMTStmts = LMT->at(usedVar);
      for (STMT_NO LMTStmt : LMTStmts) {
        if (pkb->isStmt(DesignEntity::ASSIGN, LMTStmt)) {
          addAffectsRelationship(LMT, LMTStmt, currStmt);
        }
      }
    }
  }
  updateLastModifiedVariables(currStmt, LMT);
}

void AffectsEvaluator::processWhileLoop(STMT_NO firstStmtInWhile,
                                        STMT_NO endStmt, STMT_NO whileStmt,
                                        LastModifiedTable* LMT,
                                        BoolParamCombo paramCombo) {
  LastModifiedTable originalLMT = *LMT;
  LastModifiedTable beforeLMT;
  while (LMT->empty() || beforeLMT != *LMT) {
    beforeLMT = *LMT;
    extractAffects(firstStmtInWhile, endStmt, whileStmt, LMT, paramCombo);
  }
  *LMT = mergeLMT(&originalLMT, LMT);
}

void AffectsEvaluator::processThenElseBlocks(
    unordered_set<STMT_NO> thenElseStmts, STMT_NO endStmt,
    STMT_NO nextStmtForIf, LastModifiedTable* LMT, BoolParamCombo paramCombo) {
  vector<STMT_NO> stmts(thenElseStmts.begin(), thenElseStmts.end());
  vector<LastModifiedTable> thenElseLMTs = {*LMT, *LMT};

  // traverse else and then blocks
  for (int i = 0; i < stmts.size(); i++) {
    extractAffects(stmts[i], endStmt, nextStmtForIf, &thenElseLMTs[i],
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

void AffectsEvaluator::addAffectsRelationship(LastModifiedTable* LMT,
                                              STMT_NO LMTStmt,
                                              STMT_NO currStmt) {
  affectsStmts.insert(LMTStmt);
  affectsInvStmts.insert(currStmt);
  affectsStmtPairs.push_back({LMTStmt, currStmt});

  if (tableOfAffects.find(LMTStmt) == tableOfAffects.end()) {
    tableOfAffects.insert({LMTStmt, {}});
  }
  tableOfAffects.at(LMTStmt).insert(currStmt);

  if (tableOfAffectsInv.find(currStmt) == tableOfAffectsInv.end()) {
    tableOfAffectsInv.insert({currStmt, {}});
  }
  tableOfAffectsInv.at(currStmt).insert(LMTStmt);
}

bool AffectsEvaluator::shouldTerminateBoolEarly(BoolParamCombo paramCombo,
                                                STMT_NO startStmt,
                                                STMT_NO endStmt) {
  switch (paramCombo) {
    case BoolParamCombo::LITERALS:
      return tableOfAffects.find(startStmt) != tableOfAffects.end() &&
             tableOfAffects.at(startStmt).find(endStmt) !=
                 tableOfAffects.at(startStmt).end();
    case BoolParamCombo::WILDCARDS:
      return !affectsStmtPairs.empty();
    case BoolParamCombo::LITERAL_WILDCARD:
      return tableOfAffects.find(startStmt) != tableOfAffects.end();
    case BoolParamCombo::WILDCARD_LITERAL:
      return tableOfAffectsInv.find(endStmt) != tableOfAffectsInv.end();
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
