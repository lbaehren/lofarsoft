




#include "Pulsar/DigitiserStatsPlot.h"
#include <pairutil.h>



using Pulsar::DigitiserStatsPlot;




DigitiserStatsPlot::Interface::Interface( DigitiserStatsPlot *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &DigitiserStatsPlot::get_subint,
         &DigitiserStatsPlot::set_subint,
         "subint", "Sub integration" );

  add( &DigitiserStatsPlot::get_srange,
	&DigitiserStatsPlot::set_srange,
	"srange", "Range of sub integrations to plot" );
}


