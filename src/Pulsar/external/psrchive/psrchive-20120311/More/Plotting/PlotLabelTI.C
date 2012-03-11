/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotLabel.h"

Pulsar::PlotLabel::Interface::Interface (PlotLabel* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotLabel::get_left,
       &PlotLabel::set_left,
       "l", "Top left frame label" );

  add( &PlotLabel::get_centre,
       &PlotLabel::set_centre,
       "c", "Top centre frame label" );

  add( &PlotLabel::get_right,
       &PlotLabel::set_right,
       "r", "Top right frame label" );

  add( &PlotLabel::get_offset,
       &PlotLabel::set_offset,
       "off", "Character height between label and frame" );

  add( &PlotLabel::get_spacing,
       &PlotLabel::set_spacing,
       "sep", "Character height between label rows" );

}
