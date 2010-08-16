//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/SpectrumPlot.h,v $
   $Revision: 1.10 $
   $Date: 2008/11/12 07:45:10 $
   $Author: straten $ */

#ifndef __Pulsar_SpectrumPlot_h
#define __Pulsar_SpectrumPlot_h

#include "Pulsar/PowerSpectra.h"

namespace Pulsar {

  //! Plots a single spectrum
  class SpectrumPlot : public PowerSpectra {

  public:

    // Default constructor
    SpectrumPlot ();

    // Text interface to the SpectrumPlot class
    class Interface : public TextInterface::To<SpectrumPlot> {
    public:
      Interface (SpectrumPlot* = 0);
    };

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ()
    { return new Interface (this); }

    //! Load the spectra
    void get_spectra (const Archive* data);

    //! Set the phase bin to plot
    void set_bin (const Index& _ibin) { ibin = _ibin; }
    Index get_bin () const { return ibin; }

    //! Disable baseline removal
    void preprocess (Archive*);

  protected:

    Index ibin;

  };

}

#endif
