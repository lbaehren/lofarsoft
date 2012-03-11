/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/UsingXSERVE.h"
#include <cpgplot.h>

Pulsar::UsingXSERVE::UsingXSERVE ( const std::string& device )
{
  devid = cpgopen( device.c_str() );
}

Pulsar::UsingXSERVE::~UsingXSERVE()
{
  cpgslct (devid);
  cpgclos ();
}

//! This method implements the QtPglot::plotter virtual method
void Pulsar::UsingXSERVE::plotter ()
{
  cpgslct (devid);
  cpgpage ();
  plot_data();
}


