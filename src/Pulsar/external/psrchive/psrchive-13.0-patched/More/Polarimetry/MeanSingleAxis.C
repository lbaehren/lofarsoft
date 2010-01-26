/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MeanSingleAxis.h"
#include "Pulsar/SingleAxis.h"
#include "EstimateStats.h"

using namespace std;

void Calibration::MeanSingleAxis::update (MEAL::Complex2* model) const
{
  if (MEAL::Function::very_verbose)
    cerr << "Calibration::MeanSingleAxis::update (MEAL::Complex2*)" << endl;

  SingleAxis* single_axis = dynamic_cast<SingleAxis*>(model);
  if (!single_axis)
    throw Error (InvalidParam, "Calibration::MeanSingleAxis::update",
		 "Complex2 model is not a SingleAxis");

  update (single_axis);
}

void Calibration::MeanSingleAxis::update (SingleAxis* single_axis) const
{
  if (MEAL::Function::very_verbose)
    cerr << "Calibration::MeanSingleAxis::update (SingleAxis*)"
      "\n  gain=" << mean_gain.get_Estimate() <<
      "\n  diff_gain=" << mean_diff_gain.get_Estimate() <<
      "\n  diff_phase=" << mean_diff_phase.get_Estimate() << endl;

  // avoid setting the gain to zero if nothing has been added to the mean
  Estimate<double> gain = mean_gain.get_Estimate();
  if (gain.get_value() == 0)
    single_axis->set_gain (1.0);
  else
    single_axis->set_gain (mean_gain.get_Estimate());

  single_axis->set_diff_gain  (mean_diff_gain.get_Estimate());
  single_axis->set_diff_phase (0.5 * mean_diff_phase.get_Estimate());
}

void Calibration::MeanSingleAxis::integrate (const MEAL::Complex2* model)
{
  const SingleAxis* single_axis = dynamic_cast<const SingleAxis*>(model);
  if (!single_axis)
    throw Error (InvalidParam, "Calibration::MeanSingleAxis::integrate",
		 "Complex2 model is not a SingleAxis");
  
  integrate (single_axis);
}

void Calibration::MeanSingleAxis::integrate (const SingleAxis* single_axis)
{
  mean_gain       += single_axis->get_gain ();
  mean_diff_gain  += single_axis->get_diff_gain ();
  mean_diff_phase += 2.0 * single_axis->get_diff_phase ();
}

double Calibration::MeanSingleAxis::chisq (const MEAL::Complex2* model) const
{
  const SingleAxis* single_axis = dynamic_cast<const SingleAxis*>(model);
  if (!single_axis)
    throw Error (InvalidParam, "Calibration::MeanSingleAxis::chisq",
		 "Complex2 model is not a SingleAxis");
  
  return chisq (single_axis);
}

double Calibration::MeanSingleAxis::chisq (const SingleAxis* single_axis) const
{
  MeanRadian<double> temp (2.0 * single_axis->get_diff_phase());

  return 0.5 * 
    ( ::chisq (mean_diff_gain.get_Estimate(), single_axis->get_diff_gain()) +
      ::chisq (mean_diff_phase, temp) );
}

