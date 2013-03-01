/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pulsar.h"
#include "Pulsar/Predictor.h"

// load a tempo polyco, defined in load_polyco.C
Pulsar::Predictor* load_polyco (fitsfile*, double* phase, bool verbose);

// load a tempo2 predictor, defined in load_T2Predictor.C
Pulsar::Predictor* load_T2Predictor (fitsfile*, bool verbose);

void Pulsar::FITSArchive::load_Predictor (fitsfile* fptr)
{
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Predictor try POLYCO" << endl;

  model = load_polyco (fptr, &predicted_phase, verbose > 2);

  if (model)
    return;

#ifdef HAVE_TEMPO2

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Predictor try T2PREDICT" << endl;

  model = load_T2Predictor (fptr, verbose > 2);

  if (model)
    return;

#else

  // See if the T2PREDICT HDU
  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "T2PREDICT", 0, &status);
  
  if (status == 0)
    warning <<
      "Pulsar::FITSArchive does not support tempo2 predictors \n"
      "Pulsar::FITSArchive http://psrchive.sourceforge.net/warnings/tempo2"
	    << endl;

#endif

}

