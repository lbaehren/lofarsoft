/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/ProfileShiftFit.h"

using namespace std;
using namespace Pulsar;

//
//! An application to automatically cull files
//
class cull : public Pulsar::Application
{
public:

  //! Default constructor
  cull ();

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Set the standard profile used to automatically cull
  void set_standard (const std::string& filename);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! Automatically discard profiles that do not fit the template well
  Reference::To<ProfileShiftFit> fit;

  //! Automatic: goodness of fit threshold
  double fit_threshold;
  
  //! Automatic: S/N treshold
  double snr_threshold;

  //! Automatic: phase shift threshold
  double shift_threshold;
};


cull::cull ()
  : Application ("cull", "automatically cull data files")
{
  add( new Pulsar::StandardOptions );

  fit_threshold = 10.0;
  snr_threshold = 10.0;
  shift_threshold = 0.01;
}

void cull::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &cull::set_standard, 's', "std.ar");
  arg->set_help ("use goodness of template fit");

  arg = menu.add (fit_threshold, "fit", "threshold");
  arg->set_help ("goodness of fit threshold (default: 10)");

  arg = menu.add (snr_threshold, "snr", "threshold");
  arg->set_help ("S/N threshold (default: 10)");

  arg = menu.add (shift_threshold, "shift", "threshold");
  arg->set_help ("phase shift threshold (default: 0.01 turns)");
}

//! Set the standard profile used to automatically cull
void cull::set_standard (const std::string& filename)
{
  Reference::To<Archive> std = Pulsar::Archive::load (filename);
  std = std->total();

  fit = new ProfileShiftFit;
  fit->choose_maximum_harmonic = true;
  fit->set_standard (std->get_Profile(0,0,0));
}

//! Verify setup
void cull::setup ()
{
  if (!fit)
    throw Error (InvalidState, "cull::setup"
		 "standard/template archive must be set (with -s)");
}

double absturn (double turn)
{
  return std::min( fabs(turn), fabs(turn-1.0) );
}

void cull::process (Pulsar::Archive* archive)
{
  Reference::To<Archive> total = archive->total();

  fit->set_Profile( total->get_Profile(0,0,0) );

  if (fit_threshold)
  {
    double redchi2 = fit->get_reduced_chisq();
    
    if (verbose)
      cerr << "psrcull: " << archive->get_filename() 
	   << " reduced chisq=" << redchi2 << endl;
    
    if (redchi2 > fit_threshold)
    {
      cout << "trash: " << archive->get_filename()
	   << " fit=" << redchi2 << endl;
      return;
    }
  }
  
  if (snr_threshold)
  {
    double snr = fit->get_snr();
    
    if (verbose)
      cerr << "psrcull: " << archive->get_filename() 
	   << " S/N=" << snr << endl;
    
    if (snr < snr_threshold)
    {
      cout << "trash: " << archive->get_filename()
	   << " S/N=" << snr << endl;
      return;
    }
  }
  
  if (shift_threshold)
  {
    double shift = fit->get_shift().get_value();
    
    if (verbose)
      cerr << "psrcull: " << archive->get_filename() 
	   << " shift=" << shift << endl;
    
    if ( absturn(shift) > shift_threshold )
    {
      cout << "trash: " << archive->get_filename()
	   << " shift=" << shift << endl;
      return;
    }
  }
}


int main (int argc, char** argv)
{
  cull program;
  return program.main (argc, argv);
}

