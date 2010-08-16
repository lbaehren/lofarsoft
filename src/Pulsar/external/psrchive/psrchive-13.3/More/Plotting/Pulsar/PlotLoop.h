//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotLoop.h,v $
   $Revision: 1.2 $
   $Date: 2009/10/02 10:28:17 $
   $Author: straten $ */

#ifndef __Pulsar_PlotLoop_h
#define __Pulsar_PlotLoop_h

#include "TextIndex.h"
#include <stack>

namespace Pulsar {

  class Archive;
  class Plot;

  //! Base class of all plotters
  class PlotLoop : public Reference::Able {

  public:

    //! Default constructor
    PlotLoop ();

    //! Set the Plot to be executed
    void add_Plot (Plot*);

    //! Set the Archive to be plotted
    void set_Archive (Archive*);

    //! Set the overlay flag
    void set_overlay (bool);

    //! Get the overlay flag
    bool get_overlay () const;

    //! Set the preprocess flag
    void set_preprocess (bool);

    //! Get the preprocess flag
    bool get_preprocess () const;

    //! Add an index over which to loop
    void add_index (TextIndex*);

    // execute the plot for each index in the stack
    void plot ();

  protected:

    std::vector< Reference::To<Plot> > plots;

    std::vector< Reference::To<Archive> > archives;

    bool preprocess;
    bool overlay;

    std::stack< Reference::To<TextIndex> > index_stack;

    //! Worker function called recursively down stack
    void plot ( std::stack< Reference::To<TextIndex> >& indeces );

  };

}

#endif
