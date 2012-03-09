/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "psrfitsio.h"

#include <unistd.h>

using namespace std;

static string get_version ()
{
  /*
    Create a temporary PSRFITS file using the template and HDRVER from it
  */

  string version = "unknown";

  char temporary_name[64] = "/tmp/psrfits.tmp.XXXXXX";

  if (mkstemp (temporary_name) < 0)
    return version;

  string clobber = "!" + string(temporary_name);

  string name = Pulsar::FITSArchive::get_template_name();
  
  int status = 0;
  fitsfile* fptr = 0;

  fits_create_template (&fptr, clobber.c_str(), name.c_str(), &status);
  if (status) {
    char error[FLEN_ERRMSG];
    fits_get_errstatus (status, error);
    cerr << "FITSArchive::Agent::get_description fits_open_file: " 
	 << error << endl;
    return version;
  }

  psrfits_read_key (fptr, "HDRVER", &version, version, 
		    Pulsar::Archive::verbose == 3);

  fits_close_file (fptr, &status);
  remove (temporary_name);

  return version;
}


string Pulsar::FITSArchive::Agent::get_description ()
{
  static string version;

  if (version.empty())
    version = get_version ();

  return "PSRFITS version " + version;
}

// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////
/*! The method tests whether or not the given file is of FITS type. */
bool Pulsar::FITSArchive::Agent::advocate (const char* filename)
{
  fitsfile* test_fptr = 0;
  int status = 0;
  char error[FLEN_ERRMSG];

  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::Agent::advocate test " << filename << endl;

  fits_open_file(&test_fptr, filename, READONLY, &status);

  if (status != 0) {

    if (Archive::verbose == 3) {
      fits_get_errstatus (status, error);
      cerr << "FITSAgent::advocate fits_open_file: " << error << endl;
    }

    return false;
  }

  if (Archive::verbose == 3)
    cerr << "FITSAgent::advocate test reading MJD" << endl;

  bool result = true;

  try {

    // try to read the MJD from the header
    long day;
    long sec;
    double frac;
    
    psrfits_read_key (test_fptr, "STT_IMJD", &day);
    psrfits_read_key (test_fptr, "STT_SMJD", &sec);
    psrfits_read_key (test_fptr, "STT_OFFS", &frac);
    
  }
  catch (Error& error)
  {
    if (Archive::verbose == 3)
      cerr << "FITSAgent::advocate failed to read MJD "
	   << error.get_message() << endl;
    
    result = false;
  }
  
  fits_close_file(test_fptr, &status);
  
  return result;
  
}
