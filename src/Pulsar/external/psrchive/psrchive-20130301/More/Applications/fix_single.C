/***************************************************************************
 *
 *   Copyright (C) 2011 by Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// TODO only one subint per input archive supported

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/TimeAppend.h"

#include "string.h"
#include "strutil.h"
#include "stringtok.h"

#include "MJD.h"

using namespace std;
using namespace Pulsar;

class fix_single : public Pulsar::Application
{
public:
  //! Default constructor:
  fix_single ();

  //! Process the given archive
  void process (Archive*);

  //! Initial setup
  void setup ();

  //! Finalize
  void finalize ();

protected:

  void add_options (CommandLine::Menu&);

  //! buffer size
  bool buffer_filled;

  //! total number of output files
  unsigned long i_subint_global;

  //! the buffer archive
  Reference::To<Archive> buffer;

  //! the output archive
  Reference::To<Archive> output;


  // auxillary variables
  unsigned nbin;
  unsigned nchan;
  unsigned npol;
  float *amps_old_second;

  // IO
  stringstream outss, inss;
  string out_name, ext, input_name, previous_name;

  unsigned long previous_count, input_count;
};

fix_single::fix_single()
	: Application ("fix_single", "fix the single pulse archives by rotating them by half a turn")
{
  ext = "fix";
}

void fix_single::add_options ( CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add("");

  arg = menu.add (ext, 'e', "extension");
  arg->set_help ("Write new files with this extension");
}

void fix_single::process (Archive* archive)
{
  if ( !buffer_filled )
  {
    nbin = archive->get_nbin ();
    nchan = archive->get_nchan ();
    npol = archive->get_npol ();
  }

  input_name = basename ( archive->get_filename () );
  inss << input_name.substr( input_name.length() - 14, 11 );
  inss >> input_count;
  inss.clear();
  inss.str ("");

   if ( archive->get_nsubint() != 1 )
  {
    throw Error ( InvalidState, "fix_single::process", "multiple subints not supported" ) ;
  } 

  if ( input_count != previous_count + 1 )
    buffer_filled = false;
  
  if ( buffer_filled )
  {
    output = archive->clone ();
  }

  if ( amps_old_second == NULL )
  {
    amps_old_second = new float [ nbin * nchan * npol / 2 ];
  }

  //currently only one subint input supported
  unsigned i_subint = 0;
  // read two subints at a time, get the amps, rotate etc
  try
  {
    for (unsigned i_chan = 0; i_chan < nchan; i_chan++ )
    {
      for (unsigned i_pol = 0; i_pol < npol; i_pol++ )
      {
	// index of the element containing the i_subint, i_pol, i_chan
	unsigned index = i_subint * nbin * nchan * npol + i_chan * nbin * npol + i_pol * nbin;
	if (buffer_filled)
	{
	  // copy half of one profile into beginning of the output profile and fill the other half with data from the next profile. To preserve alignement of pulsar peak with zero phase the two halfs are swapped.
	  memcpy ( (void *) (output->get_Profile(i_subint, i_pol, i_chan)->get_amps() ), (void *) (archive->get_Profile(i_subint, i_pol, i_chan)->get_amps() ), nbin / 2 * sizeof ( float ) );
	  memcpy ( (void *) ( output->get_Profile(i_subint, i_pol, i_chan)->get_amps() + nbin / 2 ), (void *) ( amps_old_second + index / 2 ), nbin / 2 * sizeof ( float ) );
	}
	//fill the "previous archive" buffer
	memcpy ( (void *) ( amps_old_second + index / 2 ), (void *) ( archive->get_Profile (i_subint, i_pol, i_chan)->get_amps() + nbin / 2), nbin / 2 * sizeof ( float ) );
	previous_count = input_count;
      }
    }
    //save output after adjusting MJD by half a turn
    if ( buffer_filled == true )
    {
      MJD epoch = archive->get_Integration(i_subint)->get_epoch() ;
      epoch -= archive->get_Integration(i_subint)->get_folding_period() / 2;
      output->get_Integration(i_subint)->set_epoch(epoch);
      outss << "pulse_" << input_count << "." << ext;
      out_name = outss.str();
      outss.str ("");
      output->unload ( out_name );
    }
    buffer_filled = true;
  }
  catch (Error& error)
  {
    cerr << "fix_single::process Problem occured with " << archive->get_filename() << endl;
    cerr << error << endl;
    buffer_filled = false;
  }

}

void fix_single::setup ()
{
  previous_count = 0;
  input_count = 0;
  buffer_filled = false;
  amps_old_second = NULL;
}

void fix_single::finalize ()
{
  if (amps_old_second != NULL )
    delete[] amps_old_second;
}

int main ( int argc, char** argv )
{
  fix_single program;
  return program.main (argc, argv);
}
