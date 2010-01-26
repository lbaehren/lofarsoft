//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PowerSpectra.h,v $
   $Revision: 1.10 $
   $Date: 2008/11/12 07:45:10 $
   $Author: straten $ */

#ifndef __Pulsar_PowerSpectra_h
#define __Pulsar_PowerSpectra_h

#include "Pulsar/FrequencyPlot.h"
#include "Pulsar/Index.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class PowerSpectra : public FrequencyPlot {

  public:

    //! Default constructor
    PowerSpectra ();

    //! Return the text interface
    TextInterface::Parser* get_interface ();

    // Text interface to the PowerSpectra class
    class Interface : public TextInterface::To<PowerSpectra> {
    public:
      Interface (PowerSpectra* = 0);
    };

    //! Perform any preprocessing steps
    void prepare (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the profiles
    virtual void get_spectra (const Archive*) = 0;

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the polarization to plot
    void set_pol (const Index& _ipol) { ipol = _ipol; }
    Index get_pol () const { return ipol; }

    //! Set the median filter window size
    void set_median_window (unsigned window) { median_window = window; }
    //! Get the median window size
    unsigned get_median_window () const { return median_window; }

    //! Set draw lines flag
    void set_draw_lines (bool flag) { draw_lines = flag; }
    //! Get draw lines flag
    bool get_draw_lines () const { return draw_lines; }

    //! Draw the spectra
    void draw (const Archive*);

    //! Draw the spectrum
    void draw (const std::vector<float>&) const;

  protected:

    std::vector< std::vector<float> > spectra;
    std::vector< int > plot_sci;
    std::vector< int > plot_sls;

    Index isubint;
    Index ipol;

    unsigned median_window;
    bool draw_lines;

  private:

    std::vector<float> frequencies;

  };

}

#endif
