

#include <Pulsar/CalPhVFreqPlot.h>






Pulsar::CalPhVFreqPlot::Interface::Interface( CalPhVFreqPlot *s_instance )
{
  set_instance( s_instance );
  
  add( &CalPhVFreqPlot::get_subint,
	&CalPhVFreqPlot::set_subint,
	"subint", "Sub-integration to plot" );
  
  add( &CalPhVFreqPlot::get_pol,
	&CalPhVFreqPlot::set_pol,
	"pol", "Polarization to plot" );
}


