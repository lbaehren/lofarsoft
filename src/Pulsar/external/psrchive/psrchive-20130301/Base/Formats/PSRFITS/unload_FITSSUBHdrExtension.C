/***************************************************************************
 *
 *   Copyright (C) 2008 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FITSArchive.h"
#include <psrfitsio.h>
#include <FITSError.h>
#include <Pulsar/FITSSUBHdrExtension.h>




using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using Pulsar::FITSSUBHdrExtension;





void Pulsar::FITSArchive::unload ( fitsfile *fptr, const FITSSUBHdrExtension *sub_hdr )
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload (fitsfile*,const FITSSUBHdrExtension*) entered" << endl;

  // Move to the SUBINT HDU

  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload(fitsfile*,const FITSSUBHdrExtension*)",
                     "fits_movnam_hdu SUBINT");

  // Write out the variables we know.

  string s_data;
  double d_data;
  int i_data;

  s_data = sub_hdr->get_int_unit();
  if( !s_data.empty() ) psrfits_update_key( fptr, "INT_UNIT", s_data );

  s_data = sub_hdr->get_int_type();
  if( !s_data.empty() ) psrfits_update_key( fptr, "INT_TYPE", s_data );

  d_data = sub_hdr->get_tsamp();
  if( d_data != 0.0 ) psrfits_update_key( fptr, "TBIN", d_data );

  d_data = sub_hdr->get_zero_off();
  if( d_data != 0.0 ) psrfits_update_key( fptr, "ZERO_OFF", d_data );

  i_data = sub_hdr->get_nbits();
  if( i_data != -1 ) psrfits_update_key( fptr, "NBITS", i_data );

  i_data = sub_hdr->get_nch_strt();
  if( i_data != -1 ) psrfits_update_key( fptr, "NCH_STRT", i_data );

  i_data = sub_hdr->get_nsblk();
  if( i_data != -1 ) psrfits_update_key( fptr, "NSBLK", i_data );

  i_data = sub_hdr->get_signint();
  if( i_data != -1 ) psrfits_update_key( fptr, "SIGNINT", i_data );
}

