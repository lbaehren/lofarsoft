//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineWindow.h,v $
   $Revision: 1.10 $
   $Date: 2009/06/24 05:02:23 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineWindow_h
#define __Pulsar_BaselineWindow_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  class Profile;
  class Smooth;

  //! Finds the phase window in which the smoothed Profile is an extremum
  class BaselineWindow : public BaselineEstimator {

  public:

    //! Default constructor
    BaselineWindow ();

    //! Set the smoothing function
    void set_smooth (Smooth*);

    //! Get the smoothing function
    Smooth* get_smooth ();

    //! Set to find the minimum mean
    void set_find_minimum ();
  
    //! Set to find the maximum mean
    void set_find_maximum ();

    //! Set to find the mean closest to the specified value
    void set_find_mean (float mean);

    //! Set the start and end bins of the search
    void set_range (int bin_start, int bin_end);

    //! Return the phase at which minimum or maximum mean is found
    float find_phase (const std::vector<float>& amps);

    //! Return the phase at which minimum or maximum mean is found
    float find_phase (unsigned nbin, const float* amps);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    BaselineWindow* clone () const;

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

    //! The smoothing function
    Reference::To<Smooth> smooth;

    //! Set true when algorithm finds max
    bool find_max;

    //! Set true when algorithm finds mean
    bool find_mean;

    //! Mean value to be found
    float mean;

    //! The first bin in the selected range
    int bin_start;

    //! The last bin in the selected range
    int bin_end;

    //! Set true when range is specified
    bool range_specified;

  };

}


#endif // !defined __Pulsar_BaselineWindow_h
