#include "TemporaryFile.h"
#include "dirutil.h"

#include <iostream>
#include <signal.h>

using namespace std;

int main ()
{
  TemporaryFile::abort = false;

  TemporaryFile test ("test_temporary");

  string filename = test.get_filename();

  if (!file_exists (filename.c_str())) {
    cerr << "TemporaryFile test failed: did not create file" << endl;
    return -1;
  }

  raise (SIGINT);

  if (file_exists (filename.c_str())) {
    cerr << "TemporaryFile test failed: did not remove file" << endl;
    return -1;
  }

  cerr << "TemporaryFile test passed" << endl;
  return 0;
}
