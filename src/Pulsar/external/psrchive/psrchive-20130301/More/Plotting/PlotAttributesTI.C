/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotAttributes.h"

Pulsar::PlotAttributes::Interface::Interface (PlotAttributes* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotAttributes::get_character_height,
       &PlotAttributes::set_character_height,
       "ch", "Character height" );

  add( &PlotAttributes::get_character_font,
       &PlotAttributes::set_character_font,
       "cf", "Character font" );

  add( &PlotAttributes::get_line_width,
       &PlotAttributes::set_line_width,
       "lw", "Line width" );

  add( &PlotAttributes::get_applied,
       &PlotAttributes::apply_set,
       "set", "Apply an option set ('pub' or 'blank')" );

  import ( "above", PlotLabel::Interface(), &PlotAttributes::get_label_above );

}
