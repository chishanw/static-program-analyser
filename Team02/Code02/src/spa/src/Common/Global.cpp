#include "Common/Global.h"

using namespace std;

bool DEBUG_MODE = true;  // TODO(anyone): remove for iter2 submission

// =======================================
//  debug-mode-only methods
// =======================================

void DMOprintErrMsgAndExit(const string& msg) {
  if (!DEBUG_MODE) {
    return;
  }
  cout << msg << endl;
  exit(1);
}

void DMOprintInfoMsg(const string& msg) {
  if (!DEBUG_MODE) {
    return;
  }

  cout << "[INFO]: " << msg << endl;
}

// =======================================
//  normal methods
// =======================================

void PrintErrMsgAndExit(const string& msg) {
  cout << msg << endl;
  exit(0);
}
