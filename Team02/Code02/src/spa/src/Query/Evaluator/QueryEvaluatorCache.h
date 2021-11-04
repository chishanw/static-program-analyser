#pragma once

#include <Common/Common.h>
#include <Query/Common.h>

#include <unordered_map>

typedef std::unordered_map<RelationshipType, std::unordered_map<int, SetOfInts>>
    TablesRs;
typedef std::unordered_map<RelationshipType,
                           std::unordered_map<ParamPosition, SetOfStmtLists>>
    MappingsRs;
// TODO(cs) make TablesRs and MappingRs from PKB common as well

// Cache for on-demand relationships
class QueryEvaluatorCache {
 public:
  QueryEvaluatorCache();

  // setters
  void addToStmtsCache(RelationshipType rsType, StmtNo leftStmt,
                       SetOfInts rightStmts);
  void addToInvStmtsCache(RelationshipType rsType, StmtNo leftStmt,
                          SetOfInts rightStmts);

  // getters
  bool isStmtInStmtsCache(RelationshipType rsType, StmtNo leftStmt);
  bool isStmtInInvStmtsCache(RelationshipType rsType, StmtNo rightStmt);
  bool isRelationship(RelationshipType rsType, StmtNo left, StmtNo right);

  SetOfInts& getStmts(RelationshipType rsType, StmtNo stmt);
  SetOfInts& getInvStmts(RelationshipType rsType, StmtNo stmt);

 private:
  TablesRs stmtToStmtsCache;
  TablesRs invStmtToStmtsCache;
};
