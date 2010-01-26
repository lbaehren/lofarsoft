//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_PhaseVsHistPlus_h
#define __Pulsar_PhaseVsHistPlus_h

#include "Pulsar/MultiPlot.h"
#include "Pulsar/PhaseVsHist.h"
#include "Pulsar/StokesPlot.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots phase vs. frequency, plus integrated profile and spectrum
  class PhaseVsHistPlus : public MultiPlot {

  public:

    //! Default constructor
    PhaseVsHistPlus ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequencyPlus class
    class Interface : public TextInterface::To<PhaseVsHistPlus> {
    public:
      Interface (PhaseVsHistPlus* = 0);
    };

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const Index& _isubint) { isubint = _isubint; }
    Index get_subint () const { return isubint; }
    
    PhaseVsHist* get_hist () { return &hist; }
    StokesPlot* get_flux () { return &flux; }

    PhaseScale* get_scale ();

  protected:

    PhaseVsHist hist;
    StokesPlot flux;

    Index isubint;

    PhaseScale xaxis;
    //FrequencyScale yaxis;

#if 0 
    template<class T> void prepare (T* plot)
    {
      plot->set_subint (isubint);
    }
#endif

  };

}

#endif
