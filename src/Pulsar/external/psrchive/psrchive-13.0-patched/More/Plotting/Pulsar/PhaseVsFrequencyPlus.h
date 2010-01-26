//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PhaseVsFrequencyPlus.h,v $
   $Revision: 1.11 $
   $Date: 2008/11/12 07:45:10 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseVsFrequencyPlus_h
#define __Pulsar_PhaseVsFrequencyPlus_h

#include "Pulsar/MultiPlot.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/SpectrumPlot.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots phase vs. frequency, plus integrated profile and spectrum
  class PhaseVsFrequencyPlus : public MultiPlot {

  public:

    //! Default constructor
    PhaseVsFrequencyPlus ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequencyPlus class
    class Interface : public TextInterface::To<PhaseVsFrequencyPlus> {
    public:
      Interface (PhaseVsFrequencyPlus* = 0);
    };

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    //! Set the polarization to plot
    void set_pol (const Index& _ipol) { ipol = _ipol; }
    Index get_pol () const { return ipol; }

    //! Plot the power spectral density
    void set_plot_psd (bool _plot);
    bool get_plot_psd () const;

    PhaseVsFrequency* get_freq () { return &freq; }
    ProfilePlot* get_flux () { return &flux; }
    SpectrumPlot* get_psd () { return &psd; }

    PhaseScale* get_scale ();

  protected:

    PhaseVsFrequency freq;
    ProfilePlot flux;
    SpectrumPlot psd;

    Index isubint;
    Index ipol;

    PhaseScale xaxis;
    FrequencyScale yaxis;

    bool plot_psd;

    template<class T> void prepare (T* plot)
    {
      plot->set_subint (isubint);
      plot->set_pol (ipol);
    }

  };

}

#endif
