/***************************************************************************
 *
 *   Copyright (C) 2005 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::zap_periodic_spikes
/*! Interpolate over peaks of n-bin wide modulation feature.
 *
 * period, phase and width are measured in bins 
 * Interpolation windows run from bin "phase" to bin "phase+width-1", etc
*/
void zap_periodic_spikes(Pulsar::Profile* profile, int period, int phase,
    int width=1)
{
  int i, j, iprev, inext;
  int nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  for (i=phase; i < nbin; i+=period)
  {
    iprev = i - 1;
    if (iprev<0) iprev += nbin;
    inext = i + width;
    if (inext>=nbin) inext -= nbin;
    
    for (j=i; j<i+width; j++) {
      int idx = j % nbin;
      if (idx<0) idx += nbin;
      amps[idx] = 0.5*(amps[iprev]+amps[inext]);
    }
  }
}
