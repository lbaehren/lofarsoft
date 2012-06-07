//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SystemCalibratorPlotter.h,v $
   $Revision: 1.4 $
   $Date: 2009/08/14 01:53:39 $
   $Author: straten $ */

#ifndef __SystemCalibratorPlotter_H
#define __SystemCalibratorPlotter_H

#include "Pulsar/CalibratorPlotter.h"
#include "Pulsar/VariationInfo.h"
#include "MEAL/Scalar.h"

#include "Estimate.h"

class EstimatePlotter;

namespace Pulsar
{
  class SystemCalibrator;

  //! Plots the various components of a SystemCalibrator
  class SystemCalibratorPlotter : public CalibratorPlotter
  {
    
  public:

    SystemCalibratorPlotter (SystemCalibrator* calibrator);

    ~SystemCalibratorPlotter ();

    //! Plot the calibrator as a function of frequency
    void plot_cal ();

    //! Plot the Stokes parameters versus parallactic angle
    void plot_cal_constraints (unsigned ichan);

    //! Plot the observed Stokes parameters versus parallactic angle
    void plot_psr_constraints (unsigned ichan, unsigned istate);

    //! Plot vertical lines over the phases used as pulsar constraints
    void plot_phase_constraints ();

    //! Plot the residual in plot_psr_constraints
    void set_plot_residual (bool val) { plot_residual = val; }

    //! Plot all time variation functions
    void plot_time_variations ();

    //! Return the calibrator to be plotted
    const SystemCalibrator* get_calibrator () const;

  protected:

    //! Plot the observed Stokes parameters versus parallactic angle
    void plot_constraints (unsigned ichan, unsigned istate);

    //! Plot the given time variation function
    void plot_time_variation (VariationInfo::Which, const MEAL::Scalar*);

    //! Reference stored on init
    Reference::To<SystemCalibrator> calibrator;

    //! Plotting methods will plot the residual
    bool plot_residual;

    //! Plotting methods will output data in text files, pcm_state*.dat
    bool output_state;
  };

}

#endif
