/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotWindow.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Plot.h"

using namespace std;

Pulsar::PlotWindow::PlotWindow ()
{
}

Pulsar::PlotWindow::~PlotWindow ()
{
}

//! Set the plot to be used
void Pulsar::PlotWindow::set_plot (Plot* _plot)
{
  plot = _plot;
}

//! Set the data to be plotted
void Pulsar::PlotWindow::set_data (Archive* _data)
{
  archive = _data;
}

//! Get the plot
Pulsar::Plot* Pulsar::PlotWindow::get_plot ()
{
  return plot;
}

//! Get the data
Pulsar::Archive* Pulsar::PlotWindow::get_data ()
{
  return archive;
}

bool Pulsar::PlotWindow::ready () const
{
  return plot && archive;
}

//! Plot the data
void Pulsar::PlotWindow::plot_data () try
{
  if (!plot || !archive)
    return;;

  plot->plot (archive);
}
catch (Error& error)
{
  cerr << "Pulsar::PlotWindow::plot_data error=" << error << endl;
}

