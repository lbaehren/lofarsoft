/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

// Integration Extension classes used to store state information
#include "Pulsar/DeFaraday.h"
#include "Pulsar/Dedisperse.h"

#include <iostream>
using namespace std;

Pulsar::Integration* Pulsar::Archive::use_Integration (Integration* subint)
{
  init_Integration (subint);
  return subint;
}

/*!  
  After an Integration has been loaded from disk, this method
  ensures that various internal book-keeping attributes are
  initialized.
*/
void Pulsar::Archive::init_Integration (Integration* subint, bool check_phase)
{
  if (verbose > 2)
    cerr << "Pulsar::Archive::init_Integration check_phase=" << check_phase << endl;

  subint->parent = this;

  if ( get_dedispersed() )
  {
    Dedisperse* corrected = new Dedisperse;
    corrected->set_reference_frequency( get_centre_frequency() );
    corrected->set_dispersion_measure( get_dispersion_measure() );
    subint->add_extension( corrected );
  }

  if ( get_faraday_corrected() )
  {
    DeFaraday* corrected = new DeFaraday;
    corrected->set_reference_frequency( get_centre_frequency() );
    corrected->set_rotation_measure( get_rotation_measure() );
    subint->add_extension( corrected );
  }

  subint->zero_phase_aligned = false;

  if (check_phase && model)
  {
    MJD epoch = subint->get_epoch();
    Phase phase = model->phase( epoch );

    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration epoch="
           << epoch.printdays(20) << " fturn=" << phase.fracturns() << endl;

    double frac = phase.fracturns();
    frac = fabs( frac - round(frac) );

    subint->zero_phase_aligned = frac < 1e-8;

    if (verbose > 2)
      cerr << "Pulsar::Archive::init_Integration frac=" << frac
           << " aligned=" << subint->zero_phase_aligned << endl;
  }

}
