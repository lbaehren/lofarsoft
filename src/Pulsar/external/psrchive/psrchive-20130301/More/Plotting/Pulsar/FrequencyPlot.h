//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FrequencyPlot.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_FrequencyPlot_h
#define __Pulsar_FrequencyPlot_h

#include "Pulsar/SimplePlot.h"
#include "Pulsar/FrequencyScale.h"

#include <vector>

namespace Pulsar {

  //! Simple plots with radio frequency along the x-axis
  class FrequencyPlot : public SimplePlot {

  public:

    //! Default constructor
    FrequencyPlot ();

    //! Get the default label for the x axis
    std::string get_xlabel (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Set the y-range
    void set_yrange (float min, float max);

    //! Get the scale
    FrequencyScale* get_scale ();

  };

}

#endif
