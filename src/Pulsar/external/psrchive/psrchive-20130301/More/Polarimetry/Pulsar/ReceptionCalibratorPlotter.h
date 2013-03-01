//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibratorPlotter.h,v $
   $Revision: 1.4 $
   $Date: 2009/08/14 01:53:39 $
   $Author: straten $ */

#ifndef __ReceptionCalibratorPlotter_H
#define __ReceptionCalibratorPlotter_H

#include "Pulsar/SystemCalibratorPlotter.h"

namespace Pulsar
{
  class ReceptionCalibrator;

  //! Plots the various components of a ReceptionCalibrator
  class ReceptionCalibratorPlotter : public SystemCalibratorPlotter
  {
    
  public:

    ReceptionCalibratorPlotter (ReceptionCalibrator* calibrator);

    ~ReceptionCalibratorPlotter ();

    //! Plot the flux calibrator as a function of frequency
    void plot_fluxcal ();

    //! Return the calibrator to be plotted
    const ReceptionCalibrator* get_calibrator () const;

  protected:

    //! Plot the observed Stokes parameters versus parallactic angle
    void plot_constraints (unsigned ichan, unsigned istate);

    //! Plot the given time variation function
    void plot_time_variation (VariationInfo::Which, const MEAL::Scalar*);

    //! Reference stored on init
    Reference::To<ReceptionCalibrator> calibrator;

    //! Plotting methods will plot the residual
    bool plot_residual;

    //! Plotting methods will output data in text files, pcm_state*.dat
    bool output_state;
  };

}

#endif
