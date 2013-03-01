/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "CommandLine.h"
#include "pad.h"

#include <stdlib.h>
#include <string.h>

using namespace std;

//! Return two columns of help text
CommandLine::Help CommandLine::Argument::get_help () const 
{
  string first;
  if ( !short_name.empty() )
    first += "-" + short_name;

  if ( !long_name.empty() )
  {
    if (!first.empty())
      first += ",";

    first += "-" + long_name;
  }

  if ( !type.empty() )
    first += " " + type;

  return Help ( first, help );
}

CommandLine::Help CommandLine::Alias::get_help () const 
{
  Help my_help = Argument::get_help ();
  Help arg_help = argument->get_help();

  my_help.second = "alias to " + arg_help.first;
  return my_help;
}

void CommandLine::Alias::set_argument (Argument* _arg)
{
  argument = _arg;
  has_arg = argument->has_arg;
  handled = 0;
}

CommandLine::Menu::Menu ()
{
  help_item = version_item = 0;
}

void CommandLine::Menu::add_help ()
{
  if (help_item)
    return;

  help_item = add (this, &Menu::help, 'h');
  help_item->set_long_name ("help");
  help_item->set_help ("print this help page");
  help_item->has_arg = optional_argument;
}

void CommandLine::Menu::add_version ()
{
  if (version_item)
    return;

  if (version_info.empty())
    return;

  version_item = add (this, &Menu::version, 'i');
  version_item->set_long_name ("version");
  version_item->set_help ("version information");
}

CommandLine::Menu::~Menu ()
{
}

//! Add an item to the menu
void CommandLine::Menu::add (Item* i)
{
  item.push_back (i);
}

//! Add a Heading with the given text
void CommandLine::Menu::add (const std::string& text)
{
  add( new Heading (text) );
}

template<typename T>
void resize (T* &ptr, unsigned size)
{
  ptr = (T*) realloc (ptr, size * sizeof(T));
}

void CommandLine::Menu::parse (int argc, char* const * argv)
{
  string shortopts;
  struct option* longopts = 0;
  unsigned nlong = 0;

  for (unsigned i=0; i<item.size(); i++)
  {
    Argument* arg = dynamic_cast<Argument*>( item[i].get() );
    if (!arg)
      continue;

    if (arg->short_name.empty())
      arg->val = nlong + 1024;
    else
    {
      arg->val = arg->short_name[0];
      shortopts += arg->short_name;
      if (arg->has_arg == required_argument)
	shortopts += ":";
    }

    if (arg->long_name.empty())
      continue;

    resize (longopts, nlong+1);
    
    longopts[nlong].name = strdup (arg->long_name.c_str());
    longopts[nlong].has_arg = arg->has_arg;
    longopts[nlong].flag = NULL;
    longopts[nlong].val = arg->val;

    nlong ++;
  }

  resize (longopts, nlong+1);
    
  longopts[nlong].name = 0;
  longopts[nlong].has_arg = 0; 
  longopts[nlong].flag = NULL;
  longopts[nlong].val = 0;

  int code = 0;
  const char* optstring = shortopts.c_str();

  // disable getopt_long error messages
  opterr = 0;

  while ((code = getopt_long_only(argc, argv, optstring, longopts, 0)) != -1) 
  {
    if (code == '?')
      code = process_error (code, argv);

    for (unsigned i=0; i<item.size(); i++)
    {
      if (item[i]->matches (code))
      {
	string arg;
	if (optarg)
	  arg = optarg;

	item[i]->handle (arg);
	item[i]->set_handled (true);

	break;
      }
    }
  }
}

int CommandLine::Menu::process_error (int code, char* const *argv)
{
  throw Error (InvalidParam, "CommandLine::Menu::parse",
	       "invalid option '%s'", argv[optind-1]);
}

//! Find the named Argument
CommandLine::Argument* CommandLine::Menu::find (const std::string& name)
{
  for (unsigned i=0; i<item.size(); i++)
  {
    Argument* arg = dynamic_cast<Argument*>( item[i].get() );
    if (arg && (arg->short_name == name || arg->long_name == name))
      return arg;
  }
  throw Error (InvalidParam, "CommandLine::Menu::find",
	       "no option named '" + name + "'");
}

void CommandLine::Menu::help (const std::string& _name)
{

  if (!_name.empty())
  {
    // cerr << "CommandLine::Menu::help name='" << _name << "'" << endl;

    // Copy input and strip any leading -'s
    string name = _name;
    while (name[0]=='-') 
      name.erase(0,1);

    Argument* arg = find (name);

    cout << "Detailed help for -" << name << ":" << endl;

    if (dynamic_cast<Alias*>(arg))
      cout << "  -" << name << " is an " << arg->get_help().second << endl;

    else 
    {

      cout << endl << 
        "  " << arg->get_help().first << "  " << arg->get_help().second
        << endl << endl;

      if (arg->long_help.empty())
        cout << "no further help available for -" << name << endl;
    
      else
        cout << arg->long_help << endl;
    }

    cout << endl;
    
    exit (0);
  }

  size_t max_length = 0;

  for (unsigned i=0; i<item.size(); i++)
  {
    Help help = item[i]->get_help();
    if (!help.second.empty())
      max_length = std::max (max_length, help.first.length());
  }

  cout << help_header << endl << endl;

  bool any_long_help = false;

  for (unsigned i=0; i<item.size(); i++)
  {
    Help help = item[i]->get_help();

    if (dynamic_cast<Alias*>(item[i].get()))
      continue;

    bool has_long_help = false;
    Argument *arg = dynamic_cast<Argument*>(item[i].get());
    if (arg)
      has_long_help = !(arg->long_help.empty());

    if (has_long_help)
      any_long_help = true;

    if (dynamic_cast<Heading*>(item[i].get()))
      cout << help.first << endl;
    else
      cout <<"  "<< pad(max_length, help.first) << " "
        << (has_long_help ? "*" : " ") << " " << help.second << endl;
  }

  if (any_long_help)
    cout  <<
      "\nOptions marked with '*' have extra help available via '-help=option'."
      << endl;

  cout << help_footer << endl;

  exit (0);
}

void CommandLine::Menu::version ()
{
  cout << version_info << endl;
  exit (0);
}


