//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotAxis.h,v $
   $Revision: 1.12 $
   $Date: 2007/10/30 02:31:15 $
   $Author: straten $ */

#ifndef __Pulsar_PlotAxis_h
#define __Pulsar_PlotAxis_h

#include "TextInterface.h"

namespace Pulsar {

  //! Stores the label and cpgbox options for an axis
  class PlotAxis : public Reference::Able {

  public:

    //! Default constructor
    PlotAxis ();

    // Text interface to the PlotAxis class
    class Interface : public TextInterface::To<PlotAxis> {
    public:
      Interface (PlotAxis* = 0);
    };

    //! Set the label to be drawn on the axis
    void set_label (const std::string& _label) { label = _label; }
    //! Get the label to be drawn on the axis
    std::string get_label () const { return label; }

    //! Set the label displacement from edge of viewport
    /*! In units of the character height (see PGMTXT documentation) */
    void set_displacement (float d) { displacement = d; }
    //! Get the label displacement from edge of viewport
    float get_displacement () const { return displacement; }

    //! Set if the frame opposite the axis may have an alternate scale
    void set_alternate (bool f) { alternate = f; }
    //! Get the world-normalized minimum value on the axis
    bool get_alternate () const { return alternate; }

    //! Set the options to be passed to pgbox for this axis
    void set_opt (const std::string& o) { opt = o; }
    //! Get the label to be drawn on the axis
    std::string get_opt () const { return opt; }

    //! Set the tick to be passed to pgbox for this axis
    void set_tick (float f) { tick = f; }
    //! 
    float get_tick () const { return tick; }

    //! Set the nsub to be passed to pgbox for this axis
    void set_nsub (unsigned f) { nsub = f; }
    //! 
    unsigned get_nsub () const { return nsub; }

    //! Add to the options to be passed to pgbox for this axis
    void add_opt (char opt);

    //! Remove from the options to be passed to pgbox for this axis
    void rem_opt (char opt);

  protected:

    std::string label;
    float displacement;

    std::string opt;
    float tick;
    unsigned nsub;

    bool alternate;

  };

  
}

#endif
