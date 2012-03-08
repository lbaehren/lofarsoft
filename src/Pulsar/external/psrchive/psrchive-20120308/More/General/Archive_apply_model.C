/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

#include <iostream>
using namespace std;

// ///////////////////////////////////////////////////////////////////////
//
// Archive::apply_model
//
/*!
  This method aligns the Integration to the current polyco, as stored
  in the model attribute.  The Integration is rotated by the difference
  between the phase predicted by the current model and that predicted by
  the old model.
  \param old the old polyco used to describe subint
  \param subint pointer to the Integration to be aligned to the current model
*/
void Pulsar::Archive::apply_model (Integration* subint, const Predictor* old)
{
  if ( !model )
    throw Error (InvalidState, "Pulsar::Archive::apply_model",
		 "no Predictor");

  try {

    // get the MJD of the rising edge of bin zero
    MJD epoch = subint -> get_epoch();

    if (verbose > 2)
      cerr << "Pulsar::Archive::apply_model compute new phase" << endl;

    // get the phase of the rising edge of bin zero
    Phase phase = model->phase (epoch);
    
    // the Integration is rotated by -phase to bring zero phase to bin zero
    Phase dphase = - phase;
    
    long double period = 1.0 / model->frequency (epoch);
    long double shift_time = dphase.fracturns() * period;
    
    if (verbose == 3) {

      Phase old_phase;
      if (old)
      {
        if (verbose > 2)
          cerr << "Pulsar::Archive::apply_model compute old phase" << endl;
	old_phase = old->phase(epoch);
      }

      cerr << "Pulsar::Archive::apply_model"
	   << "\n  old MJD " << epoch;

      if (old)
	cerr << "\n  old predictor phase " << old_phase << endl;

      cerr << "\n  new predictor phase " << phase
	   << "\n  time shift " << shift_time/86400.0 << " days" 
           << "\n             " << shift_time << " seconds "
	   << "\n  total phase shift " << dphase << endl; 
    }

    subint -> set_folding_period (period);  
    subint -> rotate (shift_time);
    subint -> zero_phase_aligned = true;

    if (verbose == 3) {
      epoch = subint -> get_epoch();
      phase = model->phase(epoch);
      cerr << "Pulsar::Archive::apply_model"
	   << "\n  new MJD "   << epoch
	   << "\n  new phase " << phase
	   << endl;

    }
  }
  catch (Error& err) {
    throw err += "Pulsar::Archive::apply_model";
  }

}
