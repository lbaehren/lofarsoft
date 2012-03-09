//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FITSArchive.h"
#include "Pulsar/FITSSUBHdrExtension.h"
#include "psrfitsio.h"
#include <iostream>



using std::cerr;
using std::endl;
using std::cout;
using std::string;
using Pulsar::FITSSUBHdrExtension;
using Pulsar::FITSArchive;



void FITSArchive::load_FITSSUBHdrExtension ( fitsfile *fptr )
{
  int status = 0;

  if (verbose > 2)
    cerr << "FITSArchive::load_FITSSUBHdrExtension entered" << endl;

  // Move to the SUBINT HDU

  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);

  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_FITSSUBHdrExtension no SUBINT HDU" << endl;

    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_FITSSUBHdrExtension",
                     "fits_movnam_hdu SUBINT");



  // read the header parameters

  int i_data;
  double d_data;
  long l_data;
  string s_data, s_default="";

  Reference::To< FITSSUBHdrExtension > si_hdr = new FITSSUBHdrExtension();

  psrfits_read_key( fptr, "INT_UNIT", &s_data, s_default, verbose == 3  );
  si_hdr->set_int_unit( s_data );

  psrfits_read_key( fptr, "INT_TYPE", &s_data, s_default, verbose == 3 );
  si_hdr->set_int_type( s_data );

  try {
    // Try to read TBIN
    psrfits_read_key( fptr, "TBIN", &d_data );
  } catch (FITSError &e) {
    // If that fails, look for TSAMP
    psrfits_read_key( fptr, "TSAMP", &d_data, 0.0, verbose == 3 );
  }
  si_hdr->set_tsamp( d_data );

  psrfits_read_key( fptr, "NBITS", &i_data, -1, verbose == 3 );
  si_hdr->set_nbits( i_data );

  psrfits_read_key( fptr, "NCH_STRT", &i_data, -1, verbose == 3 );
  si_hdr->set_nch_strt( i_data );

  psrfits_read_key( fptr, "NSBLK", &i_data, -1, verbose == 3 );
  si_hdr->set_nsblk( i_data );

  fits_get_num_rows( fptr, &l_data, &status );
  si_hdr->set_nrows( l_data );

  psrfits_read_key( fptr, "ZERO_OFF", &d_data, 0.0, verbose == 3 );
  si_hdr->set_zero_off( d_data );

  psrfits_read_key( fptr, "SIGNINT", &i_data, 0, verbose == 3 );
  si_hdr->set_signint( i_data );

  add_extension( si_hdr );
}
