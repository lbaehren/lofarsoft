/***************************************************************************
 *
 *   Copyright (C) 2004 by Matthew Bailes
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

/*!
  Returns the geometric mean of the rms of the baseline
  for the 0,0th profile in each Integration. MB Feb 2004.
  */

float Pulsar::Archive::rms_baseline (float baseline_width)
{
  if (get_nsubint() == 0)
    return 0.0;
  double sum_rms_sq;
  sum_rms_sq = 0.0;
  // find the mean and the r.m.s. of the baseline
  double min_avg, min_var;
  for (unsigned isub=0; isub < get_nsubint(); isub++){
    get_Profile(isub,0,0)->stats (
      get_Profile(isub,0,0)->find_min_phase(baseline_width), &min_avg, &min_var
    );
    sum_rms_sq += min_var;
  }
  return (float) sqrt(sum_rms_sq)/ sqrt((float) get_nsubint());
}

