/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Distortion.h"

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/SingleAxisSolver.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/BackendFeed.h"
#include "Pulsar/BasisCorrection.h"

#include "Pulsar/Integration.h"

#include "Pauli.h"

using namespace std;
using Calibration::BackendFeed;

bool verbose = false;

void feed_only (MEAL::Complex2* xform)
{
  BackendFeed* feed = dynamic_cast<BackendFeed*> (xform);

  if (!feed)
    throw Error (InvalidState, "keep_only_feed",
		 "transformation is not a BackendFeed");

  feed->set_gain( 1.0 );
  feed->set_diff_gain( 0.0 );
  feed->set_diff_phase( 0.0 );
}

void Pulsar::Distortion::set_calibrator (Archive* archive)
{
  //! The PolnCalibrator will be used to transform the data
  Reference::To<PolnCalibrator> precalibrator;
  precalibrator = new Pulsar::PolnCalibrator (archive);

  //! The PolnCalibrator extension stores the frequency of each channel
  Reference::To<PolnCalibratorExtension> extension;
  extension = archive->get<PolnCalibratorExtension>();
  
  // The Stokes parameters of the input reference signal
  Reference::To<const CalibratorStokes> reference_input;
  reference_input = archive->get<CalibratorStokes>();

  // The SingleAxis model solver
  Reference::To<Calibration::SingleAxisSolver> solver;
  solver = new Calibration::SingleAxisSolver;

  // the correction to supplement the PolnCalibrator
  Reference::To<Calibration::SingleAxis> solution;
  solution = new Calibration::SingleAxis;

  // get the Receiver correction, if any
  const Receiver* receiver = precalibrator->get_Receiver();
  BasisCorrection correction;
  Jones<double> basis = correction (receiver);

  /* The following line provides a basis-independent representation of a
     reference source that illuminates both receptors equally and in phase. */
  Quaternion<double,Hermitian> ideal (1,0,1,0);
  Stokes< Estimate<double> > input_stokes = standard (ideal);

  const unsigned nchan = archive->get_nchan();

  for (unsigned ichan=0; ichan<nchan; ++ichan) try
  {
    if (!precalibrator->get_transformation_valid(ichan))
      continue;

    if (verbose > 2)
      cerr << "Pulsar::Distortion::set_calibrator ichan=" << ichan << endl;

    MEAL::Complex2* xform = precalibrator->get_transformation(ichan);
    feed_only (xform);

    // get the Stokes parameters of the reference source observation
    Stokes< Estimate<double> > output_stokes;
    output_stokes = reference_input->get_stokes (ichan);

    // get the precalibrator transformation
    Jones<double> response = xform->evaluate();
    if (verbose > 2)
      cerr << "HybridCalibrator response=" << response << endl;

    //cerr << "A=" << output_stokes << endl;

    // pass the reference Stokes parameters through the instrument
    output_stokes = transform (output_stokes, response*basis);

    //cerr << "B=" << output_stokes << endl;

    solver->set_input (input_stokes);
    solver->set_output (output_stokes);
    solver->solve (solution);

    double freq = extension->get_centre_frequency (ichan);

    cout << extension->get_epoch() << " " << ichan << " " << freq
	 << " " << solution->get_diff_gain().get_value() 
	 << " " << xform->get_param (3) //<< " " << xform->get_param_name (3)
	 << " " << xform->get_param (4) //<< " " << xform->get_param_name (4)
	 << endl;
  }
  catch (Error& error)
    {
    }

  cout << endl;

}

