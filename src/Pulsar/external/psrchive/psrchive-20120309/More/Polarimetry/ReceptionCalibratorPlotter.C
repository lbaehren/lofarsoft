/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionCalibratorPlotter.h"
#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/Archive.h"

#include "Pulsar/SourceInfo.h"

#include "MEAL/Polynomial.h"
#include "MEAL/Steps.h"
#include "templates.h"

#include <cpgplot.h>

using namespace std;
using Pulsar::ReceptionCalibratorPlotter;

ReceptionCalibratorPlotter::ReceptionCalibratorPlotter (ReceptionCalibrator* c)
  : SystemCalibratorPlotter (c)
{
  calibrator = c;
}

ReceptionCalibratorPlotter::~ReceptionCalibratorPlotter ()
{
}

//! PGPLOT the flux calibrator model parameters as a function of frequency
void Pulsar::ReceptionCalibratorPlotter::plot_fluxcal ()
{
  if (!calibrator)
    throw Error (InvalidState,
		 "Pulsar::ReceptionCalibratorPlotter::plotcal",
                 "ReceptionCalibrator not set");

  if (verbose)
    cerr << "Pulsar::ReceptionCalibratorPlotter::plot_fluxcal call plot"
	 << endl;

  Reference::To<SourceInfo> info 
    = new SourceInfo( calibrator->flux_calibrator_estimate );

  info->set_together (true);
  info->set_label ("FluxCal Stokes");

  plot( info, calibrator->get_nchan(),
	calibrator->get_Archive()->get_centre_frequency(),
	calibrator->get_Archive()->get_bandwidth() );
}


//! Return the calibrator to be plotted
const Pulsar::ReceptionCalibrator* 
Pulsar::ReceptionCalibratorPlotter::get_calibrator () const
{
  return calibrator;
}

