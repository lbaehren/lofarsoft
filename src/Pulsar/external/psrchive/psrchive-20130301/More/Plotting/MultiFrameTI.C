/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiFrame.h"
#include "Pulsar/PlotScale.h"

using namespace std;

Pulsar::MultiFrame::Interface::Interface (MultiFrame* instance)
{
  if (instance)
    set_instance (instance);

  // cerr << "Pulsar::MultiFrame::Interface ctor" << endl;

  import_filter = true;

  import ( "x", PlotEdge::Interface(), &MultiFrame::get_x_edge );

  if (instance && instance->has_shared_x_scale())
    insert ( "x", instance->get_shared_x_scale()->get_interface() );

  import ( "y", PlotEdge::Interface(), &MultiFrame::get_y_edge );

  if (instance && instance->has_shared_y_scale())
    insert ( "y", instance->get_shared_y_scale()->get_interface() );

  import ( PlotAttributes::Interface() );

  import ( "", std::string(), PlotFrame::Interface(),
	   &MultiFrame::get_frame );
}
