/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#include <Reference.h>
#include <TextInterface.h>
#include <vector>




namespace Pulsar
{
  class Extension_register : public std::vector< Reference::To< Archive::Extension > >
  {
   public:
      Extension_register();
  };
  
  extern Extension_register extension_registry;
}




