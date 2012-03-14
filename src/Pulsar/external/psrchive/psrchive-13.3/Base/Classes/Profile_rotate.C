/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "FTransform.h"
#include "templates.h"

#include <memory>
#include <math.h>
#include <string.h>

using namespace std;

Pulsar::Option<bool>
Pulsar::Profile::rotate_in_phase_domain
("Profile::rotate_in_phase_domain", false);

/*!  
  Rotate the profile by the specified phase.  The profile will be
  rotated such that the power at phase will be found at phase zero. ie.

  \f$t^\prime=t+\phi P\f$

  where \f$t^\prime\f$ is the new start time (rising edge of bin 0),
  \f$t\f$ is the original start time, \f$\phi\f$ is equal to phase,
  and \f$P\f$ is the period at the time of folding.
*/
void Pulsar::Profile::rotate_phase (double phase)
{
  if (!finite(phase))
    throw Error (InvalidParam, "Pulsar::Profile::rotate_phase",
		 "non-finite phase = %lf\n", phase);

#ifdef _DEBUG
    cerr << "Pulsar::Profile::rotate phase=" << phase 
	 << " nbin=" << get_nbin() << endl;
#endif

  if (phase == 0.0)
    return;

  // Ensure that phase runs from 0 to 1.  This step may help to
  // preserve precision if both phase and nbin are large.
  phase -= floor (phase);

  unsigned nbin = get_nbin();
  float* amps = get_amps();

  if (!rotate_in_phase_domain)
  {
    FTransform::shift (nbin, amps, phase*double(nbin));
  }
  else
  {
    // after using floor as above, phase is always greater than zero
    unsigned binshift = unsigned (phase * double(nbin) + 0.5);

#ifdef _DEBUG
    if (verbose)
      cerr << "Pulsar::Profile::rotate " << binshift << " bins" << endl;
#endif

    ::shift (nbin, binshift, amps);
  }

}
