/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/ReceptionModelReport.h"
#include "Pulsar/CoherencyMeasurementSet.h"

#include <fstream>

using namespace std;

bool Calibration::ReceptionModel::Solver::report_chisq = false;

void Calibration::ReceptionModel::Solver::set_prefit_report
(const std::string& filename)
{
  prefit_report_filename = filename;
}

/*! Count the number of parameters that are to be fit, set nparam_infit */
void Calibration::ReceptionModel::Solver::count_infit () try
{
  if (verbose)
    cerr << "Calibration::ReceptionModel::Solver::count_infit" << endl;

  if (report)
    report << equation->get_nparam()
	   << " model parameters [index:free name=value]" << endl << endl;

  nparam_infit = 0;

  for (unsigned iparm=0; iparm < equation->get_nparam(); iparm++)
  {
    if (report)
      report << iparm << ":" << equation->get_infit(iparm) << " "
	     << equation->get_param_name(iparm)
	     << "=" << equation->get_param(iparm) << endl;
    if (equation->get_infit(iparm))
      nparam_infit ++;
  }

  if (report)
    report << nparam_infit << " free parameters" << endl;
}
catch (Error& error)
{
  throw error += "Calibration::ReceptionModel::Solver::count_infit";
}


void Calibration::ReceptionModel::Solver::count_constraint () try
{
  if (verbose)
    cerr << "Calibration::ReceptionModel::Solver::count_constraint" << endl;

  state_observed.resize (equation->get_num_input());
  for (unsigned i=0; i<state_observed.size(); i++)
    state_observed[i] = false;

  ndat_constraint = 0;

  for (unsigned idat=0; idat < equation->data.size(); idat++)
  {
    // ensure that each source_index is valid and flag the input states
    // that have at least one measurement to constrain them
    for (unsigned isource=0; isource < equation->data[idat].size(); isource++)
    {
      unsigned source_index = equation->data[idat][isource].get_input_index();

      if (source_index >= equation->get_num_input())
	throw Error (InvalidRange,
		     "Calibration::ReceptionModel::Solver::count_constraint",
		     "isource=%u >= nsource=%u",
		     source_index, equation->get_num_input());
      
      state_observed[source_index] = true;

      // count the number of constraints provided by each CoherencyMeasurement
      ndat_constraint += equation->data[idat][isource].get_nconstraint ();
    }
  }
  
  if (ndat_constraint <= nparam_infit)
    throw Error (InvalidState,
		 "Calibration::ReceptionModel::Solver::count_constraint",
		 "ndata=%u <= nfree=%u", ndat_constraint, nparam_infit);

  if (report)
    cerr << endl << ndat_constraint << " independent data" << endl << endl;
}
catch (Error& error)
{
  throw error += "Calibration::ReceptionModel::Solver::count_constraint";
}

void Calibration::ReceptionModel::Solver::check_constraints () try
{
  if (verbose)
    cerr << "Calibration::ReceptionModel::Solver::check_constraints" << endl;

  // check that all inputs with unknown parameters have a CoherencyMeasurement
  for (unsigned ipath=0; ipath < equation->get_num_transformation(); ipath++)
  {
    for (unsigned isource=0; isource < equation->get_num_input(); isource++)
    {
      bool need_source = false;

      equation->set_input_index (isource);
      const MEAL::Function* state = equation->get_input ();
      
      for (unsigned iparam=0; iparam < state->get_nparam(); iparam++)
	if( state->get_infit(iparam) )
	  need_source = true;
      
      if (need_source && !state_observed[isource])
	throw Error (InvalidRange,
		     "Calibration::ReceptionModel::Solver::check_constraints",
		     "input source %u with free parameter(s) not observed",
		     isource);

    }
  }
}
catch (Error& error)
{
  throw error += "Calibration::ReceptionModel::Solver::check_constraints";
}

