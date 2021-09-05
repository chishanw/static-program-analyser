#include "FollowKB.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Common/global.h"

using namespace std;

FollowKB::FollowKB() {
    table = unordered_map<STMT_NO, STMT_NO>({{}});
    invTable = unordered_map<STMT_NO, STMT_NO>({{}});
    tableT = unordered_map<STMT_NO, UNO_SET_OF_STMT_NO>({{}});
    invTableT = unordered_map<STMT_NO, UNO_SET_OF_STMT_NO>({{}});
}

void FollowKB::setFollows(STMT_NO s1, STMT_NO s2) {
    if (table.count(s1) == 0) {
        // True if successful
        table.insert({{s1, s2}});
        invTable.insert({{s2, s1}});
        return;
    }

    // check if value for key s1 is s2
    if (getFollows(s1) != s2) {
        ostringstream s;
        s << "[debug][setFollows] value for key "
        << "{" << s1 << "}"
        << " already exists"
            " and it is not "
        << "{" << s2 << "}";
        PrintErrorMsgAndExit(s.str());
    }
}

void FollowKB::addFollowsT(STMT_NO s1, STMT_NO s2) {
    if (tableT.count(s1) == 0) {
        // If no key, insert Set
        UNO_SET_OF_STMT_NO newSet({s2});
        tableT.insert({{s1, newSet}});
    }
    if (invTableT.count(s2) == 0) {
        // If no key, insert Set
        UNO_SET_OF_STMT_NO newInvSet({s1});
        invTableT.insert({{s2, newInvSet}});
    }

    if (tableT.at(s1).count(s2) == 0) {
        // Add key to exisiting Set
        tableT.at(s1).insert(s2);
    }
    if (invTableT.at(s2).count(s1) == 0) {
        // Add key to exisiting Set
        invTableT.at(s2).insert(s1);
    }
}

bool FollowKB::isFollowsT(STMT_NO s1, STMT_NO s2) {
    if (tableT.count(s1) == 0) {
        // Key does not exist
        return false;
    } else {
        unordered_set<STMT_NO> memberSet = tableT.at(s1);
        int result = memberSet.count(s2);
        return result > 0;
    }
}

STMT_NO FollowKB::getFollows(STMT_NO s1) {
    // Check if s1 is a valid key
    if (table.count(s1) == 0) {
        return -1;
    } else {
        return table.at(s1);
    }
}

STMT_NO FollowKB::getFollowedBy(STMT_NO s2) {
    // Check if s2 is a valid key
    if (invTable.count(s2) == 0) {
        return -1;
    } else {
        return invTable.at(s2);
    }
}

LIST_STMT_NO FollowKB::getFollowsT(STMT_NO s1) {
    if (tableT.count(s1) == 0) {
        // Return empty list
        return LIST_STMT_NO({});
    } else {
        unordered_set<STMT_NO> set = tableT.at(s1);
        LIST_STMT_NO followVector(set.begin(), set.end());

        return followVector;
    }
}

LIST_STMT_NO FollowKB::getFollowedTBy(STMT_NO s2) {
    if (invTableT.count(s2) == 0) {
        return LIST_STMT_NO({});
    } else {
        // Return empty list
        unordered_set<STMT_NO> set = invTableT.at(s2);
        LIST_STMT_NO followVector(set.begin(), set.end());

        return followVector;
    }
}
