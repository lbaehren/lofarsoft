/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserCounts.h"
#include <psrfitsio.h>
#include <templates.h>




using Pulsar::DigitiserCounts;
using Pulsar::FITSArchive;
using Pulsar::Archive;
using std::endl;
using std::cerr;




/**
 * CompressCounts     This function takes a vector of long values and compresses the data into a
 *                    vector of integers determining the scale and offset to be used to reconstruct
 *                    the original data.
 *
 * @param data        The vector of actual counts that we are compressing
 * @param target_data The vector to place the compressed data in
 * @param scale       Where to store the scale that was used for compression
 * @param offset      Where to store the offset that was used for compression.
 */

void CompressCounts( const vector<long> &data, vector<int> &target_data, float &scale, float &offset )
{
  // get the range of vlues
  long min, max;
  minmax( data, min, max );

  // determine the scale and offset
  float range = max - min;
  if (max==0 && min==0)
  {
    scale = 1.0;
    offset = 0.0;
  }
  else {
    if (range==0)
      scale = max / 65534.0f;
    else
      scale = range / 65534.0f;
    offset = min - ( -32768.0f * scale );
  }

  // adjust the target data
  target_data.resize( data.size() );
  for( unsigned d = 0; d < data.size(); d ++ )
  {
    target_data[d] = (int) floor( ( float(data[d]) - offset ) / scale + 0.5f);
  }
}




/**
 * UnloadCountsTable  This function unloads the counts data into the fits file given
 *
 * @param fptr        The fits file to unload into
 * @param const_ext   The DigitiserCounts extension to extract the data from
 */

void UnloadCountsTable ( fitsfile *fptr, const DigitiserCounts *ext )
{
  int status = 0;
  fits_insert_rows (fptr, 0, ext->subints.size(), &status);

  if( status != 0 )
    throw FITSError(status, "UnloadCountsTable", "Failed to insert rows" );

  // for each subint
  //   determine the min and max counts
  //   determine the range (max - min)
  //   determine the scale (65534 / range)
  //   determine the offset (min - ( -32768 * scale ) )

  vector<unsigned> dimensions (2);
  dimensions[0] = ext->get_npthist();
  dimensions[1] = ext->get_ndigr();

  int num_subints = ext->subints.size();
  for( int s = 0; s < num_subints; s ++ )
  {
    vector<int> int_data;
    float scale;
    float offset;

    CompressCounts( ext->subints[s].data, int_data, scale, offset );

    psrfits_write_col( fptr, "DAT_SCL", s+1, scale );
    psrfits_write_col( fptr, "DAT_OFFS", s+1, offset );
    psrfits_write_col( fptr, "DATA", s+1, int_data, dimensions );
  }
}





/**
 * UnloadCountsKeys   Unload the DIG_CNTS header parameters to the fits file given
 *
 * @param fptr        The fits file to unload into
 * @param const_ext   The DigitiserCounts extension to extract the data from
 */

void UnloadCountsKeys( fitsfile *fptr, const DigitiserCounts *ext )
{
  int ndigr = ext->get_ndigr();
  string diglev = ext->get_diglev();
  string dig_mode = ext->get_dig_mode();
  int npthist = ext->get_npthist();
  int nlev = ext->get_nlev();
  float dyn_levt = ext->get_dyn_levt();

  if (Archive::verbose > 2)
  {
    cerr << "saving dyn_levt" << dyn_levt << endl;
    cerr << "saving ndigr " << ndigr << endl;
    cerr << "saving diglev " << diglev << endl;
    cerr << "saving dig_mode " << dig_mode << endl;
    cerr << "saving npthist " << npthist << endl;
    cerr << "saving nlev " << nlev << endl;
  }

  psrfits_update_key( fptr, "DYN_LEVT", dyn_levt );
  psrfits_update_key( fptr, "NDIGR", ndigr );
  psrfits_update_key( fptr, "DIGLEV", diglev );
  psrfits_update_key( fptr, "DIG_MODE", dig_mode );
  psrfits_update_key( fptr, "NPTHIST", npthist );
  psrfits_update_key( fptr, "NLEV", nlev );

  if (Archive::verbose > 2)
    cerr << "digitiser counts keys unloaded" << endl;
}




/**
 * FITSArchive::unload   Unload the DigitierCounts extension to the fits file given
 *
 * @param fptr           The fits file to unload to
 * @param const_ext      The DigitiserCounts extension to unload
 **/

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
    const DigitiserCounts* const_ext ) try
{
  psrfits_move_hdu( fptr, "DIG_CNTS" );

  UnloadCountsKeys( fptr, const_ext );

  UnloadCountsTable( fptr, const_ext );
}
catch (Error &error)
{
  throw error += "FITSArchive::unload(DigitiserCounts)";
}
