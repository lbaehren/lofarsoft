/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IonosphereCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"

#include "Pulsar/Telescope.h"
#include "Pulsar/AuxColdPlasma.h"

#include "Pulsar/IRIonosphere.h"

using namespace std;

//! Default constructor
Pulsar::IonosphereCalibrator::IonosphereCalibrator () {}

//! Destructor
Pulsar::IonosphereCalibrator::~IonosphereCalibrator () {}

void Pulsar::IonosphereCalibrator::calibrate (Archive* archive)
{
  if (!archive->get_nsubint()) {
    if (verbose > 2)
      cerr << "Pulsar::IonosphereCalibrator no data to correct" << endl;
    return;
  }

  AuxColdPlasma* history = archive->get<AuxColdPlasma>();
  if ( history && history->get_birefringence_corrected () )
  {
    if (verbose > 2)
      cerr << "Pulsar::IonosphereCalibrator already corrected with " 
	   << history->get_birefringence_model_name() << endl;
    return;
  }

  if (!archive->get_poln_calibrated() && Archive::verbose) 
    cerr << "Pulsar::IonosphereCalibrator " 
      "WARNING: correcting without calibrator" << endl;

  Telescope* telescope = archive->get<Telescope>();

  if (!telescope)
    throw Error (InvalidState, "Pulsar::IonosphereCalibrator",
		 "no Telescope extension available");

  // The Horizon coordinates
  Horizon horizon;

  horizon.set_observatory_latitude(  telescope->get_latitude().getRadians() );
  horizon.set_observatory_longitude( telescope->get_longitude().getRadians() );
  horizon.set_source_coordinates( archive->get_coordinates() );

  // The ionospheric model
  Calibration::IRIonosphere ionosphere;

  unsigned nsub = archive->get_nsubint();
  for (unsigned isub=0; isub < nsub; isub++) {

    Integration* integration = archive->get_Integration (isub);

    horizon.set_epoch( integration->get_epoch() );
    ionosphere.set_horizon (horizon);

    Jones<float> xform = inv( ionosphere.evaluate () );
      
    integration->expert()->transform (xform);

  }

  if ( history )
  {
    history->set_birefringence_corrected (true);
    history->set_birefringence_model_name ( "IRI2001 HJL" );
  }
}
