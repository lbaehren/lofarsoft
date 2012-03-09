/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FixFluxCal.h"
#include "Pulsar/StandardCandles.h"

using namespace std;

Pulsar::FixFluxCal::FixFluxCal ()
{
}

Pulsar::FixFluxCal::~FixFluxCal ()
{
}

void Pulsar::FixFluxCal::apply (Archive* archive)
{
  changes = "no changes required";

  if (archive->get_type() != Signal::PolnCal 
      && archive->get_type() != Signal::FluxCalOn
      && archive->get_type() != Signal::FluxCalOff)
    return;

  if (!fluxcals)
    fluxcals = new StandardCandles;

  sky_coord coord = archive->get_coordinates ();
  string name = archive->get_source ();

  Signal::Source type = fluxcals->guess (name, coord);

  if (type == Signal::Unknown)
    throw Error (InvalidParam, "Pulsar::FixFluxCal::apply:",
		 "name="+name+" coord="+coord.getHMSDMS()+" type unknown");

  archive->set_type (type);
  changes = Signal::Source2string(type);

  if (name != archive->get_source ()) {
    changes += " and name " + archive->get_source() + " -> " + name;
    archive->set_source (name);
  }
}
