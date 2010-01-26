/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/psrgui/Pulsar/PlotWindow.h,v $
   $Revision: 1.2 $
   $Date: 2008/07/16 02:27:53 $
   $Author: straten $ */

#ifndef __Pulsar_PlotWindow_h
#define __Pulsar_PlotWindow_h

#include "Reference.h"

namespace Pulsar {
  
  class Archive;
  class Plot;

  //! Manages a window in which to Plot an Archive
  class PlotWindow : public Reference::Able
  {
  public:

    PlotWindow ();
    ~PlotWindow ();

    //! Set the plot to be used
    virtual void set_plot (Plot*);

    //! Set the data to be plotted
    virtual void set_data (Archive*);

    //! Get the plot
    Plot* get_plot ();

    //! Get the data
    Archive* get_data ();

    //! Ready to plot data
    bool ready () const;

    //! Plot the data
    virtual void plot_data ();

  protected:
    
    Reference::To<Pulsar::Plot> plot;
    Reference::To<Pulsar::Archive> archive;
  };

}

#endif

