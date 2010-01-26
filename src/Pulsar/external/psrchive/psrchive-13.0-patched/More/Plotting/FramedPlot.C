/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FramedPlot.h"
#include "Pulsar/PlotFrame.h"

Pulsar::FramedPlot::FramedPlot ()
{
  frame = new PlotFrame;
}

//! Get the text interface to the frame attributes
TextInterface::Parser* Pulsar::FramedPlot::get_frame_interface ()
{
  return new PlotFrame::Interface (get_frame());
}

Pulsar::PlotAttributes* Pulsar::FramedPlot::get_attributes ()
{
  return frame;
}

//! Get the frame
Pulsar::PlotFrame* Pulsar::FramedPlot::get_frame ()
{
  return frame;
}

//! Get the frame
const Pulsar::PlotFrame* Pulsar::FramedPlot::get_frame () const
{
  return frame;
}

//! Set the frame
void Pulsar::FramedPlot::set_frame (PlotFrame* f)
{
  frame = f;
}
