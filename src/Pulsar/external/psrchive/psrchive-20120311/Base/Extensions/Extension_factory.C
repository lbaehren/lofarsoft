
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/ITRFExtension.h"

Pulsar::Archive::Extension* 
Pulsar::Archive::Extension::factory (const std::string& name_parse)
{
  std::vector< Reference::To<Archive::Extension> > instances;

  instances.push_back( new ITRFExtension );

  return TextInterface::factory<Archive::Extension> (instances, name_parse);
}

