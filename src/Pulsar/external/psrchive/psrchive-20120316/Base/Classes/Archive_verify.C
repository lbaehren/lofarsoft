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
  for (unsigned check=0; check<Check::registry.size(); check++)
  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::verify testing "
           << Check::registry[check]->get_name() << endl;

    Verification* verification = 0;
    verification = dynamic_cast<Verification*>(Check::registry[check]);

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

