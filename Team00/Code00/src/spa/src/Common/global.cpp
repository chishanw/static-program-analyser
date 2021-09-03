#include "Common/global.h"

using namespace std;

bool DEBUG_MODE = true;  // TODO(submission): change this

void Debug(const string& msg) {
  if (!DEBUG_MODE) {
    return;
  }
  cout << msg << endl;
  exit(1);
}
