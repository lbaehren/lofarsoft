/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RemoveBaseline.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

void Pulsar::RemoveBaseline::Total::transform (Archive* archive)
{
  Reference::To<PhaseWeight> baseline = archive->baseline();

  const unsigned nsub = archive->get_nsubint();
  for (unsigned isub=0; isub < nsub; isub++)
    archive->get_Integration(isub) -> remove_baseline (baseline);
};

void Pulsar::RemoveBaseline::Each::transform (Archive* archive)
{
  const unsigned nsub = archive->get_nsubint();
  const unsigned nchan = archive->get_nchan();
  const unsigned npol = archive->get_npol();

  bool pscrunch = (archive->get_state() == Signal::Coherence ||
		   archive->get_state() == Signal::PPQQ);

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Integration* subint = archive->get_Integration (isub);
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      Reference::To<Profile> profile = subint->get_Profile (0,ichan);
      if (pscrunch)
      {
	profile = profile->clone();
	profile->sum (subint->get_Profile (1,ichan));
      }

      Reference::To<PhaseWeight> baseline = profile->baseline();

      for (unsigned ipol=0; ipol < npol; ipol++)
      {
	Profile* p = subint->get_Profile(ipol, ichan);
	baseline->set_Profile (p);
	p->offset (-baseline->get_mean().val);
      }
    }
  }
};
