//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Simulation.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_Simulation_H
#define __Calibration_Simulation_H

#include "Pulsar/Instrument.h"
#include "Stokes.h"

namespace Calibration {

  class Simulation : public Reference::Able {

  public:

    //! Measureable quantities of interest
    typedef enum { StokesI,
		   StokesQ,
		   StokesU,
		   StokesV,
		   StokesP,
		   StokesL, 
		   Orientation,
		   Ellipticity 
    } Measurable;


    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    Simulation ();

    //! Return the model of the instrument
    Instrument* get_instrument ();

    //! Return the transformation of the simulated environment
    /*! The simulated environment may include the instrument or, after
      calling single_axis_calibrate, the instrument plus the incorrect
      calibration */
    Jones<double> get_response ();

    //! Set Stokes V of the reference signal
    void set_cal_V (double cal_V);

    //! Set Stokes U of the reference signal
    void set_cal_U (double cal_U);

    //! Set the input source state
    void set_input (Stokes<double>& stokes);

    //! Set the output source state
    void set_output (Stokes<double>& stokes);

    //! Get the distortion to this parameter
    double get_distortion (Measurable m);

    double get_distortion (Measurable m,
			   Stokes<double>& in, 
			   Stokes<double>& out);

    //! Calibrate the instrument using the single axis model
    void single_axis_calibrate ();

  protected:

    //! The model of the instrument
    Instrument instrument;

    //! Stokes U of the reference signal
    double cal_U;

    //! Stokes V of the reference signal
    double cal_V;

    //! The transformation of the simulated environment
    Jones<double> response;

    //! The inverse transformation of the simulated environment
    Jones<double> inverse;

    //! Set when the above value may have been changed
    bool response_changed;

    //! Set when set_input is called
    Stokes<double> input;

    //! Set when set_output is called
    Stokes<double> output;

    //! Set true when set_input is called
    bool input_set;

  };

}

#endif
