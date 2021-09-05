#include "Common/Global.h"

using namespace std;

bool DEBUG_MODE = true;  // TODO(submission): change this

void PrintErrorMsgAndExit(const string& msg) {
  if (!DEBUG_MODE) {
    return;
  }
  cout << msg << endl;
  exit(1);
}

void PrintDebugMessage(const string& msg) {
  if (!DEBUG_MODE) {
    return;
  }

  cout << msg << endl;
}
