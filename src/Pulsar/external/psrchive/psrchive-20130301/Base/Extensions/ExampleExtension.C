/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ExampleExtension.h"

//! Default constructor
Pulsar::ExampleExtension::ExampleExtension ()
  : Extension ("Example")
{
}

//! Copy constructor
Pulsar::ExampleExtension::ExampleExtension (const ExampleExtension& extension)
  : Extension ("Example")
{
}

//! Operator =
const Pulsar::ExampleExtension&
Pulsar::ExampleExtension::operator= (const ExampleExtension& extension)
{
  return *this;
}

//! Destructor
Pulsar::ExampleExtension::~ExampleExtension ()
{
}
