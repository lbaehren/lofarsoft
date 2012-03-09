/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/TimeAppend.h"

#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::append 
//
/*! 
  Add clones of the Integrations in arch to this.
 */
void Pulsar::Archive::append (const Archive* arch) try
{
  TimeAppend append;
  append.append (this, arch);
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::append";
}
