/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/OffPulseCalibrator.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/SingleAxis.h"
#include "tostring.h"
#include <stdio.h>
#include <assert.h>

using namespace std;

Pulsar::OffPulseCalibrator::OffPulseCalibrator (const Archive* archive) 
  : SingleAxisCalibrator (archive)
{
}

Pulsar::OffPulseCalibrator::~OffPulseCalibrator ()
{
}

void 
Pulsar::OffPulseCalibrator::extra (unsigned ichan,
				   const vector< Estimate<double> >& source,
				   const vector< Estimate<double> >& sky)
{
  if (!transformation[ichan])
    return;

  Calibration::SingleAxis* sa;
  sa = dynamic_cast<Calibration::SingleAxis*>(transformation[ichan].get());
  assert (sa);

  sa->set_diff_gain (0.0);
}

/*! The required transformation cannot be represented by a Jones matrix */
void Pulsar::OffPulseCalibrator::calibrate (Archive* archive)
{
  cerr << "Pulsar::OffPulseCalibrator::calibrate *******************" << endl;
  calibration_setup (archive);

  unsigned nsub  = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol  = archive->get_npol();

  archive->convert_state (Signal::Coherence);

  for (unsigned isub=0; isub < nsub; isub++) {

    Integration* integration = archive->get_Integration (isub);

    vector< vector< Estimate<double> > > baselines;
    vector< vector< double > > variances;
    integration->baseline_stats (&baselines, &variances);

#ifdef _DEBUG
    string filename = archive->get_filename() + ".scale." + tostring(isub);
    FILE* fptr = fopen (filename.c_str(), "w");
#endif

    double mean_correction = 0;
    unsigned mean_count = 0;

    for (unsigned ichan=0; ichan < nchan; ichan++) {

      bool problem = false;
      for (unsigned ipol=0; ipol < npol; ipol++) {
	if (baselines[ipol][ichan] == 0)
	  problem = true;
	if (variances[ipol][ichan] == 0)
	  problem = true;
      }

      if (problem)
	continue;

      Estimate<double> gain_ratio = baselines[0][ichan] / baselines[1][ichan];
      double correction = sqrt (gain_ratio.val);
      assert (correction != 0);

      double var_y = variances[1][ichan] * correction*correction;
      integration->get_Profile(1, ichan) -> scale (correction);

      correction = 1.0/correction;
      double var_x = variances[0][ichan] * correction*correction;
      integration->get_Profile(0, ichan) -> scale (correction);

#ifdef _DEBUG
      fprintf (fptr, "%u %lf", ichan, correction);
#endif

      double var_I = var_x + var_y;

      double mean_sigma_p
	= sqrt(variances[2][ichan]) + sqrt(variances[3][ichan]);

      correction = sqrt(var_I) / mean_sigma_p;

#ifdef _DEBUG
      fprintf (fptr, " %lf\n", correction);
#endif

      mean_correction += correction;
      mean_count ++;

    }

    mean_correction /= mean_count;
    cerr << "Pulsar::OffPulseCalibrator::calibrate RMS scale factor = "
	 << mean_correction << endl;

    for (unsigned ichan=0; ichan < nchan; ichan++) {

      integration->get_Profile(2, ichan) -> scale (mean_correction);
      integration->get_Profile(3, ichan) -> scale (mean_correction);

    }

#ifdef _DEBUG
    fclose (fptr);
#endif

  }

  SingleAxisCalibrator::calibrate (archive);

}
