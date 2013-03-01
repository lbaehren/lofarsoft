//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiDataSimple.h,v $
   $Revision: 1.1 $
   $Date: 2010/09/19 04:33:19 $
   $Author: straten $ */

#ifndef __Pulsar_MultiDataSimple_h
#define __Pulsar_MultiDataSimple_h

#include "Pulsar/MultiData.h"
#include "Pulsar/SimplePlot.h"

namespace Pulsar {

  //! Combines multiple data sets in a single plot
  class MultiDataSimple : public MultiData {

  public:

    //! Construct with plot to be managed
    MultiDataSimple (SimplePlot* simple = 0);

    //! Manage a plot
    void manage (SimplePlot*);

    //! prepare the Simple Plot and push it onto the plots vector
    void plot (const Archive*);

    //! draw the accumulated plots
    void finalize ();

  protected:

    Reference::To<SimplePlot> simple_plot;

    std::vector< Reference::To<SimplePlot> > plots;
    std::vector< Reference::To<const Archive> > data_sets;

  };

}

#endif
