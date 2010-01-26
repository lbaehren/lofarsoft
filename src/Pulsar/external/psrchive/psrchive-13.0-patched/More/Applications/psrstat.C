/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  For printing attributes (like centre frequency and bandwidth) along
  with statistics (like S/N and degree of polarization) from folded
  pulsar data.
*/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Statistics.h"
#include "Pulsar/Archive.h"

#include "substitute.h"
#include "evaluate.h"

#include "TextLoop.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>

using namespace std;
using namespace Pulsar;

//! Pulsar Statistics application
class psrstat: public Pulsar::Application
{
public:

  //! Default constructor
  psrstat ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  void add_expressions (const std::string& str)
  { separate (str, expressions, ","); }

  void add_loop (const std::string& str)
  { loop.add_index( new TextIndex(optarg) ); }

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! The expressions to be evaluated
  vector<string> expressions;

  //! Indeces over which to loop
  TextLoop loop;

  //! Print the name of each file
  bool output_filename;

  //! The interface to the current archive
  Reference::To<TextInterface::Parser> interface;

  //! The current archive
  Reference::To<Pulsar::Archive> archive;

  //! Precede each value queried with name=
  bool prefix_name;

  //! Job to be performed on each leaf index
  void print ();

  void set_quiet () { Application::set_quiet(); output_filename = false; }
};

int main (int argc, char** argv)
{
  psrstat program;
  return program.main (argc, argv);
}

psrstat::psrstat ()
  : Pulsar::Application ("psrstat", "prints pulsar attributes and statistics")
{
  has_manual = true;
  version = "$Id: psrstat.C,v 1.7 2009/09/02 02:54:31 straten Exp $";

  // print/parse in degrees
  Angle::default_type = Angle::Degrees;

  // suppress warnings by default
  Archive::set_verbosity (0);

  // print the name of each file processed
  output_filename = true;
  prefix_name = true;

  loop.job.set( this, &psrstat::print );

  add( new Pulsar::StandardOptions );
}

void psrstat::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psrstat::add_expressions, 'c', "exp[s]");
  arg->set_help ("expressions to be evaluated and printed");
  arg->set_long_help
    ("exp is either the name of a parameter (attribute or statistic) \n"
     "or a mathematical expression involving one or more parameters and \n"
     "enclosed in braces");

  arg = menu.add (this, &psrstat::add_loop, 'l', "name=<range>");
  arg->set_help ("loop over the range of the named parameter");
  arg->set_long_help
    ("e.g.\n"
     " -l chan=0-5     loop over frequency channels 0 to 5 (inclusive) \n"
     " -l subint=2,5-  loop over sub-integrations 2, then 5 to last");

  arg = menu.add (prefix_name, 'Q');
  arg->set_help ("do not prefix output with 'keyword='");

  menu.set_help_footer
    ("\n"
     "Multiple expressions and/or index ranges may be specified by using \n"
     "the -c and/or -l options multiple times. \n"
     "\n"
     "For the complete list of all available parameters in an archive, \n"
     "run \"psrstat <filename>\" without any command line options.\n"
     + menu.get_help_footer());
}

void psrstat::process (Pulsar::Archive* _archive)
{
  archive = _archive;

  interface = standard_interface( archive );
  interface->set_prefix_name (prefix_name);
    
  if (expressions.size() == 0)
  {
    cout << interface->help (true) << endl;;
    return;
  }

  loop.set_container (interface);
  loop.loop ();
}

void psrstat::print ()
{
  if (output_filename)
    cout << archive->get_filename() << loop.get_index_state();

  for (unsigned j = 0; j < expressions.size(); j++)
  {
    string text = expressions[j];

    if (verbose)
      cerr << "psrstat: processing '" << text << "'" << endl;

    if ( text.find('$') == string::npos )
      cout << interface->process ( text );
    else
      cout << " " << evaluate( substitute( text, interface.get() ) ); 
  }
  
  cout << endl;
}