/*! Uses the Levenberg-Marquardt algorithm of non-linear least-squares
    minimization in order to find the best fit to the observations. */
void Calibration::ReceptionModel::Solver::solve () try
{
  if (!prefit_report_filename.empty())
    report.open( prefit_report_filename.c_str() );

  count_infit ();
  count_constraint ();

  if (report)
    report.close();

  nfree = ndat_constraint - nparam_infit;

  check_constraints ();

  singular = false;
  solved = false;

  fit ();

  check_solution ();
  set_variances ();

  solved = true;
}
catch (Error& error)
{
  throw error += "Calibration::ReceptionModel::Solver::solve";
}

void Calibration::ReceptionModel::Solver::check_solution ()
{
  if (iterations >= maximum_iterations)
  {
    if (debug)
      cerr << "maximum iterations exceeded" << endl;

    throw Error (InvalidState, 
		 "Calibration::ReceptionModel::Solver::check_solution",
		 "exceeded maximum number of iterations=%u",
		 maximum_iterations);
  }

  float reduced_chisq = best_chisq / nfree;

  if (report_chisq)
    cerr << "  reduced chisq " << reduced_chisq << endl;

  if (!finite(reduced_chisq) ||
      (maximum_reduced && reduced_chisq > maximum_reduced))
    throw Error (InvalidState,
		 "Calibration::ReceptionModel::Solver::check_solution",
		 "bad reduced chisq=%f (nfree=%u)", reduced_chisq, nfree);

  if (verbose)
    cerr << "Calibration::ReceptionModel::Solver::check_solution " 
	 << iterations << " iterations. chi_sq=" 
	 << best_chisq << "/(" << ndat_constraint << "-" << nparam_infit
	 << "=" << nfree << ")=" << reduced_chisq << endl;
}

void Calibration::ReceptionModel::Solver::set_variances ()
{
  for (unsigned iparam=0; iparam < equation->get_nparam(); iparam++)
  {
    double variance = covariance[iparam][iparam];

    if (verbose)
      cerr << "Calibration::ReceptionModel::Solver::set_variances"
	" variance[" << iparam << "]=" << variance << endl;

    if (!finite(variance))
      throw Error (InvalidState, 
		   "Calibration::ReceptionModel::Solver::set_variances",
		   "non-finite variance "
		   + equation->get_param_name(iparam));

    if (!equation->get_infit(iparam) && variance != 0)
      throw Error (InvalidState,
		   "Calibration::ReceptionModel::Solver::set_variances",
		   "non-zero unfit variance "
		   + equation->get_param_name(iparam)
		   + " = " + tostring(variance) );

    if (variance < 0)
      throw Error (InvalidState,
		   "Calibration::ReceptionModel::Solver::set_variances",
		   "invalid variance " + equation->get_param_name(iparam)
		   + " = " + tostring(variance) );

    // cerr << iparam << ".var=" << variance << endl;

    equation->set_variance (iparam, variance);
  }

  for (unsigned i=0; i < acceptance_condition.size(); i++)
    if ( !acceptance_condition[i](equation) )
      throw Error (InvalidState,
		   "Calibration::ReceptionModel::Solver::set_variances",
		   "model not accepted by condition #%u", i);
}

//! The observations used to constrain the measurement equations
std::vector<Calibration::CoherencyMeasurementSet>& 
Calibration::ReceptionModel::Solver::get_data ()
{
  return equation->data;
}

//! Add a convergence conditions
void Calibration::ReceptionModel::Solver::add_convergence_condition
( Functor< bool(ReceptionModel*) > condition )
{
  convergence_condition.push_back( condition );
}

//! Add a report
void Calibration::ReceptionModel::Solver::add_acceptance_condition
( Functor< bool(ReceptionModel*) > condition )
{
  acceptance_condition.push_back( condition );
}

void Calibration::ReceptionModel::Solver::set_equation (ReceptionModel* m)
{
  equation = m;
}
