/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "strutil.h"

using namespace std;

//! Pulsar Archive Zapping application
class psrsplit: public Pulsar::Application
{
public:

  //! Default constructor
  psrsplit ();

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  unsigned nsubint;

};

int main (int argc, char** argv)
{
  psrsplit program;
  return program.main (argc, argv);
}

psrsplit::psrsplit ()
  : Pulsar::Application ("psrsplit", "splits an archive into multiple files")
{
  version = "$Id: psrsplit.C,v 1.4 2009/09/02 02:54:31 straten Exp $";
  nsubint = 0;
}

void psrsplit::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (nsubint, 'n', "nsubint");
  arg->set_help ("number of sub-integrations per output file");
}

void psrsplit::setup ()
{
  if (nsubint == 0)
    throw Error (InvalidState, "psrsplit::setup",
		 "please specify number of sub-integrations per output file");
}

string get_extension (const std::string& filename)
{
  string::size_type index = filename.find_last_of( ".", filename.length() );
  if (index != string::npos)
    return filename.substr (index);
  else
    return "";
}

void psrsplit::process (Pulsar::Archive* archive)
{
  unsigned nsub = archive->get_nsubint();

  unsigned isplit = 0;
  unsigned isub=0;

  while( isub < nsub )
  {
    cerr << "psrsplit: extracting";
    vector<unsigned> subints;
    for (unsigned jsub=0; jsub < nsubint && isub < nsub; jsub++)
    {
      cerr << " " << isub;
      subints.push_back( isub );
      isub ++;
    }
    cerr << endl;

    Reference::To<Pulsar::Archive> sub_archive = archive->extract(subints);

    // delete the subintegrations that have been cloned in sub_archive
    for (unsigned isub=0; isub < subints.size(); isub++)
      delete archive->get_Integration (isub);

    string filename = archive->get_filename();
    string ext = stringprintf ("%04d", isplit) + get_extension (filename);
    filename = replace_extension( filename, ext );

    cerr << "psrsplit: writing " << filename << endl;

    sub_archive->unload( filename );

    isplit ++;
  }
}
