/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "OwnStream.h"
#include <iostream>

//! Constructor
OwnStream::OwnStream ()
  : cout (std::cout.rdbuf()), cerr (std::cerr.rdbuf())
{
}

//! Constructor
OwnStream::OwnStream (const OwnStream& copy)
  : cout (copy.cout.rdbuf()), cerr (copy.cerr.rdbuf())
{
}

//! Assignment operator
const OwnStream& OwnStream::operator = (const OwnStream&)
{
  return *this;
}

void OwnStream::set_cout (std::ostream& os) const
{
  cout.rdbuf( os.rdbuf() );
}

void OwnStream::set_cerr (std::ostream& os) const
{
  cerr.rdbuf( os.rdbuf() );
}

