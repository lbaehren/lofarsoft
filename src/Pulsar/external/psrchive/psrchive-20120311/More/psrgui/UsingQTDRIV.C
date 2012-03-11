/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/UsingQTDRIV.h"

Pulsar::UsingQTDRIV::UsingQTDRIV ( QWidget *parent, const char *name )
  : QPgplot (parent, name)
{
}

void Pulsar::UsingQTDRIV::plotter ()
{
  PlotWindow::plot_data();
}

void Pulsar::UsingQTDRIV::plot_data ()
{
  QPgplot::clearScreen();
  // drawPlot sets things up then calls plotter
  QPgplot::drawPlot();
}


