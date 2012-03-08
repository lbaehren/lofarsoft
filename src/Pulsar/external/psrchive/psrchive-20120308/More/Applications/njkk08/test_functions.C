#include "njkk08.h"
#include "Error.h"

#include <iostream>
using namespace std;

int main () try
{
  cerr << lookup_PA_err (0.5) << endl;

  vector<float> xint (2);
  xint[0] = 24;
  xint[1] = 57.5;

  cerr << lookup_RM_err (xint) << endl;

  return 0;
}
 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
