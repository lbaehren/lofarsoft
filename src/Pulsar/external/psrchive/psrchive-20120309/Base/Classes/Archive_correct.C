/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Correction.h"

using namespace std;

void Pulsar::Archive::correct ()
{
  for (unsigned check=0; check<Check::registry.size(); check++) {

    if (verbose == 3)
      cerr << "Pulsar::Archive::correct testing "
           << Check::registry[check]->get_name() << endl;

    Correction* correction = dynamic_cast<Correction*>(Check::registry[check]);

    if (!correction)
      continue;

    if (verbose == 3)
      cerr << "Pulsar::Archive::correct applying "
           << correction->get_name() << endl;

    correction->apply(this);

  }
}
