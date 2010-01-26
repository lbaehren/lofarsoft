/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pointing.h"

using namespace std;

void Pulsar::FITSArchive::clean_Pointing_columns (fitsfile* fptr) const
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    if( get_Integration(isub) -> get<Pointing>() )
    {
      if (verbose > 2)
	cerr << "Pulsar::FITSArchive::clean_Pointing_columns subint="
	     << isub << " has Pointing" << endl;
      return;
    }

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::clean_Pointing_columns"
      " no subints have Pointing" << endl;

  vector<string> columns;

  columns.push_back( "LST_SUB" );
  columns.push_back( "RA_SUB" );
  columns.push_back( "DEC_SUB" );
  columns.push_back( "GLON_SUB" );
  columns.push_back( "GLAT_SUB" );
  columns.push_back( "FD_ANG" );
  columns.push_back( "POS_ANG" );
  columns.push_back( "PAR_ANG" );
  columns.push_back( "TEL_AZ" );
  columns.push_back( "TEL_ZEN" );

  for (unsigned icol = 0; icol < columns.size(); icol++)
  {
    int status = 0;
    int colnum = 0;
    char* key = const_cast<char*>( columns[icol].c_str() );
    fits_get_colnum (fptr, CASEINSEN, key, &colnum, &status);
    fits_delete_col (fptr, colnum, &status);
  }
}
