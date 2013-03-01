/***************************************************************************
 *
 *   Copyright (C) 2008 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FITSSUBHdrExtension.h"



using Pulsar::FITSSUBHdrExtension;



FITSSUBHdrExtension::Interface::Interface ( FITSSUBHdrExtension *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &FITSSUBHdrExtension::get_int_type, "int_type", "Time axis (TIME, BINPHSPERI, BINLNGASC, etc)" );
  add( &FITSSUBHdrExtension::get_int_unit, "int_unit", "Unit of time axis (SEC, PHS (0-1), DEG)" );
  add( &FITSSUBHdrExtension::get_tsamp, "tsamp", "[s] Sample interval for SEARCH-mode data" );
  add( &FITSSUBHdrExtension::get_nbits, "nbits", "Nr of bits/datum (SEARCH mode 'X' data, else 1)" );
  add( &FITSSUBHdrExtension::get_nch_strt, "nch_strt", "Start channel/sub-band number (0 to NCHAN-1)" );
  add( &FITSSUBHdrExtension::get_nsblk, &FITSSUBHdrExtension::set_nsblk, "nsblk", "Samples/row (SEARCH mode, else 1)" );
  add( &FITSSUBHdrExtension::get_nrows, "nrows", "Nr of rows in subint table (search mode)" );
  add( &FITSSUBHdrExtension::get_zero_off, "zero_off", "Zero offset for SEARCH-mode data" );
  add( &FITSSUBHdrExtension::get_signint, "signint", "1 for signed ints in SEARCH-mode data, else 0" );
}



