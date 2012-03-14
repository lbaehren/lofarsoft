//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotAttributes.h,v $
   $Revision: 1.8 $
   $Date: 2008/01/14 04:55:49 $
   $Author: straten $ */

#ifndef __Pulsar_PlotAttributes_h
#define __Pulsar_PlotAttributes_h

#include "Pulsar/PlotLabel.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Stores the properties of the plot frame
  class PlotAttributes : public Reference::Able {

  public:

    //! Default constructor
    PlotAttributes ();

    //! Destructor
    ~PlotAttributes ();

    // Text interface to the PlotAttributes class
    class Interface : public TextInterface::To<PlotAttributes> {
    public:
      Interface (PlotAttributes* = 0);
    };

    //! Set the character height
    virtual void set_character_height (float h);
    float get_character_height () const { return character_height; }

    //! Set the character font
    virtual void set_character_font (int font);
    int get_character_font () const { return character_font; }

    //! Set the line width
    virtual void set_line_width (int width);
    int get_line_width () const { return line_width; }

    //! Get the above-frame label
    PlotLabel* get_label_above () { return above; }
    //! Set the above-frame label
    void set_label_above (PlotLabel*);

    //! Apply the named customization option set
    virtual void apply_set (const std::string& name);
    //! Get the applied set
    std::string get_applied () const { return set_applied; }

    //! Set publication quality character height and font and line width
    virtual void publication_quality ();

    //! Remove all plot labels
    virtual void no_labels ();

    //! Set the plot surface size, if necessary
    void initialize_surface ();

  protected:

    Reference::To<PlotLabel> above;

    float character_height;
    int character_font;
    int line_width;

    std::string set_applied;

  };

}

#endif
