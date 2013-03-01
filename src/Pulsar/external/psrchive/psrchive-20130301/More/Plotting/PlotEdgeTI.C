/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotEdge.h"
#include "pairutil.h"

Pulsar::PlotEdge::Interface::Interface (PlotEdge* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotEdge::get_viewport,
       &PlotEdge::set_viewport,
       "view", "Viewport (normalized device coordinates)" );
}
