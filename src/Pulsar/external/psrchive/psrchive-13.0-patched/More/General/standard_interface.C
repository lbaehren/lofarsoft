/***************************************************************************
 *
 *   Copyright (C) 2008-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StatisticsInterface.h"
#include "Pulsar/Archive.h"

using namespace Pulsar;

TextInterface::Parser* standard_interface (Archive* archive)
{
  Reference::To<TextInterface::Parser> interface = archive->get_interface();

  interface->set_indentation (" ");

  interface->insert( new Statistics::Interface( new Statistics(archive) ) );

  return interface.release();
}
