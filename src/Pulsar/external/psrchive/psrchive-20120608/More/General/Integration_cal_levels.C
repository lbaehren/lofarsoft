/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/CalInfoExtension.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Estimate.h"
#include "Error.h"

/*! Returns the mean (and variance of the mean) off the on
  and off pulse cal levels. */
void 
Pulsar::Integration::cal_levels (vector<vector<Estimate<double> > >& high,
				 vector<vector<Estimate<double> > >& low) const
{
  unsigned nbin = get_nbin ();
  if (nbin==0)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels", "nbin = 0");

  unsigned npol = get_npol ();
  if (npol==0)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels", "npol = 0");

  unsigned nchan = get_nchan ();
  if (nchan==0)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels", "nchan = 0");

  int hightolow, lowtohigh, buffer;
  find_transitions (hightolow, lowtohigh, buffer);

  high.resize (npol);
  low.resize (npol);

  // Get CalInfo extension to see what cal type is
  unsigned cal_nstate = 2; // Default to normal 2-state cal

  Reference::To<const CalInfoExtension> ext;
  if (parent)
    ext = parent->get<CalInfoExtension>();

  if (ext) 
    cal_nstate = ext->cal_nstate;

  if (cal_nstate<2 || cal_nstate>3) 
    throw Error (InvalidState, "Pulsar::Integration::cal_levels", 
        "unexpected nstate = %d", cal_nstate);
  
  // Standard bin ranges for 2-state cal pulse
  int high_start = lowtohigh + buffer;
  int high_end = hightolow - buffer;
  int low_start = hightolow + buffer;
  int low_end = lowtohigh - buffer;

  // For a "3-state" cal (eg Nancay), we want to ignore the 2nd half
  // of the high state.  This could be generalized/improved to do
  // something more intelligent at some point.
  if (cal_nstate==3)
  { 
    int high_mid = (hightolow > lowtohigh) 
      ? (lowtohigh + hightolow) / 2
      : (lowtohigh + nbin + hightolow) / 2;
    if (high_mid > nbin) 
      high_mid -= nbin;
    high_end = high_mid - buffer;
    if (verbose) 
      cerr << "Pulsar::Integration::cal_levels using 3-state cal" << endl;
  }
    
  for (unsigned ipol=0; ipol<npol; ipol++) {

    high[ipol].resize(nchan);
    low[ipol].resize(nchan);

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      high[ipol][ichan] = low[ipol][ichan] = 0.0;

      if (get_weight(ichan) == 0)
	continue;

      profiles[ipol][ichan]->stats (&(high[ipol][ichan].val), 0,
				    &(high[ipol][ichan].var),
                                    high_start, high_end);

      profiles[ipol][ichan]->stats (&(low[ipol][ichan].val), 0,
				    &(low[ipol][ichan].var),
                                    low_start, low_end);

      // for linear X and Y: if on cal is lower than off cal, flag bad data
      if (npol <= 2
	  && (high[ipol][ichan].val < 0 || low[ipol][ichan].val < 0
	      || high[ipol][ichan].val < low[ipol][ichan].val)) {

	if (verbose) cerr << "Pulsar::Integration::cal_levels"
		       " - bad levels for channel " << ichan 
			  << " poln " << ipol 
			  << " mean high " << high[ipol][ichan].val
			  << " mean low " << low[ipol][ichan].val << endl;
	high[ipol][ichan] = low[ipol][ichan] = 0.0;
      }
    }
  }
}
