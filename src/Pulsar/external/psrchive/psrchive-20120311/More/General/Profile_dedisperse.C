/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Profile.h"
#include "Physical.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::dedisperse
//
/*!
  A convenience interface to Profile::rotate_phase.
  Rotates the profile in order
  to remove the dispersion delay with respect to a reference frequency.
  \param dm the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
  \param ref_freq the reference frequency (in MHz)
  \param pfold the folding periond (in seconds)
*/
void Pulsar::Profile::dedisperse (double dm, double ref_freq, double pfold)
{
  if (verbose)
    cerr << "Pulsar::Profile::dedisperse dm=" << dm << " pfold=" << pfold
         << " ref_freq=" << ref_freq << endl;

  double delay = dispersion_delay (dm, ref_freq, centrefreq);

  if (verbose)
    cerr << "Pulsar::Profile::dedisperse delay="
         << delay*1e3 << " ms" << endl;

  rotate_phase (delay / pfold);

}

