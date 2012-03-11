/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"

//
//! An example of an application
//
class example : public Pulsar::Application
{
public:

  //! Default constructor
  example ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&) {}
};


/*!
  The constructor must set the name of the application and a short
  description of its purpose.  These are shown when the user types
  "example -h"

  This constructor also makes use of the StandardOptions class,
  an add-on that provides standard preprocessing with the pulsar
  command language interpreter.

  Other option sets include the CommonOptions class and the
  PlottingOptions class.
*/

example::example ()
  : Application ("example", "example psrchive program")
{
  add( new Pulsar::StandardOptions );
}

/*!

  This example simply loads every profile into memory.

  If you want to use the Profile class, then you will have to

  #include "Pulsar/Profile.h"

*/
void example::process (Pulsar::Archive* archive)
{
  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  
  for (unsigned isub=0; isub < nsub; isub++)    
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
	Pulsar::Profile* profile = archive->get_Profile (isub, ipol, ichan);
}

static example program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

