#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Type def, PascalCase
typedef int StmtNo, VarIdx, ProcIdx, ConstIdx, ExprIdx, TableElemIdx;
typedef std::string VarName, ProcName;
typedef std::unordered_set<int> SetOfStmts, SetOfInts;
typedef std::vector<int> ListOfInts, ListOfStmtNos;

// ENUM
enum class DesignEntity {
  STATEMENT,
  READ,
  PRINT,
  CALL,
  WHILE,
  IF,
  ASSIGN,
  VARIABLE,
  CONSTANT,
  PROCEDURE,
  PROG_LINE
};

enum class RelationshipType {
  FOLLOWS,
  FOLLOWS_T,
  PARENT,
  PARENT_T,
  USES_S,
  USES_P,
  MODIFIES_S,
  MODIFIES_P,
  CALLS,
  CALLS_T,
  CALLS_S,
  NEXT,
  NEXT_T,
  NEXT_BIP,
  NEXT_BIP_T,
  AFFECTS,
  AFFECTS_T,
  AFFECTS_BIP,
  AFFECTS_BIP_T,
  PTT_ASSIGN_FULL_EXPR,
  PTT_ASSIGN_SUB_EXPR,
  PTT_IF,
  PTT_WHILE,
};

enum class ParamPosition { LEFT, RIGHT, BOTH };

enum class TableType { VAR_TABLE, CONST_TABLE, PROC_TABLE, EXPR_TABLE };

// Hashing Functions
struct VectorHash {
  size_t operator()(const std::vector<int>& v) const {
    std::hash<int> hasher;
    size_t seed = 0;
    for (int i : v) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

struct PairHash {
  size_t operator()(const std::pair<int, int>& p) const {
    std::hash<int> hasher;
    size_t seed = 0;
    seed ^= hasher(p.first) + 0x9e3779b9 + (hasher(p.second) << 6) +
            (hasher(p.second) >> 2);
    return seed;
  }
};

struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

// Define Set of Stmt Lists
typedef std::unordered_set<std::vector<int>, VectorHash> SetOfStmtLists,
    SetOfIntLists;
