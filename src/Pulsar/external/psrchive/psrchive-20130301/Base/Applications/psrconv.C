/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Editor.h"
#include "Pulsar/UnloadOptions.h"

using namespace std;
using Pulsar::Editor;

//
//! Converts between file formats
//
class psrconv : public Pulsar::Application
{
public:

  //! Implements the unload behaviour required for this application
  class Unload;

  //! Default constructor
  psrconv ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! Archive editor
  Editor editor;

  //! The unloader
  Unload* unload;
};

int main (int argc, char** argv)
{
  psrconv program;
  return program.main (argc, argv);
}

//
//! Implements the unload behaviour for psrconv
//
/*!  The call to UnloadOptions::setup is disabled because, if no
  output method is specified, a default output file extension will be
  determined from the archive type.  Furthermore, psrconv breaks the
  model in which the process method performs an inplace operation and,
  upon completion, UnloadOption::finish writes the archive that was
  passed as an argument.
*/
class psrconv::Unload : public Pulsar::UnloadOptions
{ 

public:

  //! Disable the call to setup 
  void setup () {}

  //! Disable the call to finish
  void finish (Pulsar::Archive* archive) {}

  //! Unload with the default extension
  void unload (Pulsar::Archive* archive);
};


#if 0
    "Available output formats:" << endl;

  Pulsar::Archive::agent_list ();
#endif

static string output_format = "PSRFITS";
static string unload_cal_ext = ".cf";
static string unload_psr_ext = ".rf";

psrconv::psrconv ()
  : Application ("psrconv", "converts between file formats")
{
  unload = new Unload;
  add( unload );
}

void psrconv::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (output_format, 'o', "format");
  arg->set_help ("name of the output file format");

  arg = menu.add (&editor, &Editor::add_commands, 'c', "command[s]");
  arg->set_help ("one or more edit commands, separated by commas");

  arg = menu.add (&editor, &Editor::add_extensions, 'a', "extension[s]");
  arg->set_help ("one or more extensions to be added, separated by commas");
}

//
//
//
void psrconv::process (Pulsar::Archive* archive)
{
  // ensure that the selected output format can be written
  Reference::To<Pulsar::Archive> output;
  output = Pulsar::Archive::new_Archive (output_format);  
  if (!output->can_unload())
    throw Error (InvalidParam, "psrconv",
		 output_format + " unload method not implemented");

  if (verbose)
    cerr << "Converting " << archive->get_filename() << endl;

  output-> copy (*archive);

  if (verbose)
    cerr << "Conversion complete" << endl;
    
  if (editor.will_modify())
    editor.process (output);

  if (verbose)
  {
    cerr << "Source: " << output -> get_source() << endl;
    cerr << "Frequency: " << output -> get_centre_frequency() << endl;
    cerr << "Bandwidth: " << output -> get_bandwidth() << endl;
    cerr << "# of subints: " << output -> get_nsubint() << endl;
    cerr << "# of polns: " << output -> get_npol() << endl;
    cerr << "# of channels: " << output -> get_nchan() << endl;
    cerr << "# of bins: " << output -> get_nbin() << endl;
  }

  unload->unload (output);
}



//! Unload the archive
void psrconv::Unload::unload (Pulsar::Archive* archive)
{
  bool unload_setup = get_unload_setup();

  if (!unload_setup)
  {
    // use the default output filename extension
    if (archive->type_is_cal())
      set_extension( unload_cal_ext );
    else
      set_extension( unload_psr_ext );
  }

  UnloadOptions::finish (archive);

  if (!unload_setup)
  {
    /*
      reset the extension to an empty string so that get_unload_setup
      will correctly return true the next time around.
    */
    set_extension( string() );
  }
}
