/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/WidebandCorrelator.h"

//! Default constructor
Pulsar::WidebandCorrelator::WidebandCorrelator ()
  : Backend ("WidebandCorrelator")
{
  configfile = "UNSET";
  nrcvr = 0;
  tcycle = 0.0;
}

//! Copy constructor
Pulsar::WidebandCorrelator::WidebandCorrelator (const WidebandCorrelator& ext)
  : Backend (ext)
{
  operator = (ext);
}

//! Operator =
const Pulsar::WidebandCorrelator&
Pulsar::WidebandCorrelator::operator= (const WidebandCorrelator& extension)
{
  Backend::operator = (extension);

  configfile = extension.configfile;
  nrcvr = extension.nrcvr;
  tcycle = extension.tcycle;

  return *this;
}

//! Destructor
Pulsar::WidebandCorrelator::~WidebandCorrelator ()
{
}


//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::WidebandCorrelator::get_interface()
{
  return new Interface( this );
}
