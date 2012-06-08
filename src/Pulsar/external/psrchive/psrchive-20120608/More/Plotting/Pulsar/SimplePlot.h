//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/SimplePlot.h,v $
   $Revision: 1.5 $
   $Date: 2007/08/20 06:35:22 $
   $Author: straten $ */

#ifndef __Pulsar_SimplePlot_h
#define __Pulsar_SimplePlot_h

#include "Pulsar/FramedPlot.h"

namespace Pulsar {

  //! Plots a single plot
  /*! Children of SimplePlot always:
    <UL>
    <LI> plot in the currently open viewport
    <LI> use the currently set line width
    <LI> use the currently set character height
    </UL>
    That is, children of the SimplePlot class do not call
    cpgsvp, cpgslw, or cpgsch.  Children of SimplePlot class may:
    <UL>
    <LI> change the colour used within the plot frame
    <LI> change the line style within the plot frame
    </UL>
  */
  class SimplePlot : public FramedPlot {

  public:

    //! Plot in the current viewport
    virtual void plot (const Archive*);

    //! Get the default label for the x axis
    virtual std::string get_xlabel (const Archive*) = 0;

    //! Get the default label for the y axis
    virtual std::string get_ylabel (const Archive*) = 0;

    //! Derived classes must prepare to draw
    virtual void prepare (const Archive*) = 0;

    //! Derived classes must draw in the current viewport
    virtual void draw (const Archive*) = 0;

  };

}

#endif
