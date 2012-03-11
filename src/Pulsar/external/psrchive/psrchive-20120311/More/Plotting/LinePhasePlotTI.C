


#include "Pulsar/LinePhasePlot.h"
#include <pairutil.h>



using Pulsar::LinePhasePlot;


/**
 * @brief Constructor
 *
 * Set the instance given, add getters/setters for isub, srange and ipol
 **/

LinePhasePlot::Interface::Interface( LinePhasePlot *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  import ( PhasePlot::Interface() );

  add( &LinePhasePlot::get_isub,
         &LinePhasePlot::set_isub,
         "subint", "Sub integration to plot" );

  add( &LinePhasePlot::get_srange,
         &LinePhasePlot::set_srange,
         "srange", "Range of sub integrations to plot" );

  add( &LinePhasePlot::get_ipol,
         &LinePhasePlot::set_ipol,
         "ipol", "Polarization to plot" );

  add( &LinePhasePlot::get_line_colour,
         &LinePhasePlot::set_line_colour,
         "col", "Line Colour" );

  add( &LinePhasePlot::get_ichan,
         &LinePhasePlot::set_ichan,
         "ichan", "Channel to plot" );
}
