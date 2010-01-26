#include "ThreadStream.h"

using namespace std;

int main ()
{
  cerr << "ThreadStream::ctor" << endl;

  ThreadStream ts;

  cerr << "ThreadStream::copy_streambuf" << endl;

  ts.set (&cout);

  cerr << "ThreadStream::operator <<" << endl;

  ts << "test";
  ts << endl;

  return 0;
}
