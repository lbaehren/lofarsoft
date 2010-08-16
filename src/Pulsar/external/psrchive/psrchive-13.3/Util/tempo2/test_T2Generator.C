/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Generator.h"
#include "T2Predictor.h"
#include "T2Parameters.h"
#include "Error.h"
#include "load_factory.h"

using namespace std;

int main (int argc, char** argv) try {

  if (argc < 2) {
    cerr << "Please specify a parameter file name" << endl;
    return -1;
  }

  cerr << "Reading T2 parameters from " << argv[1] << endl;

  Tempo2::Parameters* parameters = load<Tempo2::Parameters>( argv[1] );

  cerr << "Loaded" << endl;

  // parameters->unload( stderr );

  Tempo2::Generator generator;

  cerr << "Set parameters" << endl;

  generator.set_parameters (parameters);

  MJD epoch1 ("54242.309294");
  MJD epoch2 ("54242.809294");

  cerr << "Set time span" << endl;

  generator.set_time_span (epoch1, epoch2);
  generator.set_frequency_span (1300, 1500);

  cerr << "Call generate" << endl;

  Pulsar::Predictor* predictor = generator.generate ();

  cerr << "generated predictor:" << endl;

  predictor->unload (stderr);

  return 0;
}
 catch (Error& e) {
   cerr << e << endl;
   return -1;
 }
