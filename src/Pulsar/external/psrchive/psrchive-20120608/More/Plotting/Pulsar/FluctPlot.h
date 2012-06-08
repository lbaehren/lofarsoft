//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FluctPlot.h,v $
   $Revision: 1.5 $
   $Date: 2008/11/12 07:45:10 $
   $Author: straten $ */

#ifndef __Pulsar_FluctPlot_h
#define __Pulsar_FluctPlot_h

#include "Pulsar/SimplePlot.h"
#include "Pulsar/ProfileVectorPlotter.h"
#include "Pulsar/Index.h"

namespace Pulsar {

  class Profile;

  //! Plots fluctuation power spectra
  class FluctPlot : public SimplePlot {

  public:

    //! Default constructor
    FluctPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the FluctPlot class
    class Interface : public TextInterface::To<FluctPlot> {
    public:
      Interface (FluctPlot* = 0);
    };

    //! Perform any preprocessing steps
    void prepare (const Archive*);

    //! Draw the profiles in the currently open window
    void draw (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Get the default label for the x axis
    std::string get_xlabel (const Archive*);

    //! Derived classes must compute the fluctuation power spectra
    virtual void get_profiles (const Archive*) = 0;

    //! Set the sub-integration to plot (where applicable)
    void set_subint (Index _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (Index _ichan) { ichan = _ichan; }
    Index get_chan () const { return ichan; }

    //! Set the polarization to plot
    void set_pol (Index _ipol) { ipol = _ipol; }
    Index get_pol () const { return ipol; }

  protected:

    ProfileVectorPlotter plotter;

    Index isubint;
    Index ichan;
    Index ipol;

  };

}

#endif
