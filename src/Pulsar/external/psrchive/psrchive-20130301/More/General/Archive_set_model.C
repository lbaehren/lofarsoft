/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

using namespace std;

void Pulsar::Archive::set_model (const Predictor* new_model, bool apply)
{
  if (!new_model)
  {
    model = 0;
    return;
  }

  if (!good_model (new_model))
    throw Error (InvalidParam, "Pulsar::Archive::set_model",
                 "supplied model does not span Integrations");

  // swap the old with the new
  Reference::To<Predictor> oldmodel = model;
  model = new_model->clone();

  if (!apply)
    return;

  if (verbose == 3)
    cerr << "Pulsar::Archive::set_model apply the new model" << endl;

  // correct Integrations
  for (unsigned isub = 0; isub < get_nsubint(); isub++)
    apply_model (get_Integration(isub), oldmodel.ptr());
}

