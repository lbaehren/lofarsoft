/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Predictor.h"
#include "load_text.h"

using namespace std;

Pulsar::Predictor* load_T2Predictor (fitsfile* fptr, bool verbose) try
{
  if (verbose)
    cerr << "load_T2Predictor entered" << endl;

  Reference::To<Tempo2::Predictor> predictor = new Tempo2::Predictor;

  load_text (fptr, "T2PREDICT", "PREDICT", predictor.get(), verbose);

  return predictor.release();
}
catch (Error& error)
{
  if (verbose)
    cerr << "load_T2Predictor error: " << error.get_message() << endl;
  return 0;
}

