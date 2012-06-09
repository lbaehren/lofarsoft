//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicBaselineSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"

Pulsar::DynamicBaselineSpectrumPlot::Interface::Interface( 
    DynamicBaselineSpectrumPlot *instance )
{
  if( instance )
    set_instance( instance );

  add( &DynamicBaselineSpectrumPlot::get_use_variance,
      &DynamicBaselineSpectrumPlot::set_use_variance,
      "var", "Plot off-pulse variance rather than mean");

  add( &DynamicBaselineSpectrumPlot::get_expression,
       &DynamicBaselineSpectrumPlot::set_expression,
       "exp", "Expression to evaluate" );

  import( DynamicSpectrumPlot::Interface() );
}

