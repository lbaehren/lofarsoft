/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CoherentDedispersion.h"

using namespace Pulsar;
using namespace std;

CoherentDedispersion::CoherentDedispersion ()
  : Extension ("CoherentDedisperion")
{
  domain = Signal::Frequency;

  dispersion_measure = 0;
  doppler_correction = 0;

  // FITS BITPIX convention: default to IEEE floating point
  nbit_data = -32;
  nbit_chirp = -32;
}

//! Clone method
CoherentDedispersion* CoherentDedispersion::clone () const
{
  return new CoherentDedispersion (*this);
}

//! get the text interface 
TextInterface::Parser* CoherentDedispersion::get_interface()
{
  return new Interface (this);
}

const CoherentDedispersion::InputChannel& 
CoherentDedispersion::get_input (unsigned ichan) const
{
  check_index( ichan );
  return input[ichan];
}

CoherentDedispersion::InputChannel& 
CoherentDedispersion::get_input (unsigned ichan) 
{
  check_index( ichan );
  return input[ichan];
}

void CoherentDedispersion::check_index (unsigned ichan) const
{
  if (ichan >= input.size())
    throw Error (InvalidParam, "Pulsar::CoherentDedispersion::get_input",
		 "ichan=%u >= input nchan=%u", ichan, input.size());
}

const CoherentDedispersion::OutputChannel& 
CoherentDedispersion::InputChannel::get_output (unsigned ichan) const
{
  check_index( ichan );
  return output[ichan];
}

CoherentDedispersion::OutputChannel& 
CoherentDedispersion::InputChannel::get_output (unsigned ichan) 
{
  check_index( ichan );
  return output[ichan];
}

void CoherentDedispersion::InputChannel::check_index (unsigned ichan) const
{
  if (ichan >= output.size())
    throw Error (InvalidParam,
		 "Pulsar::CoherentDedispersion::InputChannel::get_input",
		 "ichan=%u >= output nchan=%u", ichan, output.size());
}
