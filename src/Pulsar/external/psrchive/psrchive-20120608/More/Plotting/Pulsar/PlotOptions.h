//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotOptions.h,v $
   $Revision: 1.6 $
   $Date: 2009/09/02 02:54:31 $
   $Author: straten $ */

#ifndef __Pulsar_PlotOptions_h
#define __Pulsar_PlotOptions_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Plot command line options
  class PlotOptions : public Application::Options
  {
  public:

    //! Default constructor
    PlotOptions (bool open_device = true);

    //! Enable/disable pgplot device open and close
    void set_open_device (bool flag);

    //! Set the plot device
    void set_device (const std::string& dev) { plot_device = dev; }

    //! Set the plot dimensions in pixels
    void set_pixels (const std::string&);

    //! Set the number of panels into which the plot surface is divided
    void set_panels (const std::string&);

    //! Get the number of panels into which the plot surface is divided
    unsigned get_x_npanel () const { return x_npanel; }
    void set_x_npanel (unsigned n) { x_npanel = n; }

    unsigned get_y_npanel () const { return y_npanel; }
    void set_y_npanel (unsigned n) { y_npanel = n; }

    //! Open the plot device and configure it
    void setup ();

    //! Close the plot device
    void finalize ();

  private:

    //! Add options to the menu
    virtual void add_options (CommandLine::Menu&);

    std::string plot_device;

    // number of panel into which plot surface will be divided
    unsigned x_npanel, y_npanel;

    // width of plot surface in cm
    float surface_width;

    // aspect ratio (height/width)
    float aspect_ratio;

    // plot dimensions in pixels
    unsigned width_pixels, height_pixels;

    // open device
    bool open_device;
  };

}

#endif
