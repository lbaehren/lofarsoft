/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/DisperseWeight.h"

using namespace std;

/*! 
  \pre The profile baselines must have been removed (unchecked).
*/
void Pulsar::Integration::invint ()
{
  Reference::To<PolnProfile> profile;

  DisperseWeight shift (this);
  shift.set_weight( this->baseline() );

  // the output of the PhaseWeight shifter
  PhaseWeight shifted_baseline;

  for (unsigned ichan=0; ichan<get_nchan(); ++ichan) try
  {
    profile = new_PolnProfile (ichan);

    shift.get_weight (ichan, &shifted_baseline);

    profile -> set_baseline ( &shifted_baseline );
    profile -> invint ( profile->get_Profile(0) );
  }
  catch (Error& error)
  {
    error << " chan=" << ichan;
    throw error += "Pulsar::Integration::invint";
  }

  resize (1);
}

