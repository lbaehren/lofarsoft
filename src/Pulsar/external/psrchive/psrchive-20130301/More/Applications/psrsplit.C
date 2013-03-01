/***************************************************************************
 *
 *   Copyright (C) 2009-2011 by Willem van Straten, Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Dispersion.h"
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
  unsigned nchannel;

  bool resize_extensions;
};

int main (int argc, char** argv)
{
  psrsplit program;
  return program.main (argc, argv);
}

psrsplit::psrsplit ()
  : Pulsar::Application ("psrsplit", "splits an archive into multiple files")
{
  version = "$Id: psrsplit.C,v 1.5 2011/03/08 02:54:31 sosl Exp $";
  nsubint = 0;
  nchannel = 0;
  resize_extensions = false;
}

void psrsplit::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (nsubint, 'n', "nsubint");
  arg->set_help ("number of sub-integrations per output file");

  arg = menu.add (nchannel, 'c', "nchannel");
  arg->set_help ("number of channels per output file\n"
		 "\t\tBy default all the channels are kept");

  arg = menu.add (resize_extensions, 'e');
  arg->set_help ("Attempt to strip the extensions in the output archives\n"
		  "\t\tWarning: this is almost only useful for reducing the size of output archives\n"
		  "\t\tbut can render the extensions meaningless");
}

void psrsplit::setup ()
{
  if (nsubint == 0 && nchannel == 0)
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
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  if ( nsubint > nsub )
    throw Error (InvalidState, "psrsplit::process",
		    "More sub-integrations per output file requested than available in "+archive->get_filename());

  if ( nchannel > nchan )
    throw Error (InvalidState, "psrsplit::process",
		    "More channels per output file requested than available in "+archive->get_filename());

  unsigned isplit = 0;
  unsigned ichansplit = 0;
  unsigned isub=0;
  unsigned ichan=0;

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
    Reference::To<Pulsar::Archive> sub_chan_archive;

    // delete the subintegrations that have been cloned in sub_archive
    for (unsigned isub=0; isub < subints.size(); isub++)
      delete archive->get_Integration (isub);

    // resize extensions to avoid bloating of the disk space used
    if ( resize_extensions )
      for ( unsigned iext = 0; iext < sub_archive->get_nextension (); iext++ )
	sub_archive->get_extension ( iext )->resize( nsubint );

    while ( ichan < nchan )
    {
      // prepare for channel separation
      sub_chan_archive = sub_archive->clone ();

      if ( nchannel > 0 )
      {
	Pulsar::Dispersion correction;
	bool was_dedispersed = sub_chan_archive->get_dedispersed () ;
	if ( was_dedispersed )
	  correction.revert (sub_chan_archive);

	ichan += nchannel ;

	if ( ichan < nchan )
	  sub_chan_archive->remove_chan (ichan, nchan - 1);
	if ( ichan - nchannel > 0 )
	  sub_chan_archive->remove_chan (0, ichan - nchannel - 1 );
	if ( was_dedispersed )
	  sub_chan_archive->dedisperse ();

	sub_chan_archive->update_model ();
      }

      string ext;
      string filename = archive->get_filename();
      if ( nchannel == 0)
      {
	ext = stringprintf ("%04d", isplit) + get_extension (filename);
	ichan = nchan;
      }
      else 
	ext = stringprintf ("%04d_%04d", ichansplit, isplit) + get_extension (filename);

      filename = replace_extension( filename, ext );

      cerr << "psrsplit: writing " << filename << endl;

      sub_chan_archive->unload ( filename );

      ichansplit++;
    }
    ichan = 0;
    ichansplit = 0;

    isplit ++;
  }
}
