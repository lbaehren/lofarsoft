/***************************************************************************
 *
 *   Copyright (C) 2003 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SourceEstimate.h"
#include "Pulsar/Calibrator.h"

#include <iostream>
using namespace std;

//! Construct with the specified bin from Archive
Pulsar::SourceEstimate::SourceEstimate (int ibin)
{
  phase_bin = ibin;
  input_index = 0;
  add_data_attempts = 0;
  add_data_failures = 0;
}

using Calibration::ReceptionModel;

void Pulsar::SourceEstimate::create_source (ReceptionModel* equation)
{
  source = new MEAL::Coherency;

  input_index = equation->get_num_input();

  equation->add_input( source );
}


bool Pulsar::SourceEstimate::is_constrained () const
{
  return add_data_attempts - add_data_failures > multiples.size();
}

/*! Update the best guess of each unknown input state */
void Pulsar::SourceEstimate::update_source ()
{
  if (add_data_attempts && add_data_failures)
  {
    cerr << "Failed to add data " << add_data_failures << " out of "
	 << add_data_attempts << " times";

    if (phase_bin >= 0)
      cerr << " for phase bin " << phase_bin;

    cerr << endl;
  }

  valid = true;

  try 
  {
    source_guess.update( source );
  }
  catch (Error& error)
  {
    if (Calibrator::verbose > 2)
      cerr << "Pulsar::SourceEstimate::update_source error "
	   << error << endl;
    valid = false;
  }
}

