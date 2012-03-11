/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TextIndex.h"

#include <assert.h>

using namespace std;

TextIndex::TextIndex (const std::string& named_range)
{
  string::size_type set = named_range.find('=');

  if (set == string::npos)
    throw Error( InvalidParam, "TextIndex ctor", "argument has no '=' sign" );

  // string before the equals sign
  string before = named_range.substr (0,set);
  // string after the equals sign
  string after = named_range.substr (set+1);

  // remove any white space
  name = stringtok (before, " \t");
  range = stringtok (after, " \t");

  size_name = "n" + name;
}

//! Set the interface from which the maximum index will be determined
void TextIndex::set_container (TextInterface::Parser* parser)
{
  string number = parser->get_value( size_name );

  indeces.resize(0);

  TextInterface::parse_indeces( indeces, range, fromstring<unsigned>(number) );
}

void TextIndex::set_size_name (const std::string& name)
{
  size_name = name;
}

//! Get the index command for the specified index
std::string TextIndex::get_index (unsigned i)
{
  assert (i < indeces.size());

  unsigned index = indeces[i];

  return name + "=" + tostring(index);
}

unsigned TextIndex::size () const
{
  return indeces.size();
}
