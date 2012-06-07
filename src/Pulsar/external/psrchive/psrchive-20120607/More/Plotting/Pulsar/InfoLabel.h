//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/InfoLabel.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/07 13:50:19 $
   $Author: straten $ */

#ifndef __Pulsar_InfoLabel_h
#define __Pulsar_InfoLabel_h

#include "Pulsar/PlotLabel.h"

namespace Pulsar {

  class FluxPlot;

  //! Print a label with three rows above the plot frame
  /*! This class can be used in conjunction with the FluxPlot class to
    emulate the behaviour of the old Plotter::singleProfile method */
  class InfoLabel : public PlotLabel {

  public:

    //! Default constructor
    InfoLabel (FluxPlot* flux);

    //! Destructor
    ~InfoLabel ();

    //! Plot the label
    virtual void plot (const Archive*);

  protected:

    // The FluxPlot instance for which this is plotting a label
    Reference::To<const FluxPlot> flux;

  };

}

#endif
