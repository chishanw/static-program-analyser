#include "PKB.h"

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

PKB::PKB() { allStmtNo = UNO_SET_OF_STMT_NO(); }

bool PKB::addStatement(STMT_NO s) {
  if (allStmtNo.count(s) > 0) {
    return 0;
  } else {
    allStmtNo.insert(s);
    return 1;
  }
}

UNO_SET_OF_STMT_NO PKB::getAllStatements() { return allStmtNo; }
