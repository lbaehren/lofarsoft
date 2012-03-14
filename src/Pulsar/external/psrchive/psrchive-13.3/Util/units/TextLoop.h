//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/TextLoop.h,v $
   $Revision: 1.2 $
   $Date: 2009/02/26 08:38:25 $
   $Author: straten $ */

#ifndef __Pulsar_TextLoop_h
#define __Pulsar_TextLoop_h

#include "TextIndex.h"
#include "Functor.h"
#include <stack>

//! Loop through ranges of indeces ascertained by a TextInterface
class TextLoop : public Reference::Able 
{
public:

  //! Default constructor
  TextLoop ();

  //! Set the interface of the container to which the named indeces apply
  void set_container (TextInterface::Parser*);

  //! Add an index over which to loop
  void add_index (TextIndex*);

  //! Execute the job for each index in the stack
  void loop ();

  //! Job defined by derived types
  Functor< void() > job;

  //! Retrieve the index state
  std::string get_index_state () const;

protected:

  //! The indeces over which to loop
  typedef std::stack< Reference::To<TextIndex> > Stack;
  Stack indeces;

  //! The container interface to configure on each loop
  Reference::To<TextInterface::Parser> container;

  //! Worker function called recursively down stack
  void loop ( Stack& indeces );

  //! Current state of each index
  std::string index_state;
};

#endif
