//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/CalibratorSpectrum.h,v $
   $Revision: 1.7 $
   $Date: 2010/07/15 10:14:48 $
   $Author: straten $ */

#ifndef __Pulsar_CalibratorSpectrum_h
#define __Pulsar_CalibratorSpectrum_h

#include "Pulsar/FrequencyPlot.h"
#include "Pulsar/Index.h"
#include "EstimatePlotter.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class CalibratorSpectrum : public FrequencyPlot {

  public:

    //! Default constructor
    CalibratorSpectrum ();

    //! Return the text interface
    TextInterface::Parser* get_interface ();

    // Text interface to the CalibratorSpectrum class
    class Interface : public TextInterface::To<CalibratorSpectrum> {
    public:
      Interface (CalibratorSpectrum* = 0);
    };

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }

    //! Plot the total and polarized intensities
    void set_plot_Ip (bool flag = true) { plot_Ip = flag; }
    bool get_plot_Ip () const { return plot_Ip; }

    //! Plot the sum of on- and off-pulse polarization
    void set_plot_total (bool flag = true) { plot_total = flag; }
    bool get_plot_total () const { return plot_total; }

    //! Plot the off-pulse polarization
    void set_plot_low (bool flag = true) { plot_low = flag; }
    bool get_plot_low () const { return plot_low; }

    //! Normalize the Stokes paramters by the invariance
    void set_norm_inv (bool flag = true) { norm_inv = flag; }
    bool get_norm_inv () const { return norm_inv; }

    //! Prepare to plot
    void prepare (const Archive*);

    //! Draw the spectra
    void draw (const Archive*);

  protected:

    Index isubint;
    bool plot_total;
    bool plot_low;
    bool plot_Ip;
    bool norm_inv;

    EstimatePlotter plotter;

  };

}

#endif
