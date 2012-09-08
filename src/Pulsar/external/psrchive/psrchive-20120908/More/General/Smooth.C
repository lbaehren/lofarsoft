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
  if (_turns < 0 || _turns >= 1.0)
    throw Error (InvalidParam, "Pulsar::Smooth::set_turns",
		 "invalid turns = %f", _turns);

  last_turns = turns = _turns;

  if (turns != 0.0)
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
  if (_bins < 0 )
    throw Error (InvalidParam, "Pulsar::Smooth::set_bins",
		 "invalid bins = %f", _bins);

  last_bins = bins = _bins;

  if (bins != 0)
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
 

Pulsar::Smooth::Interface::Interface (Smooth* instance)
{
  if (instance)
    set_instance (instance);

  add( &Smooth::get_turns,
       &Smooth::set_turns,
       "turns", "smoothing window size in turns" );

  add( &Smooth::get_bins,
       &Smooth::set_bins,
       "bins", "smoothing window size in bins" );
}

//
// the following could probably be moved it's own compilation unit
//

#include "interface_factory.h"

#include "Pulsar/SmoothMean.h"
#include "Pulsar/SmoothMedian.h"
#include "Pulsar/SmoothSinc.h"

Pulsar::Smooth* Pulsar::Smooth::factory (const std::string& name_parse)
{
  std::vector< Reference::To<Smooth> > instances;

  instances.push_back( new SmoothMean );
  instances.push_back( new SmoothMedian );
  instances.push_back( new SmoothSinc );

  return TextInterface::factory<Smooth> (instances, name_parse);
}

