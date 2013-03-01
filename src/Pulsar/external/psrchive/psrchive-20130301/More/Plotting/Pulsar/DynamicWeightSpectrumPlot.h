//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DynamicWeightSpectrumPlot_h
#define __Pulsar_DynamicWeightSpectrumPlot_h

#include "Pulsar/DynamicSpectrumPlot.h"

namespace Pulsar {

  //! Plot archive weights as a func of time and freq.
  class DynamicWeightSpectrumPlot : public DynamicSpectrumPlot {

  public:

    //! Default constructor
    DynamicWeightSpectrumPlot ();

    //! Default constructor
    ~DynamicWeightSpectrumPlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the PhaseVsFrequency class
    class Interface : public TextInterface::To<DynamicWeightSpectrumPlot> {
    public:
      Interface (DynamicWeightSpectrumPlot* = 0);
    };

    void get_plot_array(const Archive *data, float *array);

  };

}

#endif
