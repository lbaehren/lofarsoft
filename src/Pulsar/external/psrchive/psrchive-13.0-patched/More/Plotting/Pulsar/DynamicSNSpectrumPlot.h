//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicSNSpectrumPlot_h
#define __Pulsar_DynamicSNSpectrumPlot_h

#include "Pulsar/DynamicSpectrumPlot.h"

namespace Pulsar {

  //! Plot pulsar signal-to-noise ratio as a func of time and freq.
  class DynamicSNSpectrumPlot : public DynamicSpectrumPlot {

  public:

    //! Default constructor
    DynamicSNSpectrumPlot ();

    //! Default constructor
    ~DynamicSNSpectrumPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<DynamicSNSpectrumPlot> {
    public:
      Interface (DynamicSNSpectrumPlot* = 0);
    };

    void get_plot_array(const Archive *data, float *array);

  };

}

#endif
