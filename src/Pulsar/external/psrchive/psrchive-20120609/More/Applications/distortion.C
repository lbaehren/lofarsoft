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

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&) {}
};


distortion::distortion ()
  : Application ("distortion", "distortion psrchive program")
{
  add( new Pulsar::StandardOptions );
}

void distortion::process (Pulsar::Archive* archive)
{
  Pulsar::Distortion d;
  d.set_calibrator( archive );
}

static distortion program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

