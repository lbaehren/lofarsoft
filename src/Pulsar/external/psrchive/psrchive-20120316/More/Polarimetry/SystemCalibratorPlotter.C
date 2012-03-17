/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SystemCalibratorPlotter.h"
#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/Archive.h"

#include "Pulsar/ReceptionModelAxisPlotter.h"
#include "Pulsar/PolynomialInfo.h"
#include "Pulsar/StepsInfo.h"
#include "Pulsar/SourceInfo.h"

#include "MEAL/Polynomial.h"
#include "MEAL/Steps.h"
#include "templates.h"

#include <cpgplot.h>

using namespace std;

Pulsar::SystemCalibratorPlotter::SystemCalibratorPlotter (SystemCalibrator* c)
{
  calibrator = c;
  plot_residual = false;
  output_state = false;
}

Pulsar::SystemCalibratorPlotter::~SystemCalibratorPlotter ()
{
}

void Pulsar::SystemCalibratorPlotter::plot_cal_constraints (unsigned chan)
{
  if (calibrator->calibrator_estimate.size() == 0)
    throw Error (InvalidState,
		 "Pulsar::SystemCalibratorPlotter::plot_cal_constraints",
		 "no calibrator data");

  plot_constraints (chan, calibrator->calibrator_estimate[0].input_index);
}

void Pulsar::SystemCalibratorPlotter::plot_psr_constraints (unsigned chan,
							    unsigned state)
{
  plot_constraints (chan, state);
}


#include "MEAL/Axis.h"
#include "MJD.h"

class MJDAbscissa : public Calibration::ReceptionModelPlotter::Abscissa
{
public:

  MJDAbscissa (MEAL::Axis<MJD>* time) { epoch = time; }

  void push_back () { mjd.push_back ( epoch->get_value() ); }

  void get_values (std::vector<double>& values) const
  {
    values.resize( mjd.size() );
    MJD min, max;
    minmax (mjd, min, max);
    MJD mid = (min + max) * 0.5;

    for (unsigned ipt=0; ipt < mjd.size(); ipt++)
      values[ipt] = (mjd[ipt] - mid).in_minutes() / 60.0;
  }

  std::string get_label () const { return "Epoch (hours)"; }

protected:
  Reference::To< MEAL::Axis<MJD> > epoch;
  std::vector<MJD> mjd;
};


void Pulsar::SystemCalibratorPlotter::plot_constraints (unsigned ichan,
							unsigned istate)
{
  if (!calibrator)
    throw Error (InvalidState,
		 "Pulsar::SystemCalibratorPlotter::plot_constraints",
                 "SystemCalibrator not set");

  if (istate >= calibrator->get_nstate())
    throw Error (InvalidRange,
		 "Pulsar::SystemCalibratorPlotter::plot_constraints",
		 "istate=%d >= nstate=%d", istate, calibrator->get_nstate());

  if (ichan >= calibrator->get_nchan())
    throw Error (InvalidRange,
		 "Pulsar::SystemCalibratorPlotter::plot_constraints",
		 "ichan=%d >= nchan=%d", ichan, calibrator->get_nchan());

  Calibration::ReceptionModelAxisPlotter<MJD> plotter;

  calibrator->model[ichan]->engage_time_variations();

  plotter.set_model( calibrator->model[ichan]->get_equation() );
  plotter.set_model_solved( calibrator->get_solved() );
  plotter.set_plot_residual( plot_residual );

  plotter.set_abscissa( new MJDAbscissa( &(calibrator->model[ichan]->time) ) );

  plotter.set_isource (istate);

  plotter.set_axis( &(calibrator->model[ichan]->time) );
  plotter.set_min ( calibrator->start_epoch );
  plotter.set_max ( calibrator->end_epoch );
  plotter.set_npt ( 100 );

  if (output_state && !calibrator->get_solved())
  {
    char buffer[256];
    sprintf (buffer, "pcm_state%d_chan%d.dat", istate, ichan);

    plotter.set_output (buffer);
  }

  plotter.plot_observations ();

}


//! PGPLOT the calibrator model parameters as a function of frequency
void Pulsar::SystemCalibratorPlotter::plot_cal ()
{
  if (!calibrator)
    throw Error (InvalidState,
		 "Pulsar::SystemCalibratorPlotter::plot_cal",
                 "SystemCalibrator not set");

  if (verbose)
    cerr << "Pulsar::SystemCalibratorPlotter::plot_cal call plot" << endl;

  Reference::To<SourceInfo> info 
    = new SourceInfo( calibrator->calibrator_estimate );

  info->set_together (true);
  info->set_label ("CAL Stokes");

  plot( info, calibrator->get_nchan(),
	calibrator->get_Archive()->get_centre_frequency(),
	calibrator->get_Archive()->get_bandwidth() );

}

void Pulsar::SystemCalibratorPlotter::plot_time_variations ()
{
  if (!calibrator)
    throw Error (InvalidState,
		 "Pulsar::SystemCalibratorPlotter::plot_time_variations",
                 "SystemCalibrator not set");

  if (calibrator->gain_variation)
    plot_time_variation (VariationInfo::Gain,
			 calibrator->gain_variation);

  if (calibrator->diff_gain_variation)
    plot_time_variation (VariationInfo::Boost,
			 calibrator->diff_gain_variation);

  if (calibrator->diff_phase_variation)
    plot_time_variation (VariationInfo::Rotation,
			 calibrator->diff_phase_variation);
}

void Pulsar::SystemCalibratorPlotter::plot_time_variation
(VariationInfo::Which which, const MEAL::Scalar* scalar)
{
  cpgpage();

  if ( dynamic_cast<const MEAL::Steps*> (scalar) )
  {
    if (verbose)
      cerr << "Pulsar::SystemCalibratorPlotter::plot_time_variations"
	   << " steps" << endl;

    plot( new StepsInfo(calibrator, which),
	  calibrator->get_nchan(),
	  calibrator->get_Archive()->get_centre_frequency(),
	  calibrator->get_Archive()->get_bandwidth() );

    return;
  }

  if( dynamic_cast<const MEAL::Polynomial*> (scalar) )
  {
    if (verbose)
      cerr << "Pulsar::SystemCalibratorPlotter::plot_time_variations"
	   << " polynomial" << endl;

    plot( new PolynomialInfo(calibrator, which),
	  calibrator->get_nchan(),
	  calibrator->get_Archive()->get_centre_frequency(),
	  calibrator->get_Archive()->get_bandwidth() );

    return;
  }

  cerr << "Pulsar::SystemCalibratorPlotter::plot_time_variations"
       << " unrecognized" << endl;
}

//! Return the calibrator to be plotted
const Pulsar::SystemCalibrator* 
Pulsar::SystemCalibratorPlotter::get_calibrator () const
{
  return calibrator;
}

