/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileWeightStatic.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::ProfileWeightStatic* Pulsar::ProfileWeightStatic::clone () const
{
  return new ProfileWeightStatic (*this);
}

//! Set the profile phase bins
void Pulsar::ProfileWeightStatic::set_bins (const std::string& _bins)
{
  bins = _bins;
  turns.clear();
}

std::string Pulsar::ProfileWeightStatic::get_bins () const
{
  return bins;
}

//! Set the profile phase ranges in turns
void Pulsar::ProfileWeightStatic::set_turns (const std::string& _turns)
{
  turns = _turns;
  bins.clear();
}

std::string Pulsar::ProfileWeightStatic::get_turns () const
{
  return turns;
}

void Pulsar::ProfileWeightStatic::calculate (PhaseWeight* weight)
{
  vector<unsigned> phase_bins;

  unsigned nbin = profile->get_nbin();

  if (!bins.empty())
    TextInterface::parse_indeces (phase_bins, bins, nbin);

  weight->resize (nbin);
  weight->set_all (0.0);

  for (unsigned i=0; i<phase_bins.size(); i++)
  {
    if (phase_bins[i] >= nbin)
      throw Error (InvalidParam, "Pulsar::ProfileWeightStatic::calculate",
		   "phase_bin[%u]=%u >= nbin=%u", i, phase_bins[i], nbin);

    (*weight)[ phase_bins[i] ] = 1.0;
  }
}

class Pulsar::ProfileWeightStatic::Interface 
  : public TextInterface::To<ProfileWeightStatic>
{
public:
  Interface (ProfileWeightStatic* instance)
  {
    if (instance)
      set_instance (instance);

    add( &ProfileWeightStatic::get_bins,
	 &ProfileWeightStatic::set_bins,
	 "bins", "phase bins in window" );
  }

  std::string get_interface_name () const { return "set"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::ProfileWeightStatic::get_interface ()
{
  return new Interface (this);
}

