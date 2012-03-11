/***************************************************************************
 *
 *   Copyright (C) 2010 by Patrick Weltevrede
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RemoveVariableBaseline.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

void Pulsar::RemoveVariableBaseline::transform (Archive* archive)
{
  const unsigned nsub = archive->get_nsubint();
  const unsigned nchan = archive->get_nchan();
  const unsigned npol = archive->get_npol();

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Integration* subint = archive->get_Integration (isub);

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      for (unsigned ipol=0; ipol < npol; ipol++)
      {
	Reference::To<Profile> profile = subint->get_Profile (ipol,ichan);

	const unsigned nbin = profile->get_nbin();
	float* data = profile->get_amps();

	/* here you can modify each array of nbin floats */

      }
    }
  }
};
