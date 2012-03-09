/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ObsExtension.h"

Pulsar::ObsExtension::Interface::Interface (ObsExtension *s_instance)
{
  if (s_instance)
    set_instance (s_instance);

  add( &ObsExtension::get_observer,
       &ObsExtension::set_observer,
      "observer", "Observer name(s)" );

  add( &ObsExtension::get_project_ID,
       &ObsExtension::set_project_ID,
       "projid", "Project name" );
}




