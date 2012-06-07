/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyIntegrate.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include "ModifyRestore.h"
#include "Error.h"

using namespace std;

//! Default constructor
Pulsar::FrequencyIntegrate::FrequencyIntegrate ()
{
  set_range_policy( new EvenlySpaced );
  dedisperse = true;
  defaraday = true;
}

//! Correct dispersion before integrating, if necessary
void Pulsar::FrequencyIntegrate::set_dedisperse (bool flag)
{
  dedisperse = flag;
}

bool Pulsar::FrequencyIntegrate::get_dedisperse () const
{
  return dedisperse;
}

//! Correct Faraday rotation before integrating, if necessary
void Pulsar::FrequencyIntegrate::set_defaraday (bool flag)
{
  defaraday = flag;
}

bool Pulsar::FrequencyIntegrate::get_defaraday () const
{
  return defaraday;
}

void Pulsar::FrequencyIntegrate::transform (Integration* integration)
{
  unsigned subint_nchan = integration->get_nchan();
  unsigned subint_npol  = integration->get_npol();

  range_policy->initialize (this, integration);

  unsigned output_nchan = range_policy->get_nrange();

  if (subint_nchan <= 1 || output_nchan >= subint_nchan)
  {
    if (Integration::verbose) 
      cerr << "Pulsar::FrequencyIntegrate::transform nothing to do" << endl;
    return;
  }

  double dm = integration->get_effective_dispersion_measure();
  bool must_dedisperse = dedisperse &&  dm != 0;

  double rm = integration->get_effective_rotation_measure();
  bool must_defaraday = defaraday && subint_npol == 4 && rm != 0;

  if (Integration::verbose)
    cerr << 
      "Pulsar::FrequencyIntegrate::transform nchan"
      " input=" << subint_nchan << " output=" << output_nchan << endl <<
      "Pulsar::FrequencyIntegrate::transform"
      " dm=" << dm << " fix=" << must_dedisperse <<
      " rm=" << rm << " fix=" << must_defaraday << endl;

  unsigned start = 0;
  unsigned stop = 0;

  ModifyRestore<bool> mod (range_checking_enabled, false);

  for (unsigned ichan=0; ichan < output_nchan; ichan++) try
  {     
    range_policy->get_range (ichan, start, stop);
    
    double reference_frequency = integration->weighted_frequency (start,stop);

    if (Integration::verbose)
      cerr << "Pulsar::FrequencyIntegrate::transform ichan=" << ichan 
	   << " freq=" << reference_frequency << endl;

    if (must_dedisperse)
      integration->expert()->dedisperse (start, stop, reference_frequency);

    if (must_defaraday)
      integration->expert()->defaraday (start, stop, reference_frequency);

    for (unsigned ipol=0; ipol < subint_npol; ipol++)
    {
      if (Integration::verbose)
	cerr << "Pulsar::FrequencyIntegrate::transform ipol=" << ipol << endl;

      Profile* output = integration->get_Profile (ipol, ichan);

      for (unsigned jchan=start; jchan<stop; jchan++)
      {
	Profile* input  = integration->get_Profile (ipol, jchan);
	if (jchan==start)
	  *(output) = *(input);
	else
	  output->average (input);
      } 
    }

    integration->set_centre_frequency (ichan, reference_frequency);
  }
  catch (Error& error)
  {
   throw error += "FrequencyIntegrate::transform";
  }

  if (Integration::verbose)
    cerr << "Pulsar::FrequencyIntegrate::transform resize" << endl;

  integration->expert()->resize (0, output_nchan, 0);

  if (Integration::verbose) 
    cerr << "Pulsar::FrequencyIntegrate::transform finish" << endl;
} 
