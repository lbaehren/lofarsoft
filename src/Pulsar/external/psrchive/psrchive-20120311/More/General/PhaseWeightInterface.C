/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeightInterface.h"

Pulsar::PhaseWeight::Interface::Interface (PhaseWeight* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseWeight::get_weight_sum,
       "count", "Count of selected phase bins" );

  add( &PhaseWeight::get_weighted_sum,
       "sum", "Sum of selected phase bins" );

  add( &PhaseWeight::get_min,
       "min", "Minimum of selected phase bins" );

  add( &PhaseWeight::get_max,
       "max", "Maximum of selected phase bins" );

  add( &PhaseWeight::get_avg,
       "avg", "Average of selected phase bins" );

  add( &PhaseWeight::get_rms,
       "rms", "Standard deviation of selected phase bins" );

  add( &PhaseWeight::get_median,
       "med", "Median of selected phase bins" );

  add( &PhaseWeight::get_median_difference,
       "mdm", "Median difference from the median" );
}


