/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/FITSArchive.h"
#include "polyco.h"

#ifdef HAVE_TEMPO2
#include "T2Predictor.h"

// unload a tempo2 predictor, defined in unload_T2Predictor.C
void unload_T2Predictor (fitsfile*, const Tempo2::Predictor*, bool verbose);
#endif

using namespace std;

void unload_polyco (fitsfile*, const polyco*, double pred_phs, bool verbose);

void Pulsar::FITSArchive::unload_Predictor (fitsfile* fptr) const
{
  const polyco* t1model = dynamic_cast<const polyco*> (model.ptr());
  if (t1model)
    unload_polyco (fptr, t1model, predicted_phase, verbose > 2);
  else
    delete_hdu (fptr, "POLYCO");

#ifdef HAVE_TEMPO2

  const Tempo2::Predictor* t2model;
  t2model = dynamic_cast<const Tempo2::Predictor*> (model.ptr());
  if (t2model)
    unload_T2Predictor (fptr, t2model, verbose > 2);
  else
    delete_hdu (fptr, "T2PREDICT");

#else

  delete_hdu (fptr, "T2PREDICT");

#endif
}

