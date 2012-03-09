/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

using namespace std;

/*!
  Uses the polyco model, as well as the centre frequency and mid-time of
  each Integration to determine the predicted pulse phase.
 */
void Pulsar::Archive::centre (double phase_offset)
{
  // this function doesn't work for things without polycos
  if (get_type () != Signal::Pulsar)
    return;

  if (!model)
    throw Error (InvalidState, "Pulsar::Archive::centre",
                 "Pulsar observation with no polyco");

  Phase half_turn (phase_offset);

  for (unsigned isub=0; isub < get_nsubint(); isub++)  {

    Integration* subint = get_Integration(isub);

    // Rotate according to polyco prediction
    Phase phase = model->phase (subint->get_epoch()) +
      model->dispersion (subint->get_epoch(), subint->get_centre_frequency());

    if (verbose == 3)
      cerr << "Pulsar::Archive::center phase=" << phase << endl;

    double fracturns = (half_turn - phase).fracturns();
    subint -> rotate ( fracturns * subint -> get_folding_period() );
  }
}

