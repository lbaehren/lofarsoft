/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WeightedFrequency.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;


Pulsar::Option<bool> Pulsar::WeightedFrequency::round_to_kHz
(
 "WeightedFrequency::round_to_kHz", true,

 "Round mean frequency to nearest kHz [boolean]",

 "If true, then WeightedFrequency (used by tscrunch and fscrunch)\n"
 "will round the weighted mean frequency to the nearest value in kHz"
);

/*!
  \return the weighted mean centre frequency (in MHz)
  \param  start the index of the first frequency to include in the mean
  \param  end one more than the index of the last frequency
*/
double Pulsar::WeightedFrequency::operator () (unsigned istart,
					       unsigned iend) const
{
  unsigned nindex = get_nindex();

  if (iend == 0)
    iend = nindex;

  if (nindex == 0)
    throw Error (InvalidRange, "Pulsar::WeightedFrequency::get_mean",
                 "nindex == 0");

  if (istart >= nindex)
    throw Error (InvalidRange, "Pulsar::WeightedFrequency::get_mean",
		 "istart=%d nindex=%d", istart, nindex);

  if (iend > nindex)
    throw Error (InvalidRange, "Pulsar::WeightedFrequency::get_mean",
		 "iend=%d nindex=%d", iend, nindex);

  double weightsum = 0.0;
  double freqsum = 0.0;

  double fstart = 0.0;
  double fend = 0.0;

  for (unsigned index=istart; index < iend; index++) try
  {
    double freq   = get_frequency (index);
    double weight = get_weight (index);
    
    if (Archive::verbose > 2)
      cerr << "Pulsar::WeightedFrequency::get_mean [" << index << "]"
	" freq=" << freq << " wt=" << weight << endl;
      
    freqsum += freq * weight;
    weightsum += weight;
    
    if (index == istart)
      fstart = freq;
    if (index == iend-1)
      fend = freq;
  }
  catch (Error& err)
  {
    throw err += "Pulsar::WeightedFrequency::get_mean";
  }
  
  double result = 0.0;
  
  if (weightsum != 0.0)
  {
    result = freqsum / weightsum;
    if (Archive::verbose > 2)
      cerr << "Pulsar::WeightedFrequency::get_mean mean=" << result << endl;
  }
  else
  {
    result = 0.5 * ( fstart + fend );
    if (Archive::verbose > 2)
      cerr << "Pulsar::WeightedFrequency::get_mean mid=" << result << endl;
  }

  if (round_to_kHz)
  {
    // Nearest kHz
    result = 1e-3 * double( int(result*1e3) );

    if (Archive::verbose > 2)
      cerr << "Pulsar::WeightedFrequency::get_mean kHz=" << result*1e3 << endl;
  }

  return result;
}

Pulsar::WeightedFrequency::OverEpoch::OverEpoch (const Archive* _archive)
{
  archive = _archive;
  ichan = 0;
  ipol = 0;
}

void Pulsar::WeightedFrequency::OverEpoch::set_Archive (const Archive* a)
{
  archive = a;
}

void Pulsar::WeightedFrequency::OverEpoch::set_ichan (unsigned _ichan)
{
  ichan = _ichan;
}

unsigned
Pulsar::WeightedFrequency::OverEpoch::get_nindex () const
{
  return archive->get_nsubint ();
}

double
Pulsar::WeightedFrequency::OverEpoch::get_frequency (unsigned isubint) const
{
  return archive->get_Profile (isubint, ipol, ichan)->get_centre_frequency();
}

double
Pulsar::WeightedFrequency::OverEpoch::get_weight (unsigned isubint) const
{
  return archive->get_Profile (isubint, ipol, ichan)->get_weight();
}


Pulsar::WeightedFrequency::OverFrequency::OverFrequency (const Integration* s)
{
  subint = s;
  ipol = 0;
}

void
Pulsar::WeightedFrequency::OverFrequency::set_Integration
(const Integration* s)
{
  subint = s;
}

unsigned
Pulsar::WeightedFrequency::OverFrequency::get_nindex () const
{
  return subint->get_nchan ();
}

double
Pulsar::WeightedFrequency::OverFrequency::get_frequency (unsigned ichan) const
{
  return subint->get_Profile (ipol, ichan)->get_centre_frequency();
}

double
Pulsar::WeightedFrequency::OverFrequency::get_weight (unsigned ichan) const
{
  return subint->get_Profile (ipol, ichan)->get_weight();
}
