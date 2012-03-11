//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicCalSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"

Pulsar::DynamicCalSpectrumPlot::Interface::Interface( 
    DynamicCalSpectrumPlot *instance )
{
  if( instance )
    set_instance( instance );

  import( DynamicSpectrumPlot::Interface() );
}

