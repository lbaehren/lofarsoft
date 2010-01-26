#include "BatchQueue.h"

#include <iostream>

using namespace std;

static unsigned method_calls = 0;

class A : public Reference::Able {
public:
  void method ()
  {
    method_calls++;
#ifdef _DEBUG
    cerr << "A::method\n"; 
#endif
  }
};

int main () try {

  BatchQueue queue;

#if HAVE_PTHREAD
  queue.resize (8);
#endif

  unsigned total = 123;

  A a;
  for (unsigned i=0; i<total; i++)
    queue.submit (&a, &A::method);
  
  queue.wait ();

  if (method_calls != total) {
    cerr << "Error: submitted=" << total << " finished=" << method_calls
	 << endl;
    return -1;
  }

  cerr << "BatchQueue test passed" << endl;
  return 0;

}
 catch (Error& error) {
   cerr << error << endl;
   return -1;
 }
