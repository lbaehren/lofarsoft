/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"
#include "pairutil.h"

Pulsar::PlotAxis::PlotAxis ()
{
  label = PlotLabel::unset;
  displacement = 2.5;
  opt = "BCNST";
  tick = 0.0;
  nsub = 0;
  alternate = false;
}

//! Add to the options to be passed to pgbox for this axis
void Pulsar::PlotAxis::add_opt (char o)
{
  std::string::size_type found = opt.find(o);
  if (found == std::string::npos)
    opt += o;
}

//! Remove from the options to be passed to pgbox for this axis
void Pulsar::PlotAxis::rem_opt (char o)
{
  std::string::size_type found = opt.find(o);
  if (found != std::string::npos)
    opt.erase (found, 1);
}

