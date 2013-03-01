/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"

#include "Pulsar/TextParameters.h"
#include "load_text.h"

#include "fitsio_tempo.h"
#include "psrephem.h"
#include "ephio.h"

using namespace std;

Pulsar::Parameters* load_TextParameters (fitsfile* fptr, bool verbose) try
{
  if (verbose)
    cerr << "load_TextParameters entered" << endl;

  Reference::To<Pulsar::Parameters> param = new Pulsar::TextParameters;

  load_text (fptr, "PSRPARAM", "PARAM", param.get(), verbose);

  return param.release();
}
catch (Error& error)
{
  if (verbose)
    cerr << "load_TextParameters " << error.get_message() << endl;
  return 0;
}

void Pulsar::FITSArchive::load_Parameters (fitsfile* fptr) try
{
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Parameters try PSRPARAM" << endl;

  ephemeris = load_TextParameters (fptr, verbose > 2);

  if (ephemeris)
    return;

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Parameters try PSREPHEM" << endl;

  int status = 0;

  // Load the ephemeris from the FITS file
  fits_movnam_hdu (fptr, BINARY_TBL, "PSREPHEM", 0, &status);

  if (status == 0) try
  {
    Legacy::psrephem* eph = new Legacy::psrephem;

    ::load (fptr, eph);

    set_dispersion_measure( eph->get_double(EPH_DM) );
    set_rotation_measure( eph->get_double(EPH_RM) );

    ephemeris = eph;

    if (verbose > 2)
      cerr << "FITSArchive::load_Parameters ephemeris loaded" << endl;

    return;
  }
  catch (Error& error)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_Parameters load PSREPHEM failed "
           << error.get_message() << endl;
  }

  ephemeris = 0;
  set_dispersion_measure (0);
  set_rotation_measure (0);

  if (verbose > 2)
    cerr << "FITSArchive::load_Parameters no ephemeris" << endl;
}
catch (Error& error)
{
  throw error += "FITSArchive::load_Parameters";
}

