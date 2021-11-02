# pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

// Type def, PascalCase
typedef int StmtNo, VarIdx, ProcIdx, ConstIdx, TableElemIdx;
typedef std::string VarName, ProcName;
typedef std::unordered_set<int> SetOfStmts, SetOfInts;
typedef std::vector<int> ListOfInts, ListOfStmtNos;

// ENUM
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
  NEXT,
  NEXT_T,
  NEXT_BIP,
  NEXT_BIP_T,
  AFFECTS,
  AFFECTS_T,
  AFFECTS_BIP,
  AFFECTS_BIP_T,
};

enum class ParamPosition { LEFT, RIGHT, BOTH };

enum class TableType { VAR_TABLE, CONST_TABLE, PROC_TABLE };

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

struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

// Define Set of Stmt Lists
typedef std::unordered_set<std::vector<int>, VectorHash> SetOfStmtLists;
