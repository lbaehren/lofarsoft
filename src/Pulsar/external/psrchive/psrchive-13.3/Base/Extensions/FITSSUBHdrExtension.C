/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/FITSSUBHdrExtension.h"



using Pulsar::FITSSUBHdrExtension;




//Default constructor
FITSSUBHdrExtension::FITSSUBHdrExtension ()
    : Extension( "FITSSUBHdrExtension" )
{
  int_type = "";
  int_unit = "";
  tsamp = -1;
  nbits = -1;
  nch_strt = -1;
  nsblk = -1;
  nrows = -1;
}

// Copy constructor
FITSSUBHdrExtension::FITSSUBHdrExtension (const Pulsar::FITSSUBHdrExtension& extension)
    : Extension( "FITSSUBHdrExtension" )
{
  operator = (extension);
}

// Operator =
const FITSSUBHdrExtension& FITSSUBHdrExtension::operator= (const FITSSUBHdrExtension& extension)
{
  int_type = extension.int_type;
  int_unit = extension.int_unit;
  tsamp = extension.tsamp;
  nbits = extension.nbits;
  nch_strt = extension.nch_strt;
  nsblk = extension.nsblk;
  nrows = extension.nrows;

  return *this;
}

// Destructor
FITSSUBHdrExtension::~FITSSUBHdrExtension ()
{}


TextInterface::Parser* FITSSUBHdrExtension::get_interface()
{
  return new Interface( this );
}



