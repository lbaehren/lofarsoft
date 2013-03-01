/***************************************************************************
 *
 *   Copyright (C) 2002-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/WeightedFrequency.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, rounded to nearest kHz)
  \param  ichan the index of the requested frequency channel
  \param  start the index of the first Integration to include in the mean
  \param  end one more than the index of the last Integration
*/
double Pulsar::Archive::weighted_frequency (unsigned ichan,
					    unsigned start,
					    unsigned end) const
{
  WeightedFrequency::OverEpoch weighted (this);
  weighted.set_ichan (ichan);

  return weighted (start, end);
}

