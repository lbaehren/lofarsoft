//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicSNSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"

Pulsar::DynamicSNSpectrumPlot::Interface::Interface( 
    DynamicSNSpectrumPlot *instance )
{
  if( instance )
    set_instance( instance );

  import( DynamicSpectrumPlot::Interface() );
}

