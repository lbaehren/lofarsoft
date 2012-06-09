



#include "Pulsar/BandpassChannelWeightPlot.h"



using namespace Pulsar;



BandpassChannelWeightPlot::Interface::Interface( BandpassChannelWeightPlot *s_instance )
{
  if( s_instance )
    set_instance( s_instance );
  
  import ( "psd", SpectrumPlot::Interface(), &BandpassChannelWeightPlot::get_psd );
  import ( "weights", ChannelWeightsPlot::Interface(), &BandpassChannelWeightPlot::get_weights );
  
  import( "x", PlotScale::Interface(), &BandpassChannelWeightPlot::get_x_scale );
  
  import( "y", PlotScale::Interface(), &BandpassChannelWeightPlot::get_y_scale );
}


