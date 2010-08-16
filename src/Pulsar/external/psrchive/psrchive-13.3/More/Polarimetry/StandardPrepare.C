/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardPrepare.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

using namespace std;

Calibration::StandardPrepare*
Calibration::StandardPrepare::factory (std::string name)
{
  if (name.find (",") != string::npos)
  {
    MultipleRanking* mult = new MultipleRanking;
    for (string sub=stringtok(name,","); !sub.empty(); sub=stringtok(name,","))
      mult->add( factory(sub) );
    return mult;
  }

  if (name == "int")
    return new MaximumIntensity;

  if (name == "pol")
    return new MaximumPolarization;

  if (name == "orth")
    return new MaximumOrthogonality;

  if (name == "inv")
    return new MaximumInvariant;

  throw Error (InvalidParam, "Calibration::StandardPrepare::factory",
	       "no bin selection policy named %s", name.c_str());
}

//! Prepare the data for use by the StandardData class
void Calibration::StandardPrepare::prepare (Pulsar::Archive* archive)
{
  archive->convert_state (Signal::Stokes);
  archive->remove_baseline ();
  archive->dedisperse ();
  archive->centre (0.0);
}

//! Set the number of input states to be chosen
void Calibration::StandardPrepare::set_input_states (unsigned nbin)
{
  bins.resize (nbin);
}

//! Return the selected bins
void Calibration::StandardPrepare::get_bins (std::vector<unsigned>& b) const
{
  b = bins;
}

//! Choose the input states
void Calibration::StandardPrepare::choose (const Pulsar::Archive* input) try
{
  if (bins.size() == 0)
    return;

  Reference::To<Pulsar::Archive> autobin = input->clone();
  autobin->fscrunch ();
  autobin->tscrunch ();
  prepare (autobin);

  profile = autobin->get_Integration(0)->new_PolnProfile(0);

  extra (input);

  unsigned nbin = profile->get_nbin();
  vector<bool> chosen (nbin, false);

  for (unsigned ichoose=0; ichoose < bins.size(); ichoose++)
  {
    double maximum = 0.0;
    unsigned imax = 0;

    for (unsigned ibin=0; ibin<nbin; ibin++)
    {
      // ranking may be a dynamic procedure, therefore rank is not stored
      double ranking = get_rank (ibin);

      if (ranking > maximum && !chosen[ibin])
      {
	maximum = ranking;
	imax = ibin;
      }
    }

    set_chosen (imax);

    bins[ichoose] = imax;
    chosen[imax] = true;
  }
}
catch (Error& error)
{
  throw error += "Calibration::StandardPrepare::choose";
}

void Calibration::StandardPrepare::set_chosen (unsigned)
{
}

void Calibration::StandardPrepare::extra (const Pulsar::Archive*)
{
}

double Calibration::MaximumPolarization::get_rank (unsigned ibin)
{
  Stokes<float> stokes = profile->get_Stokes (ibin);
  return stokes.abs_vect();
}

double Calibration::MaximumIntensity::get_rank (unsigned ibin)
{
  return profile->get_Profile(0)->get_amps()[ibin];
}

double Calibration::MaximumOrthogonality::get_rank (unsigned ibin)
{
  Stokes<float> stokes = profile->get_Stokes (ibin);

  if (last_chosen[0] == 0.0)
    return stokes.abs_vect();

  Vector<3,float> x = cross( stokes.get_vector (), last_chosen.get_vector() );
  return x * x;
}

void Calibration::MaximumOrthogonality::set_chosen (unsigned ibin)
{
  last_chosen = profile->get_Stokes (ibin);
}
 
double Calibration::MaximumInvariant::get_rank (unsigned ibin)
{
  return invariant->get_amps()[ibin];
}

void Calibration::MaximumInvariant::extra (const Pulsar::Archive* input) try
{
  Reference::To<Pulsar::Archive> autobin = input->clone();

  autobin->dedisperse ();
  autobin->centre (0.0);

  autobin->invint ();

  autobin->fscrunch ();
  autobin->tscrunch ();

  invariant = autobin->get_Integration(0)->get_Profile(0,0);
}
catch (Error& error)
{
  throw error += "Calibration::MaximumInvariant::extra";
}

Calibration::MultipleRanking::MultipleRanking ()
{
  current = 0;
}

//! Add a ranking scheme
void Calibration::MultipleRanking::add (StandardPrepare* rank)
{
  rankings.push_back (rank);
}

//! Return the total intensity
double Calibration::MultipleRanking::get_rank (unsigned ibin)
{
  return rankings[current]->get_rank (ibin);
}

//! Called when a phase bin has been chosen
void Calibration::MultipleRanking::set_chosen (unsigned ibin)
{
  current ++;
  current %= rankings.size();

  for (unsigned i=0; i<rankings.size(); i++)
    rankings[i]->set_chosen (ibin);
}

//! Choose the input states
void Calibration::MultipleRanking::extra (const Pulsar::Archive* input)
{
  for (unsigned i=0; i<rankings.size(); i++)
  {
    rankings[i]->profile = profile;
    rankings[i]->extra (input);
  }
}
