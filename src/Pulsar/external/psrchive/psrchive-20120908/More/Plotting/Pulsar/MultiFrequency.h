//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiFrequency.h,v $
   $Revision: 1.14 $
   $Date: 2007/10/02 05:08:15 $
   $Author: straten $ */

#ifndef __Pulsar_MultiFrequency_h
#define __Pulsar_MultiFrequency_h

#include "Pulsar/MultiPlot.h"
#include "Pulsar/FrequencyScale.h"

namespace Pulsar {

  //! Plots multiple viewports with pulse phase along the shared x-axis
  class MultiFrequency : public MultiPlot {

  public:

    //! Default constructor
    MultiFrequency ();

    // Text interface to the MultiFrequency class
    class Interface : public TextInterface::To<MultiFrequency> {
    public:
      Interface (MultiFrequency* = 0);
    };

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    //! Get the scale
    FrequencyScale* get_scale ();

  };

}

#endif
