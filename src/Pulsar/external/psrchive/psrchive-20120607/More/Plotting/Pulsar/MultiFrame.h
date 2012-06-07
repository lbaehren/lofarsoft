//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiFrame.h,v $
   $Revision: 1.17 $
   $Date: 2008/06/11 04:50:51 $
   $Author: straten $ */

#ifndef __Pulsar_MultiFrame_h
#define __Pulsar_MultiFrame_h

#include "Pulsar/PlotFrame.h"
#include <map>

namespace Pulsar {

  //! Manages multiple plot frames
  class MultiFrame : public PlotAttributes {

  public:

    // Text interface to the MultiFrame class
    class Interface : public TextInterface::To<MultiFrame> {
    public:
      Interface (MultiFrame* = 0);
    };

    //! Manage the given PlotFrame
    void manage (const std::string& name, PlotFrame*);

    //! Return a previously mapped plot frame
    PlotFrame* get_frame (const std::string& name);

    //! Return the x-edge
    PlotEdge* get_x_edge() { return &x_edge; }

    //! Return the y-edge
    PlotEdge* get_y_edge() { return &y_edge; }

    //! Return the shared x-scale
    PlotScale* get_shared_x_scale() { return x_scale; }
    bool has_shared_x_scale () { return x_scale; }
    void set_shared_x_scale (PlotScale* x) { x_scale = x; }

    //! Return the shared y-scale
    PlotScale* get_shared_y_scale() { return y_scale; }
    bool has_shared_y_scale () { return y_scale; }
    void set_shared_y_scale (PlotScale* y) { y_scale = y; }

    //! Set the character height
    void set_character_height (float height);

    //! Set the character font
    void set_character_font (int font);

    //! Set the line width
    void set_line_width (int width);
 
    //! Set no_labels in all sub-plots
    void no_labels ();

    //! Focus the frame
    virtual void focus (const Archive*);

    //! Decorate the frame
    virtual void decorate (const Archive*);

  protected:

    std::map< std::string, Reference::To<PlotFrame> > frames;

    PlotEdge x_edge;
    PlotEdge y_edge;

    Reference::To<PlotScale> x_scale;
    Reference::To<PlotScale> y_scale;

  };

}

#endif
