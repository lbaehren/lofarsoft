/***************************************************************************
 *
 *   Copyright (C) 2008-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StatisticsInterface.h"
#include "Pulsar/Archive.h"
#include "substitute.h"
#include "evaluate.h"

using namespace Pulsar;

TextInterface::Parser* standard_interface (Archive* archive)
{
  Reference::To<TextInterface::Parser> interface = archive->get_interface();

  interface->set_indentation (" ");

  interface->insert( new Statistics::Interface( new Statistics(archive) ) );

  return interface.release();
}

std::string process (TextInterface::Parser* interface, const std::string& text)
{
  if ( text.find('$') == std::string::npos )
    return interface->process ( text );
  else
    return interface->get_indentation() + 
      evaluate( substitute( text, interface ) );
}
