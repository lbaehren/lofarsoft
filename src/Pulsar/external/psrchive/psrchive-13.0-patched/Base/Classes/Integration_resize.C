/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;

/*!
  If any current dimension is greater than that requested, the Profiles
  will be deleted and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  If any of the supplied paramaters is equal to zero,
  the dimension is left unchanged.
  */
void Pulsar::Integration::resize (unsigned new_npol,
				  unsigned new_nchan, 
				  unsigned new_nbin)
{
  unsigned cur_npol = get_npol();
  unsigned cur_nchan = get_nchan();
  unsigned cur_nbin = get_nbin();

  if (new_npol == 0)
    new_npol = cur_npol;
  if (new_nchan == 0)
    new_nchan = cur_nchan;
  if (new_nbin == 0)
    new_nbin = cur_nbin;

  if (verbose)
    cerr << "Integration::resize npol=" << new_npol
	 << " nchan=" << new_nchan << " nbin=" << new_nbin << endl 
	 << " old npol=" << cur_npol
	 << " nchan=" << cur_nchan << " nbin=" << cur_nbin << endl;

  unsigned ipol=0, ichan=0;

  profiles.resize (new_npol);

  for (ipol=0; ipol < cur_npol; ipol++) {
    profiles[ipol].resize (new_nchan);
    for (ichan=cur_nchan; ichan < new_nchan; ichan++)
      profiles[ipol][ichan] = new_Profile();
  }

  for (ipol=cur_npol; ipol < new_npol; ipol++) {
    profiles[ipol].resize (new_nchan);
    for (ichan=0; ichan < new_nchan; ichan++)
      profiles[ipol][ichan] = new_Profile();
  }

  for (ipol=0; ipol < new_npol; ipol++)
    for (ichan=0; ichan < new_nchan; ichan++)
      profiles[ipol][ichan] -> resize(new_nbin);

  set_npol (new_npol);
  set_nchan (new_nchan);
  set_nbin (new_nbin);

}
