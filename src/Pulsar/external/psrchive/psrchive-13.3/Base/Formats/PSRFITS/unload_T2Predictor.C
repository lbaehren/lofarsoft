/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Predictor.h"
#include "unload_text.h"

using namespace std;

void unload_T2Predictor (fitsfile* fptr,
			 const Tempo2::Predictor* model, bool verbose)
{
  if (verbose)
    cerr << "unload_T2Predictor entered" << endl;

  unload_text (fptr, "T2PREDICT", "PREDICT", model, verbose);
}
