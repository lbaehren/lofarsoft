#include "Pulsar/Parameters.h"
#include "load_factory.h"

using namespace std;

int main (int argc, char** argv) try {

  Pulsar::Parameters::verbose = true;

  if (argc < 2)  {
    cerr << "Please specify a filename" << endl;
    return -1;
  }

  Pulsar::Parameters* parameters = factory<Pulsar::Parameters> (argv[1]);

  delete parameters;

  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
catch (...) {
  cerr << "Exception caught" << endl;
  return -1;
}

