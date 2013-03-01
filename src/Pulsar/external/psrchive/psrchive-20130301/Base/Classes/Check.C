/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Check.h"

void Pulsar::Archive::Check::disable (const std::string& name)
{
  Registry::List<Check>& registry = Registry::List<Check>::get_registry();

  for (unsigned icheck=0; icheck < registry.size();)
    if (registry[icheck]->get_name() == name)
      registry.erase(icheck);
    else
      icheck ++;
}
