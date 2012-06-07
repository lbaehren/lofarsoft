/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserCounts.h"
#include "psrfitsio.h"
#include <tostring.h>
#include <Error.h>
#include <templates.h>
#include <fstream>




using Pulsar::DigitiserCounts;
using Pulsar::FITSArchive;
using std::cerr;
using std::cout;
using std::endl;
using std::string;




/**
 * ExtractCounts     Extract the data from the vector of ints given, scale them and add the offset before
 *                   putting the new value into the vector of longs.
 *
 * @param src_data   The vector of ints (as it appears in a fits file )
 * @param data       The vector of longs with the actual count values
 * @param scale      The scale factor for the data
 * @param offset     The offset to add to the scaled src data.
 **/

void ExtractCounts( const vector<int> &src_data, vector<long> &data, const float scale, const float offset  )
{
  data.resize( src_data.size() );

  for( unsigned d = 0; d < src_data.size(); d ++ )
  {
    data[d] = (long int) floor( float(src_data[d]) * scale + offset + 0.5f );
  }
}




/**
 * LoadKeys              Load the header parameters from the DIG_CNTS table and enter them into
 *                       the DigitiserCounts extension given.
 *
 * @param fptr           The fitsfile to load from
 * @param counts         The extension to store the parameters in
 **/

void LoadKeys( fitsfile *fptr, Reference::To<DigitiserCounts> counts )
{
  string s_data;

  psrfits_read_key( fptr, "DYN_LEVT", &s_data );
  if( s_data != "*" )
    counts->set_dyn_levt( fromstring<float>( s_data ) );

  psrfits_read_key( fptr, "DIG_MODE", &s_data );
  counts->set_dig_mode( s_data );

  psrfits_read_key( fptr, "NLEV", &s_data );
  if( s_data == "*" )
    counts->set_nlev( 0 );
  else
    counts->set_nlev( fromstring<unsigned int>( s_data ) );

  psrfits_read_key( fptr, "NPTHIST", &s_data );
  if( s_data == "*" )
    counts->set_npthist( 0 );
  else
    counts->set_npthist( fromstring<unsigned int>( s_data ) );

  psrfits_read_key( fptr, "DIGLEV", &s_data );
  counts->set_diglev( s_data );

  psrfits_read_key( fptr, "NDIGR", &s_data );
  counts->set_ndigr( fromstring<int>( s_data ) );
}




/**
 * LoadCounts        Load the DATA column from the DIG_CNTS table into the DigitiserCounts extension
 * 
 * @param fptr       The file to load from, already set to the DIG_CNTS hdu
 * @param counts     The DigitiserCounts extension to save the table to
 **/

void LoadCounts( fitsfile *fptr, Reference::To<DigitiserCounts> counts )
{
  // load the rows of data
  int status = 0;
  long num_rows;
  fits_get_num_rows( fptr, &num_rows, &status );
  if( status )
    throw FITSError( status, "LoadCounts", "Failed to get num rows from DIG_CNTS table" );

  int row_length = counts->get_npthist() * counts->get_ndigr();
  if( row_length <= 0 )
    throw( Error( InvalidParam, "Pulsar::FITSArchive::load_DigitiserCounts", "npthist or ndigr not correct" ) );

  // Adjust the data using the scale and offset to get the real value
  counts->subints.resize( num_rows );
  for( int s = 0; s < num_rows; s ++ )
  {
    float offset, scale;

    psrfits_read_col( fptr, "DAT_OFFS", &offset, s+1, 0.0f );
    psrfits_read_col( fptr, "DAT_SCL", &scale, s+1, 0.0f );

    vector<int> int_data( row_length );
    psrfits_read_col( fptr, "DATA", int_data, s+1, 0 );

    ExtractCounts( int_data, counts->subints[s].data, scale, offset );
  }
}


/**
 * FITSArchive::load_DigitiserCounts   Creates a DigitiserCounts extension and loads it from
 *                                     the fits file given.
 *
 * @param fptr                         The fitsfile to load the extension from.
 **/

void FITSArchive::load_DigitiserCounts (fitsfile* fptr)
{
  try
  {
    psrfits_move_hdu( fptr, "DIG_CNTS" );

    Reference::To< DigitiserCounts > ext = new DigitiserCounts();

    if( !ext )
      throw Error( BadAllocation,
                   "Pulsar::FITSArchive::load_DigitiserCounts",
                   "failed to allocate DigitiserCounts" );

    LoadKeys( fptr, ext );

    LoadCounts( fptr, ext );

    add_extension( ext );
  }
  catch( Error e )
  {
    // If any problem occured, we simply fail to load the extension.
    if( Archive::verbose > 2 )
      cerr << "FITSArchive::load_DigitiserCounts failed with error" << e << endl;
  }
}






