#include "RegularExpression.h"

#include <iostream>
using namespace std;

int main ()
{
  RegularExpression re ("wb.*_c");

  if (!re.get_match("wb12345sd567_c")) {
    cerr << "Failed simple match test positive" << endl;
    return -1;
  }

  if (re.get_match("wbc")) {
    cerr << "Failed simple match test negative" << endl;
    return -1;
  }

  cerr << "Simple tests passed" << endl;
  return 0;
}
