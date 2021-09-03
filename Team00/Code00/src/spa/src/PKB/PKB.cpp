#include "PKB.h"

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

PKB::PKB() { allStmtNo = SET_OF_STMT_NO(); }

bool PKB::addStatement(STMT_NO s) {
    if (allStmtNo.count(s) > 0) {
        return 0;
    } else {
        allStmtNo.insert(s);
        return 1;
    }
}

LIST_STMT_NO PKB::getAllStatements() {
    return LIST_STMT_NO(allStmtNo.begin(), allStmtNo.end());
}
