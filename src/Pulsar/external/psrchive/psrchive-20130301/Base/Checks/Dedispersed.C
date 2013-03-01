/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Dedispersed.h"
#include "Pulsar/Dedisperse.h"

using namespace std;

// defined in DeFaradayed.C
bool diff (double x, double y);

void Pulsar::Dedispersed::apply (const Archive* archive)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::Dedispersed::apply verification" << endl;

  unsigned nsubint = archive->get_nsubint();

  for (unsigned isubint=0; isubint < nsubint; isubint++) {

    const Integration* subint = archive->get_Integration (isubint);

    const Dedisperse* ext = subint->get<Dedisperse>();

    if (!archive->get_dedispersed()) {

      if (ext)
	throw Error (InvalidState, "Pulsar::Dedispersed::apply",
		     "Archive::dedispsersed is not set and Integration[%d]\n\t"
		     "has a Dedisperse Extension", isubint);

    }
    else {

      if (!ext)
	throw Error (InvalidState, "Pulsar::Dedispersed::apply",
		     "Archive::dedispsersed is set and Integration[%d]\n\t"
		     "has no Dedisperse Extension", isubint);
    
      if (diff( ext->get_reference_frequency(),
		archive->get_centre_frequency() ))
	throw Error (InvalidState, "Pulsar::Dedispersed::apply",
		     "Archive::dedispsersed is set and Integration[%d]\n\t"
		     "Dedisperse::reference_frequency = %lf doesn't equal\n\t"
		     "Archive::centre_frequency = %lf", isubint,
		     ext->get_reference_frequency(),
		     archive->get_centre_frequency());
  
      if (diff( ext->get_dispersion_measure(),
		archive->get_dispersion_measure() ))
	throw Error (InvalidState, "Pulsar::Dedispersed::apply",
		     "Archive::dedispsersed is set and Integration[%d]\n\t"
		     "Dedisperse::dispersion_measure = %lf does not equal\n\t"
		     "Archive::dispersion_measure = %lf", isubint,
		     ext->get_dispersion_measure(),
		     archive->get_dispersion_measure());
    }

  }

}
