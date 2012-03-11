/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardData.h"
#include "Pulsar/PolnProfile.h"

// #define _DEBUG 1

using namespace std;

//! Default constructor
/*! If specified, baseline and on-pulse regions are defined by select */
Calibration::StandardData::StandardData (const Pulsar::PolnProfile* select)
{
  stats = new Pulsar::PolnProfileStats;

  if (select)
    select_profile (select);
}

//! Select the baseline and on-pulse regions from profile
void Calibration::StandardData::select_profile (const Pulsar::PolnProfile* p)
{
  if (normalize)
    stats -> set_avoid_zero_determinant ();

  stats->select_profile (p);
  total_determinant = stats->get_total_determinant ();
}

//! Set the profile from which estimates will be derived
void Calibration::StandardData::set_profile (const Pulsar::PolnProfile* p)
{
  stats->set_profile (p);

#ifdef _DEBUG
  cerr << "Calibration::StandardData::set_profile onpulse nbin=" 
       << stats->get_stats()->get_onpulse_nbin() << endl;
#endif

  total_determinant = stats->get_total_determinant ();
}

//! Normalize estimates by the average determinant
void Calibration::StandardData::set_normalize (bool norm)
{
#ifdef _DEBUG
  cerr << "Calibration::StandardData::set_normalize " << norm << endl;
#endif

  if (norm)
    normalize = new MEAL::NormalizeStokes;
  else
    normalize = 0;
}

//! Get the Stokes parameters of the specified phase bin
Stokes< Estimate<double> >
Calibration::StandardData::get_stokes (unsigned ibin)
{
  Stokes< Estimate<double> > result = stats->get_stokes (ibin);

  if (normalize)
  {
#ifdef _DEBUG1
    cerr << "Calibration::StandardData::get_stokes normalize total_det="
	 << total_determinant << endl;
#endif
    normalize->normalize (result, total_determinant);
    result *= sqrt( (double) stats->get_stats()->get_onpulse_nbin() );
  }

#ifdef _DEBUG
  cerr << "Calibration::StandardData::get_stokes ibin=" << ibin << endl
       << "result=" << result << endl;
#endif

  return result;
}

//! Get the algorithm used to compute the profile statistics
Pulsar::PolnProfileStats* Calibration::StandardData::get_poln_stats ()
{
  return stats;
}
