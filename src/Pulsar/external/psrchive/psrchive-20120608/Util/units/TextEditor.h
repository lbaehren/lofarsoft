//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/TextEditor.h,v $
   $Revision: 1.1 $
   $Date: 2007/11/18 11:56:53 $
   $Author: straten $ */

#ifndef __TextEditor_h
#define __TextEditor_h

#include "TextInterface.h"

//! Edit the metadata in a pulsar archive
template<class T>
class TextEditor : public Reference::Able
{

 public:

  TextEditor ()
    {
      prefix_name = true;
      output_identifier = true;
    }

  //! Add a semi-colon or comma-separated list of commands to execute
  void add_commands (const std::string& str)
  { standard_separation (commands, str); }

  //! Add a semi-colon or comma-separated list of extensions to install
  void add_extensions (const std::string& str)
  { standard_separation (extensions, str); }

  //! Return true if the process method will modify the archive
  bool will_modify () const;

  //! Edit the Archive according to the current state
  std::string process (T*);

  //! Add the named extension to the instance
  virtual void add_extension (T*, const std::string& name) { }

  //! Get the identifier for this instance
  virtual std::string get_identifier (const T*) { return std::string(); }

  //! Prefix parameter value queries with parameter name=
  bool prefix_name;

  //! Print the name of each file processed
  bool output_identifier;

  //! Optionally set the delimiter used to separate elements of a container
  std::string delimiter;

  protected:

  //! commands to be executed
  std::vector<std::string> commands;

  //! extensions to be added
  std::vector<std::string> extensions;

};

template <typename T> 
bool TextEditor<T>::will_modify () const
{
  if (extensions.size() > 0)
    return true;

  for (unsigned j = 0; j < commands.size(); j++)
    if (commands[j].find('=') != std::string::npos)
      return true;

  return false;
}

template <typename T> 
std::string TextEditor<T>::process (T* instance)
{
  for (unsigned i = 0; i < extensions.size(); i++)
    add_extension (instance, extensions[i]);

  Reference::To<TextInterface::Parser> interface = instance->get_interface();

  if (commands.size() == 0)
    return interface->help (true);

  // so that a space precedes each parameter processed
  interface->set_indentation (" ");
  interface->set_prefix_name (prefix_name);

  if (delimiter.length())
    interface->set_delimiter (delimiter);

  std::string result;

  if (output_identifier)
    result = get_identifier (instance);

  for (unsigned j = 0; j < commands.size(); j++)
    result += interface->process (commands[j]);

  return result;
}

#endif
