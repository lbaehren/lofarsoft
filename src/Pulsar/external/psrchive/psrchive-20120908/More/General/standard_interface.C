/***************************************************************************
 *
 *   Copyright (C) 2008-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StatisticsInterface.h"
#include "Pulsar/PolnStatistics.h"
#include "Pulsar/Archive.h"
#include "substitute.h"
#include "evaluate.h"

using namespace Pulsar;

TextInterface::Parser* standard_interface (Archive* archive)
{
  // print/parse in degrees
  Angle::default_type = Angle::Degrees;

  Reference::To<TextInterface::Parser> interface = archive->get_interface();

  interface->set_indentation (" ");

  Statistics* stats = new Statistics(archive);
  if (archive->get_npol() == 4)
    stats->add_plugin ( new PolnStatistics );

  interface->insert( new Statistics::Interface( stats ) );

    
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
