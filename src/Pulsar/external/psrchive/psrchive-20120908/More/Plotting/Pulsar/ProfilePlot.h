//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/ProfilePlot.h,v $
   $Revision: 1.13 $
   $Date: 2007/10/02 05:08:15 $
   $Author: straten $ */

#ifndef __Pulsar_ProfilePlot_h
#define __Pulsar_ProfilePlot_h

#include "Pulsar/FluxPlot.h"

namespace Pulsar {

  //! Plots a single pulse profile
  class ProfilePlot : public FluxPlot {

  public:

    //! Default constructor
    ProfilePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the ProfilePlot class
    class Interface : public TextInterface::To<ProfilePlot> {
    public:
      Interface (ProfilePlot* = 0);
    };

    //! Load the profiles
    void get_profiles (const Archive* data);

    //! Draw using FluxPlot::draw, then maybe draw_transitions
    void draw (const Archive*);

    //! Set if cal transitions will be drawn
    void set_plot_cal_transitions (bool flag) { plot_cal_transitions = flag; }
    bool get_plot_cal_transitions () const { return plot_cal_transitions; }

    //! Draw the profile transitions in the current viewport and window
    void draw_transitions (const Profile* profile);

  protected:

    bool plot_cal_transitions;

  };

}

#endif
