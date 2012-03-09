/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FeedExtension.h"
#include "TextInterface.h"

//! Default constructor
Pulsar::FeedExtension::FeedExtension ()
  : Extension ("Feed")
{
}

//! Copy constructor
Pulsar::FeedExtension::FeedExtension (const FeedExtension& extension)
  : Extension ("Feed")
{
  transformation = extension.transformation;
}

//! Operator =
const Pulsar::FeedExtension&
Pulsar::FeedExtension::operator= (const FeedExtension& extension)
{
  transformation = extension.transformation;
  return *this;
}

//! Destructor
Pulsar::FeedExtension::~FeedExtension ()
{
}

// Text interface to a FeedExtension
class Pulsar::FeedExtension::Interface 
  : public TextInterface::To<FeedExtension>
{
public:
  Interface( FeedExtension *s_instance  = NULL )
  {
    if( s_instance )
      set_instance( s_instance );
  }
};

//! Get the text interface
TextInterface::Parser* Pulsar::FeedExtension::get_interface()
{
  return new Interface( this );
}

