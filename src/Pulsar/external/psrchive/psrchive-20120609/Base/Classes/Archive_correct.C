/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Correction.h"
#include "Registry.h"

using namespace std;

void Pulsar::Archive::correct ()
{
  Check::ensure_linkage();
  Registry::List<Check>& check_registry = Registry::List<Check>::get_registry();

  if (verbose == 3)
    cerr << "Pulsar::Archive::correct " << check_registry.size()
      << " entries" << endl;

  for (unsigned check=0; check<check_registry.size(); check++) {

    if (verbose == 3)
      cerr << "Pulsar::Archive::correct testing "
           << check_registry[check]->get_name() << endl;

    Correction* correction = dynamic_cast<Correction*>(check_registry[check]);

    if (!correction)
      continue;

    if (verbose == 3)
      cerr << "Pulsar::Archive::correct applying "
           << correction->get_name() << endl;

    correction->apply(this);

  }
}
