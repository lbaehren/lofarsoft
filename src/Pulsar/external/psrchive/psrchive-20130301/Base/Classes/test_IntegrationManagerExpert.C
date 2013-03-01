/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/IntegrationManagerExpert.h"

int main ()
{
  Pulsar::IntegrationManager* manager = 0;

  if (manager)
    manager->expert()->sort();

  return 0;
}
