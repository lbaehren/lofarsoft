/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DeFaradayed.h"
#include "Pulsar/DeFaraday.h"

using namespace std;

bool diff (double x, double y)
{
  return fabs(x-y)/(x+y) > 1e-15;
}

void Pulsar::DeFaradayed::apply (const Archive* archive)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::DeFaradayed::apply verification" << endl;

  unsigned nsubint = archive->get_nsubint();

  for (unsigned isubint=0; isubint < nsubint; isubint++)
  {
    const Integration* subint = archive->get_Integration (isubint);
    const DeFaraday* ext = subint->get<DeFaraday>();

    if (!archive->get_faraday_corrected())
    {
      if (ext)
	throw Error (InvalidState, "Pulsar::DeFaradayed::apply",
		     "Archive::faraday_corrected is not set and Integration[%d]\n\t"
		     "has a DeFaraday Extension", isubint);
    }
    else
    {
      if (!ext)
	throw Error (InvalidState, "Pulsar::DeFaradayed::apply",
		     "Archive::faraday_corrected is set and Integration[%d]\n\t"
		     "has no DeFaraday Extension", isubint);
    
      if (diff( ext->get_reference_frequency(),
		archive->get_centre_frequency() ))
	throw Error (InvalidState, "Pulsar::DeFaradayed::apply",
		     "Archive::faraday_corrected is set and Integration[%d]\n\t"
		     "DeFaraday::reference_frequency = %lf does not equal\n\t"
		     "Archive::centre_frequency = %lf  (diff = %lf)", isubint,
		     ext->get_reference_frequency(),
		     archive->get_centre_frequency());
  
      if (diff( ext->get_rotation_measure(),
		archive->get_rotation_measure() ))
	throw Error (InvalidState, "Pulsar::DeFaradayed::apply",
		     "Archive::faraday_corrected is set and Integration[%d]\n\t"
		     "DeFaraday::rotation_measure = %lf does not equal\n\t"
		     "Archive::rotation_measure = %lf", isubint,
		     ext->get_rotation_measure(),
		     archive->get_rotation_measure());
    }
  }
}

