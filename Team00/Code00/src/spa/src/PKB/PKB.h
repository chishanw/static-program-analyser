#pragma once

#include <stdio.h>

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "FollowKB.h"

using namespace std;

class PKB : public FollowKB {
 public:
    // Constructor
    PKB();

    // Methods
    bool addStatement(STMT_NO s);
    LIST_STMT_NO getAllStatements();  // ordered by insertion order

 private:
    // Members
    SET_OF_STMT_NO allStmtNo;
};
