/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/DeltaRM.h"
#include "Pulsar/DeltaPA.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/FrequencyIntegrate.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

Pulsar::DeltaRM::DeltaRM ()
{
  threshold = 3.0;
  used_bins = 0;
}

Pulsar::DeltaRM::~DeltaRM ()
{
}

//! Set the archive from which to derive the refined rotation measure
void Pulsar::DeltaRM::set_data (Archive* archive)
{
  data = archive;
  data->defaraday();
}

#if HAVE_PGPLOT

#include "Pulsar/StokesCylindrical.h"
#include <cpgplot.h>

static void plot (const Pulsar::Archive* archive, unsigned chan)
{
  string name = "band" + tostring(chan) + ".ps/cps";

  cpgbeg (0, name.c_str(), 0, 0);
  cpgslw(2);
  cpgsvp (.1,.9, .1,.9);
  
  Pulsar::StokesCylindrical plot;

  plot.set_chan (chan);
  plot.plot (archive);

  cpgend();
}

#endif

//! Set the phase bins to be included in the mean
void Pulsar::DeltaRM::set_include (const std::vector<unsigned>& bins)
{
  include_bins = bins;
}

//! Set the phase bins to be excluded exclude the mean
void Pulsar::DeltaRM::set_exclude (const std::vector<unsigned>& bins)
{
  exclude_bins = bins;
}

//! Refine the rotation measure estimate
void Pulsar::DeltaRM::refine ()
{
  if (!data)
    throw Error (InvalidState, "Pulsar::DeltaRM::refine", "no data");

  Reference::To<Archive> archive_clone = data->clone();

  Reference::To<Integration> clone = archive_clone->get_Integration(0);

  FrequencyIntegrate fscr;
  FrequencyIntegrate::EvenlyWeighted policy;

  policy.set_ndivide (2);
  fscr.set_range_policy ( &policy );

  fscr.transform (clone);

#if HAVE_PGPLOT

  cerr << "Pulsar::DeltaRM::refine plotting half-band integrated profiles"
       << endl;

  plot (archive_clone, 0);
  plot (archive_clone, 1);

#endif

  /*
    remove the remaining Faraday rotation, so that residual delta_PA
    to be measured is (hopefully) less than 2pi 
  */
  clone->defaraday ();

  if (clone->get_nchan() != 2)
    throw Error (InvalidState, "Pulsar::DeltaRM::refine",
		 "nchan != 2 after FrequencyIntegrate");

  double f0 = clone->get_Profile(0,0)->get_centre_frequency();
  double f1 = clone->get_Profile(0,1)->get_centre_frequency();

  // speed of light in m/s
  double speed_of_light = 299792458;

  double lambda_0 = speed_of_light / (f0 * 1e6);
  double lambda_1 = speed_of_light / (f1 * 1e6);

  cerr << "frequency_0 = " << f0 << " MHz"
    "  ->  lambda_0 = " << lambda_0 << " m" << endl
       << "frequency_1 = " << f1 << " MHz"
    "  ->  lambda_1 = " << lambda_1 << " m" << endl;

  Reference::To<PolnProfile> profile0 = clone->new_PolnProfile(0);
  Reference::To<PolnProfile> profile1 = clone->new_PolnProfile(1);

  Pulsar::DeltaPA delta_pa;

  delta_pa.set_threshold (threshold);
  delta_pa.set_include (include_bins);
  delta_pa.set_exclude (exclude_bins);

  Estimate<double> delta_PA = delta_pa.get (profile0, profile1);
  Estimate<double> delta_RM = delta_PA/(lambda_1*lambda_1-lambda_0*lambda_0);

  rotation_measure = delta_RM + data->get_rotation_measure();
  
  cerr << "delta PA = <PA_1 - PA_0> = " << 180.0/M_PI * delta_PA << " deg.\n"
       << "delta RM = " << delta_RM << endl
       << "final RM = " << rotation_measure << endl;

  used_bins = delta_pa.get_used_bins();
}

