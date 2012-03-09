//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicCalSpectrumPlot_h
#define __Pulsar_DynamicCalSpectrumPlot_h

#include "Pulsar/DynamicSpectrumPlot.h"

namespace Pulsar {

  //! Plots off-pulse mean or variance as a func of time and freq.
  class DynamicCalSpectrumPlot : public DynamicSpectrumPlot {

  public:

    //! Default constructor
    DynamicCalSpectrumPlot ();

    //! Default constructor
    ~DynamicCalSpectrumPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<DynamicCalSpectrumPlot> {
    public:
      Interface (DynamicCalSpectrumPlot* = 0);
    };

    void get_plot_array(const Archive *data, float *array);

  };

}

#endif
