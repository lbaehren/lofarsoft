/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardSNRWeight.h"
#include "Pulsar/ProfileShiftFit.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;

//! Set the weights of all Profiles in the Archive
void Pulsar::StandardSNRWeight::operator () (Archive* archive)
{
  use_chan = archive->get_nchan() == chan.size();
  Weight::operator() (archive);
}

//! Default constructor
Pulsar::StandardSNRWeight::StandardSNRWeight (const std::string& filename)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::StandardSNRWeight filename=" << filename << endl;

  Reference::To<Archive> archive = Archive::load(filename);
  archive->pscrunch();
  archive->tscrunch();

  if (archive->get_nchan() > 1)
  {
    chan.resize (archive->get_nchan());
    for (unsigned ichan=0; ichan < chan.size(); ichan++)
    {
      chan[ichan] = new ProfileShiftFit;
      chan[ichan] -> choose_maximum_harmonic = true;
      chan[ichan] -> set_standard ( archive->get_Profile (0,0,ichan) );
    }
  }

  archive->fscrunch();
  total = new ProfileShiftFit;
  total -> choose_maximum_harmonic = true;
  total -> set_standard ( archive->get_Profile (0,0,0) );
}

//! Get the weight of the specified channel
double Pulsar::StandardSNRWeight::get_weight (const Integration* integration,
					      unsigned ichan)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::StandardSNRWeight::get_weight ichan=" << ichan << endl;

  ProfileShiftFit* use = total;
  if (use_chan)
    use = chan[ichan];

  use->set_Profile( integration->get_Profile(0,ichan) );
  double snr = use->get_snr();

  if (Archive::verbose > 2)
    cerr << "Pulsar::StandardSNRWeight::get_weight snr=" << snr << endl;

  return snr*snr;
}
