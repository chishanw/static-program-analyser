#pragma once

#include <stdio.h>

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "FollowKB.h"
#include "ParentKB.h"

using namespace std;

class PKB : public FollowKB, public ParentKB {
 public:
  // Constructor
  PKB();

  // Methods
  bool addStatement(STMT_NO s);
  UNO_SET_OF_STMT_NO getAllStatements();

 private:
  // Members
  UNO_SET_OF_STMT_NO allStmtNo;
};
