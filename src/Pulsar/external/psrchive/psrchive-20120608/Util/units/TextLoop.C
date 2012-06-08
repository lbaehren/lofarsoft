/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TextLoop.h"

using namespace std;

//! Default constructor
TextLoop::TextLoop ()
{
}

//! Add an index over which to loop
void TextLoop::add_index (TextIndex* index)
{
  indeces.push( index );
}

//! Set the interface of the container to which the named indeces apply
void TextLoop::set_container (TextInterface::Parser* parser)
{
  container = parser;
}

// execute the job for each index in the stack
void TextLoop::loop ()
{
  loop (indeces);
}

void TextLoop::loop( std::stack< Reference::To<TextIndex> >& indeces )
{
  if( indeces.empty() )
  {
    // bottom of stack; perform job
    job ();
    return;
  }

  Reference::To<TextIndex> index = indeces.top();
  indeces.pop();

  index->set_container( container );

  const string backup_state = index_state;

  for (unsigned i=0; i<index->size(); i++) try
  {
    string index_command = index->get_index(i);

    index_state = backup_state + " " + index_command;

    container->process( index_command );
    loop (indeces);

    index_state = backup_state;
  }
  catch (Error& error)
  {
    index_state = backup_state;
  }

  indeces.push( index );
}

std::string TextLoop::get_index_state () const
{
  return index_state;
}

