/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FourthMoments.h"

//! Construct with a name
Pulsar::FourthMoments::FourthMoments (const char* name)
  : MoreProfiles (name)
{
}

Pulsar::FourthMoments* Pulsar::FourthMoments::clone () const
{
  return new FourthMoments (*this);
}

//! multiplies each bin of the profile by scale
void Pulsar::FourthMoments::scale (double scale)
{
  // e.g. var(a*x) = a^2 * var(x)
  MoreProfiles::scale (scale * scale);
}

//! offsets each bin of the profile by offset
void Pulsar::FourthMoments::offset (double offset)
{
  // do nothing: var(a+x) = var(x)
}

//! integrate neighbouring phase bins in profile
void Pulsar::FourthMoments::bscrunch (unsigned nscrunch)
{
  MoreProfiles::bscrunch (nscrunch);
  MoreProfiles::scale (1.0/nscrunch);
}

//! integrate neighbouring phase bins in profile
void Pulsar::FourthMoments::bscrunch_to_nbin (unsigned nbin)
{
  unsigned current_nbin = 0;
  if (profile.size())
    current_nbin = profile[0]->get_nbin();

  MoreProfiles::bscrunch_to_nbin (nbin);
  MoreProfiles::scale ( double(nbin)/double(current_nbin) ); 
}

//! integrate neighbouring sections of the profile
void Pulsar::FourthMoments::fold (unsigned nfold)
{
  MoreProfiles::fold (nfold);
  MoreProfiles::scale (1.0/nfold);
}

void Pulsar::FourthMoments::pscrunch ()
{
  delete this;
}

// defined in Profile_average.C
void weighted_average (Pulsar::Profile* result,
		       const Pulsar::Profile* other,
		       bool second_moment);

//! average information from another MoreProfiles
void Pulsar::FourthMoments::average (const MoreProfiles* more)
{
  const FourthMoments* fourth = dynamic_cast<const FourthMoments*> (more);
  if (!fourth)
    return;

  const unsigned nprof = profile.size();
  for (unsigned iprof=0; iprof < nprof; iprof++)
    weighted_average (get_Profile(iprof), more->get_Profile(iprof), true);
}
