/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ObsExtension.h"


//! Default constructor
Pulsar::ObsExtension::ObsExtension ()
    : Extension ("ObsExtension")
{
  observer = "UNSET";
  affiliation = "UNSET";
  project_ID = "UNSET";
}

//! Copy constructor
Pulsar::ObsExtension::ObsExtension (const ObsExtension& extension)
    : Extension ("ObsExtension")
{
  observer = extension.observer;
  affiliation = extension.affiliation;
  project_ID = extension.project_ID;
}

//! Operator =
const Pulsar::ObsExtension&
Pulsar::ObsExtension::operator= (const ObsExtension& extension)
{
  observer = extension.observer;
  affiliation = extension.affiliation;
  project_ID = extension.project_ID;

  return *this;
}

//! Destructor
Pulsar::ObsExtension::~ObsExtension ()
{}


//! Ge text interface
TextInterface::Parser* Pulsar::ObsExtension::get_interface()
{
  return new Interface( this );
}
