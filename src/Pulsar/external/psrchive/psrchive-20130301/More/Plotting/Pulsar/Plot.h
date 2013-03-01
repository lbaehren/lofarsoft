//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Plot.h,v $
   $Revision: 1.19 $
   $Date: 2010/09/19 04:33:37 $
   $Author: straten $ */

#ifndef __Pulsar_Plot_h
#define __Pulsar_Plot_h

#include "Pulsar/PlotAttributes.h"
#include "TextInterface.h"

namespace Pulsar {

  class Archive;

  //! Base class of all plotters
  class Plot : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    Plot () { constructor = 0; }

    //! Process the Archive as needed before calling plot
    virtual void preprocess (Archive*);

    //! Plot in the current viewport
    virtual void plot (const Archive*) = 0;

    //! Some plots may postpone plotting
    virtual void finalize () { }

    //! Provide access to the plot attributes
    virtual PlotAttributes* get_attributes () = 0;

    //! Get the text interface to the configuration attributes
    virtual TextInterface::Parser* get_interface () = 0;

    //! Get the text interface to the frame attributes
    virtual TextInterface::Parser* get_frame_interface () = 0;

    //! Process a configuration command
    virtual void configure (const std::string&);

    //! Creates a default constructed instance of a derived type
    class Constructor;

    //! Set the constructor
    void set_constructor (Constructor* c) { constructor = c; }

    //! Get the constructor
    Constructor* get_constructor ();

  private:

    // used by the configure method
    Reference::To<TextInterface::Parser> tui;
    Reference::To<TextInterface::Parser> fui;

    //! The constructor for the derived type of this instance
    Constructor* constructor;
  };

  /*!  
    This class provides a dynamic clone-like functionality without
    having to go through the entire inheritance hierarchy.
    PlotFactory installs the Constructor for the derived type.
  */
  class Plot::Constructor
  {
  public:

    virtual ~Constructor () { }

    //! Return a new default constructed instance of a derived type
    virtual Plot* construct () = 0;
  };

}

#endif
