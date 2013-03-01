/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Verification.h"

using namespace std;

void Pulsar::Archive::verify () const try
{
  Check::ensure_linkage();
  Registry::List<Check>& check_registry = Registry::List<Check>::get_registry();

  for (unsigned check=0; check<check_registry.size(); check++)
  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::verify testing "
           << check_registry[check]->get_name() << endl;

    Verification* verification = 0;
    verification = dynamic_cast<Verification*>(check_registry[check]);

    if (!verification)
      continue;

    if (verbose == 3)
      cerr << "Pulsar::Archive::verify applying "
           << verification->get_name() << endl;

    verification->apply(this);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::verify";
}

