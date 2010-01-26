/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeIntegrate.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/IntegrationExtension.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Predictor.h"
#include "Pulsar/Pulsar.h"
#include "Pulsar/DigitiserCounts.h"

#include "ModifyRestore.h"
#include "Error.h"

using namespace std;

double weight (Pulsar::Integration* subint)
{
  double result = 0;
  for (unsigned ichan=0; ichan < subint->get_nchan(); ichan++)
    result += subint->get_weight (ichan);
  return result;
}

void Pulsar::TimeIntegrate::transform (Archive* archive) try 
{
  // Account for custom Integration ordering:
  IntegrationOrder* order = archive->get<IntegrationOrder>();

  if (order)
  {
    Divided* divided = dynamic_kast<Divided>( range_policy );
    if (!divided)
      throw Error( InvalidState, "Pulsar::TimeIntegrate::transform",
		   "Archive has IntegrationOrder but "
		   "range policy is not Divided" );

    order->combine( archive, divided->get_nintegrate() );
    return;
  }

  unsigned archive_nsub = archive->get_nsubint();
  unsigned archive_nchan = archive->get_nchan();
  unsigned archive_npol = archive->get_npol();

  range_policy->initialize (this, archive);
  unsigned output_nsub = range_policy->get_nrange();

  if (archive_nsub <= 1 || output_nsub >= archive_nsub)
  {
    if (Archive::verbose > 2) 
      cerr << "Pulsar::TimeIntegrate::transform nothing to do" << endl;
    return;
  }

  if ( Archive::verbose > 1 &&
       archive->has_model() && 
       !archive->expert()->zero_phase_aligned() )
    cerr << "Pulsar::TimeIntegrate::transfrom WARNING"
      " not all Integrations are zero phase aligned" << endl;

  double dm = archive->get_dispersion_measure();
  bool must_dedisperse = dm != 0 && !archive->get_dedispersed();

  double rm = archive->get_rotation_measure();
  bool must_defaraday = archive_npol == 4 &&
    rm != 0 && !archive->get_faraday_corrected();

  if (Archive::verbose > 2)
    cerr << "Pulsar::TimeIntegrate::transform nsubint"
      " input=" << archive_nsub << " output=" << output_nsub << 
      " dm=" << dm << " rm=" << rm << endl;

  unsigned start = 0;
  unsigned stop = 0;

  ModifyRestore<bool> mod (range_checking_enabled, false);

  DigitiserCounts *digitiserCounts = archive->get<DigitiserCounts>();

  for (unsigned isub=0; isub < output_nsub; isub++)
  {
    range_policy->get_range (isub, start, stop);

    if (Archive::verbose > 2)
      cerr << "Pulsar::TimeIntegrate::transform isub=" << isub << endl;

    Integration* result = archive->get_Integration (isub);
    
    // //////////////////////////////////////////////////////////////////////
    //
    //  compute the new duration and weighted mid-time of the result
    //
    // //////////////////////////////////////////////////////////////////////

    double duration = 0.0;
    double total_weight = 0.0;
    unsigned count = 0;

    bool weight_midtime = true;

    for (unsigned iadd=start; iadd < stop; iadd++)
    {
      Integration* cur = archive->get_Integration (iadd);
      
      duration += cur->get_duration();
      total_weight += weight (cur);
      count ++;
    }
    
    if (total_weight == 0)
    {
      total_weight = count;
      weight_midtime = false;
    }

    if (Archive::verbose > 2)
      cerr << "Pulsar::TimeIntegrate::transform total weight="
           << total_weight << endl;

    result->set_duration (duration);
    
    double avg_period=0.0;
    MJD epoch, alt_epoch;
    
    for (unsigned iadd=start; iadd < stop; iadd++)
    {
      Integration* cur = archive->get_Integration (iadd);

      double cur_weight = (weight_midtime) ? weight(cur) : 1.0;

      if (Archive::verbose > 2)
        cerr << "Pulsar::TimeIntegrate::transform isub=" << iadd << " weight=" 
             << cur_weight << " epoch=" << cur->get_epoch() << endl;

      epoch += cur_weight/total_weight * cur->get_epoch();

      avg_period += cur_weight/total_weight * cur->get_folding_period();

      if (iadd==(stop+start)/2)
        alt_epoch = cur->get_epoch();
    }

    if (digitiserCounts != NULL)
      digitiserCounts->CombineSubints(isub, start, stop);

    if (Archive::verbose > 2)
      cerr << "Pulsar::TimeIntegrate::transform weighted epoch=" 
           << epoch << endl;

    // //////////////////////////////////////////////////////////////////////
    //
    // round epoch to nearest integer period
    //
    // //////////////////////////////////////////////////////////////////////
    
    if (archive->get_type() == Signal::Pulsar)
    {
      //
      // ensure that the phase predictor includes the new integration time
      //

      //
      // can only be done if archive has an ephemeris and it is valid
      //
      if (archive->has_ephemeris()) try
      {
        archive->expert()->update_model (epoch);
      }
      catch (Error& error)
      {
	//
        // creation may fail if the ephemeris is only a place holder for 
	// attributes.  propagate the error only if there is an existing model
	//
        if (archive->has_model())
          throw error;

	if (Archive::verbose)
	  warning << "Pulsar::TimeIntegrate::transform"
                     " could not update pulse phase predictor" << endl;
      }
      
      if (archive->has_model())
      {
	const Predictor* model = archive->get_model();

	// get the time of the first subint to be integrated into isub
	MJD firstmjd = archive->get_Integration (start) -> get_epoch ();
	// get the phase at the time of the first subint
	Phase first_phase = model->phase(firstmjd);
	
	// get the phase at the midtime of the result
	Phase mid_phase = model->phase (epoch);
	// get the period at the midtime of the result
	double period = 1.0 / model->frequency (epoch);
	
	// set the phase at the midtime equal to that of the first subint
	Phase desired (mid_phase.intturns(), first_phase.fracturns());

	epoch = model->iphase (desired, &epoch);
  
	if (Archive::verbose > 2)
	  cerr << "TimeIntegrate::transform epoch=" << epoch 
               << " phase=" << model->phase(epoch) << endl;
	
	result->set_folding_period (period);
      }
      else
      {
        /*
	  If no model exists, then it is not possible to recompute an 
	  aribtrary epoch. Instead, use the epoch of a subint near
	  the middle of the range integrated (saved above as alt_epoch).
	*/

        if (Archive::verbose > 2)
          cerr << "TimeIntegrate::transform using alt_epoch=" << alt_epoch
	       << " diff=" << (alt_epoch-epoch).in_seconds() << "s" << endl;

        epoch = alt_epoch;
        result->set_folding_period (avg_period);
      }
    }
    
    result->set_epoch (epoch);
    
    // //////////////////////////////////////////////////////////////////////
    //
    // integrate Profile data
    //
    // //////////////////////////////////////////////////////////////////////
    
    for (unsigned ichan=0; ichan < archive_nchan; ichan++)
    {
      if (Archive::verbose > 2) 
	cerr << "Pulsar::TimeIntegrate::transform weighted_frequency chan="
	     << ichan << endl;
      
      double reference_frequency = 0.0;
      
      reference_frequency = archive->weighted_frequency (ichan, start, stop);
      
      if (Archive::verbose > 2) 
	cerr << "Pulsar::TimeIntegrate::transform ichan=" << ichan
	     << " new frequency=" << reference_frequency << endl;
      
      for (unsigned iadd=start; iadd < stop; iadd++)
      {
	Integration* subint = archive->get_Integration (iadd);
	
	if (must_dedisperse)
	  subint->expert()->dedisperse (ichan, ichan+1,
					dm, reference_frequency);
	
	if (must_defaraday)
	  subint->expert()->defaraday (ichan, ichan+1,
				       rm, reference_frequency);
	
	subint->set_centre_frequency (ichan, reference_frequency);
      }
      
      if (Archive::verbose > 2) 
	cerr << "Pulsar::TimeIntegrate::transform sum profiles" << endl;
      
      for (unsigned ipol=0; ipol < archive_npol; ++ipol)
      {
	Profile* avg = archive->get_Profile (isub, ipol, ichan);
	Profile* add = archive->get_Profile (start, ipol, ichan);
	
	*(avg) = *(add);
	
	for (unsigned jsub=start+1; jsub<stop; jsub++)
        {
	  add = archive->get_Profile (jsub, ipol, ichan);
	  avg->average (add);
	}
      } // for each poln
    } // for each channel
    
    // //////////////////////////////////////////////////////////////////////
    //
    // integrate Extension data
    //
    // //////////////////////////////////////////////////////////////////////

    for (unsigned iadd=start; iadd < stop; iadd++)
    {
      Integration* cur = archive->get_Integration (iadd);

      if (iadd == start)
	// transfer the Extensions from the start Integration to the result
	for (unsigned iext = 0; iext < cur->get_nextension(); iext++)
	  result->add_extension( cur->get_extension(iext) );
      else
	// integrate the Extensions into the result
	for (unsigned iext = 0; iext < result->get_nextension(); iext++)
	  result->get_extension(iext)->integrate (cur);
    }

    // //////////////////////////////////////////////////////////////////////
    //
    // update all Extensions
    //
    // //////////////////////////////////////////////////////////////////////

    for (unsigned iext = 0; iext < result->get_nextension(); iext++)
    {
      Integration::Extension* ext = result->get_extension(iext);
      ext->update (result);
    }

  } // for each integrated result

  if (digitiserCounts != NULL)
    digitiserCounts->subints.resize(output_nsub);

  archive->resize (output_nsub);
}

catch (Error& err)
{
  throw err += "Pulsar::TimeIntegrate::transform";
}


