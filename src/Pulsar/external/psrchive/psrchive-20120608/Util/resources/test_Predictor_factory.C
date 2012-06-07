#include "Pulsar/Predictor.h"
#include "load_factory.h"

using namespace std;

int main (int argc, char** argv) try {

  Pulsar::Predictor::verbose = true;

  if (argc < 2)  {
    cerr << "Please specify a filename" << endl;
    return -1;
  }

  Pulsar::Predictor* parameters = factory<Pulsar::Predictor> (argv[1]);

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

