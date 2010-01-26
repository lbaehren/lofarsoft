//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SimPolnTiming.h,v $
   $Revision: 1.7 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_SimPolnTiming_h
#define __Pulsar_SimPolnTiming_h

#include "Pulsar/SyntheticPolnProfile.h"
#include "Pulsar/PolnProfileFit.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/Gaussian.h"

#include <iostream>

namespace Pulsar {

  class Archive;

  class Range {
  public:
    //! Constructor
    Range (unsigned steps, double min, double max);

    unsigned steps;
    double min, max;
    unsigned start, stop;

    //! Get the current value in the range
    double get_value (unsigned current);

    //! Parse the range from a string
    void parse (const std::string& txt);
  };

  std::ostream& operator<< (std::ostream& ostr, const Range& r);

  //! Simulates full-Stokes arrival time estimation
  class SimPolnTiming {

  public:

    //! Default constructor
    SimPolnTiming ();

    //! Destructor
    ~SimPolnTiming ();

    //! The synthetic polarimetric profile generator
    SyntheticPolnProfile generate;

    //! The arrival time estimation algorithm
    PolnProfileFit fit;

    //! The gaussian total intensity profile
    MEAL::Gaussian gaussian;

    //! The rotating vector model used to describe the position angle
    MEAL::RotatingVectorModel rvm;

    //! Number of bins in pulse profile
    unsigned nbin;

    //! Set the phase of the standard profile centre
    void set_centre (double);

    //! Relative noise in standard profile
    float standard_noise;

    //! Difference between magnetic axis and line of sight
    float rvm_beta;

    //! Set the archive to be used as a standard
    void set_standard (Archive*);

    //! Number of simulated observations for each step in simulation
    unsigned iterations;

    //! Relative noise in simulated observations
    float measurement_noise;

    //! Phase offset of simulated observations
    float offset;

    //! Vary the line of sight
    bool vary_line_of_sight;

    //! Number of steps when varying the RVM orientation
    Range slope;
    double poln_slope;

    //! Number of steps when varying the Gaussian width
    Range width;
    double pulse_width;

    //! Number of steps when varying the differental gain
    Range dgain;
    double diff_gain;

    //! Number of steps when varying the parallactic angle
    Range parallactic;
    double parallactic_angle;

    //! Number of steps when varying the parallactic angle
    Range boost_vector;
    double B_orientation;
    double B_ellipticity;
    double beta;

    //! Run the simulation, outputting results to cout
    void run_simulation ();

    //! Title written over plot
    std::string title;

    //! Output values using Estimate operator <<
    bool as_Estimate;

    //! Output stream
    std::ostream* output;

    bool logarithmic;
    void extrapolate_benefit (unsigned steps);
    void susceptibility ();

    //! Find the phase shift due to boosts of varying orientation
    void boost_around ();

  protected:

    class Result {
    public:
      Estimate<double> I_est_mean;
      Estimate<double> S_est_mean;
      Estimate<double> I_mean;
      Estimate<double> S_mean;
      Estimate<double> I_err;
      Estimate<double> S_err;
    };

    //! One loop in the simulation
    void one_loop (unsigned level);

    //! One step in the simulation
    Result one_step ();

    //! The data to use as a standard
    Reference::To<Pulsar::Archive> archive;

  };

}

#endif
