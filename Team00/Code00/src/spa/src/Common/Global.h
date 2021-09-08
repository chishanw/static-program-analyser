#include <iostream>
#include <string>

extern bool DEBUG_MODE;

// =======================================
//  debug-mode-only methods
// =======================================

void DMOprintErrMsgAndExit(const std::string&);
void DMOprintInfoMsg(const std::string&);

// =======================================
//  normal methods
// =======================================

void PrintErrMsgAndExit(const std::string&);
