/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/WeightedFrequency.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, rounded to nearest kHz)
  \param  chan_start the first chan included in the calculation
  \param  chan_end one more than the index of the last chan
*/
double Pulsar::Integration::weighted_frequency (unsigned chan_start,
						unsigned chan_end) const
{
  WeightedFrequency::OverFrequency weighted (this);
  return weighted (chan_start, chan_end);
}
