//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesFluctPlot.h,v $
   $Revision: 1.8 $
   $Date: 2009/02/21 20:42:22 $
   $Author: straten $ */

#ifndef __Pulsar_StokesFluctPlot_h
#define __Pulsar_StokesFluctPlot_h

#include "Pulsar/FluctPlot.h"

namespace Pulsar {

  class Profile;

  //! Plots fluctuation power spectra
  class StokesFluctPlot : public FluctPlot {

  public:

    //! Default constructor
    StokesFluctPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the StokesFluctPlot class
    class Interface : public TextInterface::To<StokesFluctPlot> {
    public:
      Interface (StokesFluctPlot* = 0);
    };

    //! Perform any preprocessing steps
    void prepare (const Archive* data);

    //! Draw the profiles in the currently open window
    void draw (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Return the fluctuation power spectra to be plotted
    void get_profiles (const Archive*);

    //! Set the values to be plotted (one or more of I,Q,U,V,L,p)
    void set_plot_values (const std::string& plot) { plot_values = plot; }
    std::string get_plot_values () const { return plot_values; }

    //! Signal-to-noise ratio on the y-axis
    void set_signal_to_noise (const bool plot) { signal_to_noise = plot; }
    bool get_signal_to_noise () const { return signal_to_noise; }

    //! Set the colour for each value plotted
    void set_plot_colours (const std::string& plot) { plot_colours = plot; }
    std::string get_plot_colours () const { return plot_colours; }
 
    //! Set the line style for each value plotted
    void set_plot_lines (const std::string& plot) { plot_lines = plot; }
    std::string get_plot_lines () const { return plot_lines; }
 
    //! Plot the last significant harmonic
    void set_plot_last_harmonic (bool p) { plot_last_harmonic = p; }
    bool get_plot_last_harmonic () const { return plot_last_harmonic; }

  protected:
 
    bool signal_to_noise;
    std::string plot_values;
    std::string plot_colours;
    std::string plot_lines;

    bool plot_last_harmonic;
    unsigned last_harmonic;

  };

}

#endif
