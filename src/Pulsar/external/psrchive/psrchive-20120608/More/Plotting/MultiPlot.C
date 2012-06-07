/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiPlot.h"
#include "Pulsar/FramedPlot.h"
#include "Pulsar/MultiFrame.h"
#include "pairutil.h"

#include <cpgplot.h>

using namespace std;

Pulsar::MultiPlot::MultiPlot ()
{
}

TextInterface::Parser* Pulsar::MultiPlot::get_frame_interface ()
{
  return new MultiFrame::Interface (&frames);
}

Pulsar::PlotAttributes* Pulsar::MultiPlot::get_attributes ()
{
  return &frames;
}

void Pulsar::MultiPlot::plot (const Archive* data)
{
  if (verbose)
    cerr << "Pulsar::MultiPlot::plot call prepare" << endl;

  prepare (data);

  if (verbose)
    cerr << "Pulsar::MultiPlot::plot done prepare" << endl;
  
  std::map< std::string, Reference::To<FramedPlot> >::iterator ptr;
  for (ptr = plots.begin(); ptr != plots.end(); ptr++)
  {

    FramedPlot* plot = ptr->second;
    PlotFrame* frame = plot->get_frame();

    // set the viewport of the frame
    std::pair<float,float> xvp, yvp;
    set_viewport (frame, xvp, yvp);

    Error* to_throw = 0;

    try {
      // prepare the plot
      prepare (plot);

      // plot
      plot->plot(data);
    }
    catch (Error& error)
    {
      error += "Pulsar::MultiPlot::plot";
      error << " (in " << ptr->first << ")";
      to_throw = new Error (error);
    }

    // restore the viewport of the frame
    frame->get_x_scale(true)->set_viewport( xvp );
    frame->get_y_scale(true)->set_viewport( yvp );

    // this could cause a memory leak
    if (to_throw)
      throw *to_throw;

  }

  if (verbose)
    cerr << "Pulsar::MultiPlot::plot focus" << endl;
  get_frame()->focus (data);

  if (verbose)
    cerr << "Pulsar::MultiPlot::plot decorate frame" << endl;
  get_frame()->decorate (data);
}

void Pulsar::MultiPlot::set_viewport (PlotFrame* frame, 
				      std::pair<float,float>& sub_xvp,
				      std::pair<float,float>& sub_yvp)
{
  // get the total viewport allocated to the multiple plots
  std::pair<float,float> xvp = frames.get_x_edge()->get_viewport();
  std::pair<float,float> yvp = frames.get_y_edge()->get_viewport();

#if _DEBUG
  cerr << "Pulsar::MultiPlot::set_viewport total"
    " x=" << xvp << " y=" << yvp << endl;
#endif

  // get the fraction allocated to this sub-plot
  sub_xvp = frame->get_x_scale(true)->get_viewport();
  sub_yvp = frame->get_y_scale(true)->get_viewport();

#if _DEBUG
  cerr << "Pulsar::MultiPlot::set_viewport sub"
    " x=" << sub_xvp << " y=" << sub_yvp << endl;
#endif

  // calculate the total viewport allocated to this sub-plot
  stretch (sub_xvp, xvp);
  stretch (sub_yvp, yvp);

  // set the viewport accordingly
  frame->get_x_scale(true)->set_viewport( xvp );
  frame->get_y_scale(true)->set_viewport( yvp );
}

//! Manage a plot
void Pulsar::MultiPlot::manage (const std::string& name, FramedPlot* plot)
{
  if (verbose)
    cerr << "Pulsar::MultiPlot::manage name=" << name << endl;

  frames.manage (name, plot->get_frame());

  plot->get_frame()->get_label_above()->set_all( PlotLabel::unset );

  if (frames.has_shared_x_scale())
  {
    if (verbose)
      cerr << "Pulsar::MultiPlot::manage shared x_scale" << endl;
    plot->get_frame()->set_x_scale( frames.get_shared_x_scale() );
  }

  if (frames.has_shared_y_scale())
  {
    if (verbose)
      cerr << "Pulsar::MultiPlot::manage shared y_scale" << endl;
    plot->get_frame()->set_y_scale( frames.get_shared_y_scale() );
  }

  plots[name] = plot;
}

void Pulsar::MultiPlot::unmanage (FramedPlot* plot)
{
  std::map< std::string, Reference::To<FramedPlot> >::iterator ptr;
  for (ptr = plots.begin(); ptr != plots.end(); ptr++)
    if (ptr->second.ptr() == plot) {
      plots.erase (ptr);
      return;
    }
}
