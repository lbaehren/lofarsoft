/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#include "Pulsar/FreqVsSNRPlot.h"



using namespace Pulsar;
using namespace std;



FreqVsSNRPlot::Interface::Interface( FreqVsSNRPlot *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &FreqVsSNRPlot::get_subint,
         &FreqVsSNRPlot::set_subint,
         "subint", "Set the sub integration to plot" );

  add( &FreqVsSNRPlot::get_pol,
         &FreqVsSNRPlot::set_pol,
         "pol", "Set the polarization to plot" );
}


