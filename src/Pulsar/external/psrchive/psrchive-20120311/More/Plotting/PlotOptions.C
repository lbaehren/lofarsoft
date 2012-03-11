/***************************************************************************
 *
 *   Copyright (C) 2008-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotOptions.h"
#include "pgutil.h"

#include <stdio.h>
#include <cpgplot.h>

using namespace std;

Pulsar::PlotOptions::PlotOptions (bool open)
{
  plot_device = "?";

  x_npanel = y_npanel = 1;

  surface_width = 0.0;

  aspect_ratio = 0.0;

  width_pixels = height_pixels = 0;

  open_device = open;
}

void Pulsar::PlotOptions::set_open_device (bool flag)
{
  open_device = flag;
}

void Pulsar::PlotOptions::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add ("\n" "Plot device options:");

  arg = menu.add (plot_device, 'D', "device");
  arg->set_help ("plot to the specified device");

  arg = menu.add (this, &PlotOptions::set_pixels, 'g', "WxH");
  arg->set_help ("plot dimensions in pixels, width times height");

  arg = menu.add (this, &PlotOptions::set_panels, 'N', "XxY");
  arg->set_help ("plot panels, width by height");

  arg = menu.add (aspect_ratio, 'r', "ratio");
  arg->set_help ("aspect ratio (height/width)");

  arg = menu.add (surface_width, 'w', "width");
  arg->set_help ("plot surface width (in centimetres)");

}

//! Set the plot dimensions in pixels
void Pulsar::PlotOptions::set_pixels (const std::string& arg)
{
  char c = 0;
  if (sscanf (arg.c_str(), "%u%c%u", &width_pixels, &c, &height_pixels) != 3)
    throw Error (InvalidParam, "Pulsar::PlotOptions::set_pixels",
		 "could not parse '" + arg + "' as plot dimensions");
}

//! Set the number of panels into which the plot surface is divided
void Pulsar::PlotOptions::set_panels (const std::string& arg)
{
  char c = 0;
  if (sscanf (arg.c_str(), "%u%c%u", &x_npanel, &c, &y_npanel ) != 3)
    throw Error (InvalidParam, "Pulsar::PlotOptions::set_pixels",
		 "could not parse '" + arg + "' as number of panels");
}

//! Open the graphics device and configure it
void Pulsar::PlotOptions::setup ()
{
  if (!open_device)
    return;

  // open the plot device
  if (cpgopen(plot_device.c_str()) < 0)
    throw Error (InvalidParam, "Pulsar::PlotOptions::setup",
		 "Could not open plot device");

  // set the size of the plot
  if (surface_width || aspect_ratio)
    pgplot::set_paper_size (surface_width, aspect_ratio);

  if (width_pixels && height_pixels)
    pgplot::set_dimensions (width_pixels, height_pixels);

  // prompt before plotting the next page
  cpgask(1);

  // break the surface up into multiple panels
  if (x_npanel > 1 || y_npanel > 1)
    cpgsubp (x_npanel, y_npanel);

  // set the viewport with room for titles
  cpgsvp (0.15,0.9, 0.15,0.9);
}

void Pulsar::PlotOptions::finalize ()
{
  if (open_device)
    cpgend();
}
