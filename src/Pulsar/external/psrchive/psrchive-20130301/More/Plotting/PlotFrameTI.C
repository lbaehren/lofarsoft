/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotFrame.h"
#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"

Pulsar::PlotFrame::Interface::Interface (PlotFrame* instance)
{
  if (instance)
  {
    set_instance (instance);
    insert ( "x", instance->get_x_scale()->get_interface() );
    insert ( "y", instance->get_y_scale()->get_interface() );
  }
  else
  {
    import ( "x", PlotScale::Interface(),
	     (PlotScale* (PlotFrame::*)(void)) &PlotFrame::get_x_scale );
    import ( "y", PlotScale::Interface(),
	     (PlotScale* (PlotFrame::*)(void)) &PlotFrame::get_y_scale );
  }

  import ( "x", PlotAxis::Interface(), 
	   (PlotAxis* (PlotFrame::*)(void)) &PlotFrame::get_x_axis );
  import ( "y", PlotAxis::Interface(),
	   (PlotAxis* (PlotFrame::*)(void)) &PlotFrame::get_y_axis );

  add( &PlotFrame::get_transpose,
       &PlotFrame::set_transpose,
       "transpose", "Transpose the x and y axes (experimental)" );

  import ( PlotAttributes::Interface() );

  import ( "below", PlotLabel::Interface(), &PlotFrame::get_label_below );

}
