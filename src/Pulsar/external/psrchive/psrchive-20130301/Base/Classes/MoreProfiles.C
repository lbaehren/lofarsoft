/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MoreProfiles.h"
#include "templates.h"

//! Construct with a name
Pulsar::MoreProfiles::MoreProfiles (const char* name)
  : DataExtension (name)
{
}

//! Copy constructor
Pulsar::MoreProfiles::MoreProfiles (const MoreProfiles& other)
  : DataExtension (other.get_extension_name().c_str())
{
  profile.resize (other.profile.size());
  for (unsigned i=0; i<profile.size(); i++)
    profile[i] = other.profile[i]->clone();
}

//! resize the profile vector
void Pulsar::MoreProfiles::resize (unsigned nprofile, unsigned nbin) try
{
  profile.resize (nprofile);
  for (unsigned i=0; i<profile.size(); i++)
  {
    if (!profile[i])
      profile[i] = new Profile (nbin);

    profile[i]->resize (nbin);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::MoreProfiles::resize";
}

//! get the size of the profile vector
unsigned Pulsar::MoreProfiles::get_size () const
{
  return profile.size();
}

static void check (unsigned i, unsigned n)
{
  if (i >= n)
    throw Error (InvalidParam, "Pulsar::MoreProfiles::get_Profile",
		 "i=%u >= size=%u", i, n);
}

//! get the ith profile
Pulsar::Profile* Pulsar::MoreProfiles::get_Profile (unsigned i)
{
  check (i, profile.size());
  return profile[i];
}

//! get the ith profile
const Pulsar::Profile* Pulsar::MoreProfiles::get_Profile (unsigned i) const
{
  check (i, profile.size());
  return profile[i];
}

unsigned Pulsar::MoreProfiles::get_nbin () const try
{
  if (profile.size() == 0)
    return 0;

  return profile[0]->get_nbin();
}
catch (Error& error)
{
  throw error += "Pulsar::MoreProfiles::get_nbin";
}

void Pulsar::MoreProfiles::resize (unsigned nbin)
{
  foreach (profile, &Profile::resize, nbin);
}

void Pulsar::MoreProfiles::set_weight (float weight)
{
  foreach (profile, &Profile::set_weight, weight);
}

void Pulsar::MoreProfiles::scale (double scale)
{
  foreach (profile, &Profile::scale, scale);
}

void Pulsar::MoreProfiles::offset (double offset)
{
  foreach (profile, &Profile::offset, offset);
}

void Pulsar::MoreProfiles::rotate_phase (double phase)
{
  foreach (profile, &Profile::rotate_phase, phase);
}

void Pulsar::MoreProfiles::zero ()
{
  foreach (profile, &Profile::zero);
}

void Pulsar::MoreProfiles::bscrunch (unsigned nscrunch)
{
  foreach (profile, &Profile::bscrunch, nscrunch);
}

void Pulsar::MoreProfiles::bscrunch_to_nbin (unsigned nbin)
{
  foreach (profile, &Profile::bscrunch_to_nbin, nbin);
}

void Pulsar::MoreProfiles::fold (unsigned nfold)
{
  foreach (profile, &Profile::fold, nfold);
}

//! integrate information from another Profile
void Pulsar::MoreProfiles::integrate (const Profile* p)
{
  Functor< void(const MoreProfiles*) > f (this, &MoreProfiles::average);
  foreach<MoreProfiles> (p, f);
}

//! integrate data from another MoreProfiles
void Pulsar::MoreProfiles::average (const MoreProfiles* that)
{
  if (this->profile.size() != that->profile.size())
    throw Error (InvalidParam, "Pulsar::MoreProfiles::average",
		 "this.size=%u != that.size=%u",
		 this->profile.size(), that->profile.size());

  const unsigned nprof = profile.size();
  for (unsigned iprof=0; iprof < nprof; iprof++)
    profile[iprof] -> average( that->profile[iprof] );
}
