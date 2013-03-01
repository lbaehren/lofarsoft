/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Distortion.h"

using namespace Pulsar;

//
//! An distortion of an application
//
class distortion : public Pulsar::Application
{
public:

  //! Default constructor
  distortion ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Set the name of the standard (template) that will be cloned and distorted
  void set_standard (const std::string&);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  Reference::To<Archive> standard;
};


distortion::distortion ()
  : Application ("distortion", "distortion psrchive program")
{
  add( new Pulsar::StandardOptions );
}

void distortion::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (this, &distortion::set_standard, 's', "filename");
  arg->set_help ("standard/template to be distorted");
}

void distortion::set_standard (const std::string& filename)
{
  standard = Archive::load( filename );
}

void distortion::process (Pulsar::Archive* archive)
{
  Pulsar::Distortion d;
  if (standard)
    d.set_standard( standard );
  d.set_calibrator( archive );
}

static distortion program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

