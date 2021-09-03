#pragma once

#include <stdio.h>

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

typedef int STMT_NO;
typedef std::unordered_set<STMT_NO> UNO_SET_OF_STMT_NO;
typedef std::set<STMT_NO> SET_OF_STMT_NO;
typedef std::vector<STMT_NO> LIST_STMT_NO;

class FollowKB {
 public:
    // Consturctor
    FollowKB();

    // Methods for DE
    void setFollows(STMT_NO s1, STMT_NO s2);
    void addFollowsT(STMT_NO s1, STMT_NO s2);

    // Methods for QE
    bool isFollowsT(STMT_NO s1, STMT_NO s2);
    STMT_NO getFollows(STMT_NO s1);
    STMT_NO getFollowedBy(STMT_NO s2);
    LIST_STMT_NO getFollowsT(STMT_NO s1);
    LIST_STMT_NO getFollowedTBy(STMT_NO s2);

 private:
    // Follow Tables
    std::unordered_map<STMT_NO, STMT_NO> table;
    std::unordered_map<STMT_NO, STMT_NO> invTable;
    std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> tableT;
    std::unordered_map<STMT_NO, UNO_SET_OF_STMT_NO> invTableT;
};
