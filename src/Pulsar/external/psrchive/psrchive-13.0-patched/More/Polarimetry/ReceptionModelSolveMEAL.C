/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelSolveMEAL.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "MEAL/LevenbergMarquardt.h"

#include <iostream>

using namespace std;

std::string Calibration::SolveMEAL::get_name () const
{
  return "MEAL";
}

Calibration::SolveMEAL* Calibration::SolveMEAL::clone () const
{
  return new SolveMEAL;
}

// ///////////////////////////////////////////////////////////////////////////
//
// template specializations that enable the use of MEAL::LevenbergMarquardt
//
// ///////////////////////////////////////////////////////////////////////////

// template specialization of MEAL::lmcoff
float lmcoff (// input
	      Calibration::ReceptionModel& model,
	      const Calibration::CoherencyMeasurement& obs,
	      const Estimate<char>& ignored,
	      // storage
	      vector<Jones<double> >& gradient,
	      // output
	      vector<vector<double> >& alpha,
	      vector<double>& beta)
{
  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff input index=" 
	 << obs.get_input_index() << endl;

  model.set_input_index (obs.get_input_index());

  Jones<double> result = model.evaluate (&gradient);

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff"
      "\n  data=" << obs.get_coherency() <<
      "\n  model=" << result << endl;

  Jones<double> delta_y = obs.get_coherency() - result;

  /* Note that Calibration::CoherencyMeasurement implements the interface
     of the WeightingScheme template class used by LevenbergMacquardt */
  return MEAL::lmcoff1 (model, delta_y, obs, gradient, alpha, beta);
}

// template specialization of MEAL::lmcoff
float lmcoff (// input
	      Calibration::ReceptionModel& model,
	      const Calibration::CoherencyMeasurementSet& data,
	      const Estimate<char>& ignored,
	      // storage
	      vector<Jones<double> >& gradient,
	      // output
	      vector<vector<double> >& alpha,
	      vector<double>& beta)
{
  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff set abscissa" << endl;

  // set the independent variables for this set of measurements
  data.set_coordinates();
  // set the signal path through which these measurements were observed
  model.set_transformation_index (data.get_transformation_index());

  double chisq = 0.0;
  for (unsigned ist=0; ist<data.size(); ist++)
    chisq += lmcoff (model, data[ist], ignored, gradient, alpha, beta);

  return chisq;
}


void Calibration::SolveMEAL::fit ()
{
  if (!equation)
    throw Error (InvalidState, "Calibration::SolveMEAL::fit", "no equation");

  // the engine used to find the chi-squared minimum
  MEAL::LevenbergMarquardt< Jones<double> > fit;

  // get info from all of the MEAL classes
  // MEAL::Function::verbose = 1;

  // get info from the LevenbergMarquardt algorithm
  // fit.verbose = 3;

  // get info from this method
  // debug = true;

  // The abscissa, ordinate and ordinate error are contained in
  // Calibration::CoherencyMeasurementSet
  vector< Estimate<char> > fake (get_data().size());

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::SolveMEAL::fit compute initial fit" << endl;

  try {
    best_chisq = fit.init (get_data(), fake, *equation);
  }
  catch (Error& error)
  {
    error << "\n\t" "init";
    throw error += "Calibration::SolveMEAL::fit";
  }

  fit.lamda = 1e-5;
  fit.lamda_increase_factor = 10;
  fit.lamda_decrease_factor = 0.5;

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::SolveMEAL::fit chisq=" << best_chisq << endl;

  float last_lamda = 0.0;

  unsigned stick_to_steepest_decent = 0;
  unsigned patience = 5;

  for (iterations = 0; iterations < maximum_iterations; iterations++) try
  {
    float chisq = fit.iter (get_data(), fake, *equation);

    if (debug)
      cerr << "ITERATION: " << iterations << endl;

    if (convergence_chisq)
    {
      if (debug)
	cerr << "chisq=" << chisq << " convergence="
	     << convergence_chisq << endl;

      if (chisq < convergence_chisq)
	break;
      else
	continue;
    }

    float delta_chisq = chisq - best_chisq;
    float reduced_chisq = chisq / nfree;

    if (Calibration::ReceptionModel::verbose || debug)
      cerr << "chisq=" << chisq << " delta_chisq=" << delta_chisq
           << " reduced_chisq=" << reduced_chisq
	   << " lamda=" << fit.lamda << endl;

    if (chisq < best_chisq)
      best_chisq = chisq;

    bool reiterate = false;
    for (unsigned i=0; i < convergence_condition.size(); i++)
      if ( !convergence_condition[i](equation) )
	reiterate = true;

    if (reiterate)
      continue;

    if (fit.lamda == 0.0 && fabs(delta_chisq) < 1.0 && delta_chisq <= 0)
    {
      if (debug)
	cerr << "fit good" << endl;
      break;
    }

    if (fit.lamda == 0.0 && delta_chisq > 0)
    {
      if (debug)
	cerr << "maybe not so good" << endl;
      fit.lamda = last_lamda;

      // count when Newton's method seems to be doing very poorly
      stick_to_steepest_decent ++;
    }

    if (delta_chisq <= 0 && fabs(delta_chisq) < 10)
    {
      if (debug)
	cerr << "fit close" << endl;

      if (stick_to_steepest_decent >= 5)
      {
	if (iterations >= maximum_iterations/2 &&
	    fabs(delta_chisq)/best_chisq < 1e-3)
	{
	  if (debug)
	    cerr << "small change in late stages.  patience="
		 << patience << endl;

	  patience --;

	  if (!patience)
	  {
	    if (debug)
	      cerr << "no more patience" << endl;
	    break;
	  }
	}

	if (debug)
	  cerr << "remain patient!" << endl;
      }
      else
      {
	if (debug)
	  cerr << "go for it!" << endl;
	if (fit.lamda != 0)
	  last_lamda = fit.lamda;
	fit.lamda = 0.0;
      }
    }
  }
  catch (Error& error)
  {
    /* Each iterative step includes inversion of the Hessian matrix.
       If this fails, then it is likely singular (i.e. there is an
       ill-constrained free parameter). */

    singular = true;
    error << "\n\t" "iteration=" << iterations;
    throw error += "Calibration::SolveMEAL::fit";
  }

  if (iterations == maximum_iterations)
    return;

  try
  {
    fit.result (*equation, covariance);
  }
  catch (Error& error)
  {
    error << "\n\t" "result";
    throw error += "Calibration::SolveMEAL::fit";
  }

  if (covariance.size() != equation->get_nparam())
    throw Error (InvalidState, "Calibration::SolveMEAL::fit",
		 "MEAL::LevenbergMarquardt<Jones<double>>::result returns"
		 "\n\tcovariance matrix dimension=%d != nparam=%d",
		 covariance.size(), equation->get_nparam());
}
