/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SolverInfo.h"
#include "Pulsar/PolnCalibrator.h"

//! Constructor
Pulsar::SolverInfo::SolverInfo (const PolnCalibrator* cs)
{
  poln_calibrator = cs;
}
    
std::string Pulsar::SolverInfo::get_title () const
{
  return "Goodness of Fit (Reduced \\gx\\u2\\d)";
}

//! Return the number of parameter classes
unsigned Pulsar::SolverInfo::get_nclass () const
{
  return 1;
}

//! Return the name of the specified class
std::string Pulsar::SolverInfo::get_name (unsigned iclass) const
{
  return "\\gx\\u2\\d/N\\dfree";
}


//! Return the number of parameters in the specified class
unsigned Pulsar::SolverInfo::get_nparam (unsigned iclass) const
{
  return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::SolverInfo::get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const
{
  const MEAL::LeastSquares* solver = poln_calibrator->get_solver (ichan);

  if (!solver->get_solved())
    return 0.0;

  double reduced_chisq = solver->get_chisq() / solver->get_nfree();
  double variance = 1.0 / solver->get_nfree();

  return Estimate<float> (reduced_chisq, variance);
}

