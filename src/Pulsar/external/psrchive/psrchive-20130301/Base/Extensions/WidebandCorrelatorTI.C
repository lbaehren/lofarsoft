/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/BackendInterface.h"

using Pulsar::WidebandCorrelator;

WidebandCorrelator::Interface::Interface( WidebandCorrelator *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  import ( Backend::Interface() );

  add( &WidebandCorrelator::get_config,
       &WidebandCorrelator::set_config,
       "config", "Configuration filename" );

  add( &WidebandCorrelator::get_nrcvr,
       &WidebandCorrelator::set_nrcvr,
       "nrcvr", "Number of receiver channels" );

  add( &WidebandCorrelator::get_tcycle,
       &WidebandCorrelator::set_tcycle,
       "tcycle", "Correlator cycle time" );
}
