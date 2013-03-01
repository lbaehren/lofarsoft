//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotEdge.h,v $
   $Revision: 1.1 $
   $Date: 2006/12/02 04:50:55 $
   $Author: straten $ */

#ifndef __Pulsar_PlotEdge_h
#define __Pulsar_PlotEdge_h

#include "TextInterface.h"

#include <utility>

namespace Pulsar {

  //! The edge of a plot viewport
  class PlotEdge : public Reference::Able {

  public:

    PlotEdge () : viewport (0.15, 0.85) { }

    // Text interface to the PlotEdge class
    class Interface : public TextInterface::To<PlotEdge> {
    public:
      Interface (PlotEdge* = 0);
    };

    //! Set the viewport (normalized device coordinates)
    void set_viewport (const std::pair<float,float>& f) { viewport = f; }

    //! Get the viewport (normalized device coordinates)
    std::pair<float,float> get_viewport () const { return viewport; }

  protected:

    std::pair<float,float> viewport;

  };

  
}

#endif
