/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/DigitiserCountsPlot.h"
#include <pairutil.h>



using namespace Pulsar;



DigitiserCountsPlot::Interface::Interface( DigitiserCountsPlot *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &DigitiserCountsPlot::get_srange,
	&DigitiserCountsPlot::set_srange,
	"srange", "Range of subints to plot" );
  
  add( &DigitiserCountsPlot::get_subint,
	&DigitiserCountsPlot::set_subint,
	"subint", "sub integration to plot" );
}


