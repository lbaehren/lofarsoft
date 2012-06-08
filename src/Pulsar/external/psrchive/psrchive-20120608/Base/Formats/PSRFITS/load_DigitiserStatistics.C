/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Pulsar.h"
#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserStatistics.h"
#include <Error.h>

#include "psrfitsio.h"
#include "tostring.h"



using namespace std;
using namespace Pulsar;



void load_dig_stats_rows( fitsfile *fptr, DigitiserStatistics *ext )
{
  long num_rows;
  int status = 0;

  if( !ext )
    throw Error( InvalidPointer, "load_dig_stats_rows", "called with null ext" );

  fits_get_num_rows( fptr, &num_rows, &status );

  if( status )
    throw FITSError( status, "load_dig_stats_rows", "fits_get_num_rows DIG_STAT" );

  int ndigr = ext->get_ndigr();
  int npar = ext->get_npar();
  int ncycsub = ext->get_ncycsub();

  ext->rows.resize( num_rows );
  for( int r = 0; r < num_rows; r ++ )
  {
    // read in the ATTEN value
    // We may get an exception if the table doesn't contain the correct (ndigr) number of values.
    // psrchive hasn't saved ATTEN correctly for some time so there are a lot of files around in
    // that situation. I'll just fill those attenuation values with zeroes.
    try
    {
      ext->rows[r].atten.resize( ndigr );
      psrfits_read_col( fptr, "ATTEN", ext->rows[r].atten, r+1 );
    }
    catch( Error e )
    {
      if( Archive::verbose > 1 )
        cerr << "load_dig_stats_rows: Failed to read in ATTEN values, filling with zeroes" << endl;
      for( int i = 0; i < ndigr; i ++ )
	ext->rows[r].atten[i] = 0.0;
    }

    // read in the DATA vector
    ext->rows[r].data.resize( ndigr * npar * ncycsub );
    psrfits_read_col( fptr, "DATA", ext->rows[r].data, r+1 );
    ext->rows[r].index = r+1;
  }
}


void Pulsar::FITSArchive::load_DigitiserStatistics (fitsfile* fptr) try
{
  int status = 0;

  if (verbose > 2)
    cerr << "FITSArchive::load_digistat entered" << endl;

  // Move to the DIG_STAT HDU

  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_STAT", 0, &status);

  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_digistat no DIG_STAT HDU" << endl;

    return;
  }

  Reference::To<DigitiserStatistics> dstats = new DigitiserStatistics();

  if( !dstats )
    throw Error( Undefined, "load_DigitiserStatistics", "Failed to create DigitiserStatistics extension" );

  string s_data;

  // load the DIGLEV from HDU
  psrfits_read_key( fptr, "DIGLEV", &s_data );
  dstats->set_diglev( s_data );

  long temp_long = 0;

  // load the NPAR from HDU
  try
  {
    psrfits_read_key( fptr, "NPAR", &temp_long );
  }
  catch( Error& e )
  {
    psrfits_read_key( fptr, "NLEV", &temp_long );
  }
  dstats->set_npar( temp_long );

  // load the NCYCSUB from the HDU
  psrfits_read_key( fptr, "NCYCSUB", &temp_long );
  dstats->set_ncycsub( static_cast<int>(temp_long) );

  // load the NDIGR from HDU
  psrfits_read_key( fptr, "NDIGR", &temp_long);
  dstats->set_ndigr( static_cast<int>(temp_long) );

  // load the DIG_MODE
  psrfits_read_key (fptr, "DIG_MODE", &s_data );
  dstats->set_dig_mode( s_data );

  load_dig_stats_rows( fptr, dstats );

  if (dstats->rows.size() > 0)
    add_extension (dstats);

  if (verbose > 2)
    cerr << "FITSArchive::load_digistat exiting" << endl;
}
 catch (Error& error)
   {
     if (verbose > 2)
        cerr << "FITSArchive::load_DigitiserStatistics error " 
             << error.get_message() << endl;
   }

