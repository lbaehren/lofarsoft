/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Smooth.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::Smooth::Smooth ()
{
  last_turns = turns = Pulsar::Profile::default_duty_cycle;
  last_bins = bins = 0;
}

Pulsar::Smooth::~Smooth ()
{

}

void Pulsar::Smooth::set_turns (float _turns)
{
  if (_turns <= 0 || _turns >= 1.0)
    throw Error (InvalidParam, "Pulsar::Smooth::set_turns",
		 "invalid turns = %f", _turns);

  last_turns = turns = _turns;
  bins = 0;
}

float Pulsar::Smooth::get_turns () const
{
  return turns;
}

float Pulsar::Smooth::get_last_turns () const
{
  return last_turns;
}

//! Set the number of phase bins in the bins used to smooth
void Pulsar::Smooth::set_bins (float _bins)
{
  if (_bins <= 1 )
    throw Error (InvalidParam, "Pulsar::Smooth::set_bins",
		 "invalid bins = %f", _bins);

  last_bins = bins = _bins;
  turns = 0;
}

//! Get the number of phase bins in the bins used to smooth
float Pulsar::Smooth::get_bins () const
{
  return bins;
}

float Pulsar::Smooth::get_last_bins () const
{
  return last_bins;
}

float Pulsar::Smooth::get_bins (const Profile* profile)
{
  if (bins)
    return bins;
  else
    return last_bins = turns * float( profile->get_nbin() );
}
 
float Pulsar::Smooth::get_turns (const Profile* profile)
{
  if (turns)
    return turns;
  else
    return last_turns = bins / float( profile->get_nbin() );
}
 
