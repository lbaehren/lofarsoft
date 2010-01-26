/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/IntegrationOrder.h"

//! Default constructor
Pulsar::IntegrationOrder::IntegrationOrder (const char* name)
  : Extension (name)
{
  IndexState = "Null State";
}

//! Copy constructor
Pulsar::IntegrationOrder::IntegrationOrder (const IntegrationOrder& extension)
  : Extension (extension.get_extension_name().c_str())
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
}

//! Operator =
const Pulsar::IntegrationOrder&
Pulsar::IntegrationOrder::operator= (const IntegrationOrder& extension)
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
  return *this;
}

//! Destructor
Pulsar::IntegrationOrder::~IntegrationOrder ()
{
}

std::string Pulsar::IntegrationOrder::get_IndexState () const
{
  return IndexState;
}

std::string Pulsar::IntegrationOrder::get_Unit () const
{
  return Unit;
}

void Pulsar::IntegrationOrder::resize (unsigned i)
{
  indices.resize(i);
}

unsigned Pulsar::IntegrationOrder::size () const
{
  return indices.size();
}

void Pulsar::IntegrationOrder::erase (unsigned i)
{
  indices.erase( indices.begin() + i );
}

double Pulsar::IntegrationOrder::get_Index (unsigned subint) const
{
  if (subint >= indices.size())
    throw Error (InvalidParam, "Pulsar::IntegrationOrder::get_Index",
		 "Invalid index number");
  
  return indices[subint];
}

void Pulsar::IntegrationOrder::set_Index (unsigned subint, double i)
{
  if (subint >= indices.size())
    throw Error (InvalidParam, "Pulsar::IntegrationOrder::set_Index",
		 "Invalid index number");

  indices[subint] = i;
}
