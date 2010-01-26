/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Parameters.h"

using namespace std;

/*! \param new_ephemeris the ephemeris to be installed
    \param update create a new polyco for the new ephemeris
 */
void Pulsar::Archive::set_ephemeris (const Parameters* new_ephemeris,
				     bool update)
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::set_ephemeris" << endl;

  if (!new_ephemeris)
    ephemeris = 0;
  else
    ephemeris = new_ephemeris->clone();

  if (ephemeris && update)
  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::set_ephemeris update predictor" << endl;
    update_model ();
  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::set_ephemeris exit" << endl;
}

