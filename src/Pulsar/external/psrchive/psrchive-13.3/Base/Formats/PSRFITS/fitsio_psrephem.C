/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "psrephem.h"
#include "ephio.h"

#include "FITSError.h"
#include "fitsutil.h"
#include "coord.h"

#include <fitsio.h> 

#include <iostream>
#include <vector>
#include <algorithm>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef sun
#include <ieeefp.h>
#endif

using namespace std;
using Legacy::psrephem;

// utility function defined at the end of the file
void datatype_match (int typecode, int ephind);

// return the FITS TFORM field for the specified EPHIO index
char* eph_tform (int ephind);

/*! This method prepares a mapping between the columns of the PSREPHEM hdu
  and the indeces used internally.

  \pre fptr must refer to a PSRFITS file with a PSREPHEM hdu 
  \post fptr will be left set to the PSREPHEM hdu
*/

void fits_map (fitsfile* fptr, vector<int>& ephind, int& maxstrlen)
{
  int status = 0;          // status returned by FITSIO routines

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "PSREPHEM", 0, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::fits_map", "fits_movnam_hdu");

  // ask for the number of columns in the binary table
  int ncols = 0;
  fits_get_num_cols (fptr, &ncols, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::fits_map", "fits_get_num_cols");

  if (psrephem::verbose)
    cerr << "psrephem::fits_map ncols=" << ncols << endl;

  //
  // construct a mapping between the column number and the ephio index
  //
  ephind.resize (ncols);

  string parstr;

  char keyword [FLEN_KEYWORD+1];
  char value   [FLEN_VALUE+1];
  char comment [FLEN_COMMENT+1];

  maxstrlen = 0;

  string previous;

  for (int icol=0; icol<ncols && status==0; icol++) {

    int  typecode = 0;
    long repeat = 0;
    long width = 0;
    fits_get_coltype (fptr, icol+1, &typecode, &repeat, &width, &status);

    fits_make_keyn ("TTYPE", icol+1, keyword, &status);
    fits_read_key (fptr, TSTRING, keyword, value, comment, &status);

    if (psrephem::verbose)
      cerr << icol+1 << " '" << value << "' repeat=" << repeat 
	   << " width=" << width << " typecode=" << typecode
	   << " comment=" << comment << endl;
    
    parstr = value;

    // standardize the various strings encountered to date
    if ( parstr == "PSR-NAME" || parstr == "PSR_NAME" )
      parstr = "PSRJ";

    if ( parstr == "ECC" )
      parstr = "E";

    //
    // columns containing the fractional component must be preceded by integer
    //
    if ( parstr == "TZRFMJD" ) {
      if (previous != "TZRIMJD")
	throw Error (InvalidState, "psrephem::fits_map",
		     "TZRFMJD does not follow TZRIMJD");
      parstr = "TZRMJD";
    }

    if ( parstr == "FF0" ) {
      if (previous != "IF0")
	throw Error (InvalidState, "psrephem::fits_map",
		     "FF0 does not follow IF0");
      parstr = "F";
    }

    previous = parstr;

    //
    // search for a match
    //
    ephind[icol] = -1;

    for (int ieph=0; ieph<EPH_NUM_KEYS; ieph++)

      if (parstr == parmNames[ieph]) {

	// match found ... 
	// test the validity of operating assumption.  datatype match
	// will display an error if there is a mismatch in assumption
	if (psrephem::verbose)
	  cerr << "psrephem::fits_map column " << icol
	       << " matches ephind " << ieph << " ";

	datatype_match (typecode, ieph);

	ephind[icol] = ieph;

	// check the amount of space required to read
	if (typecode == TSTRING && repeat > maxstrlen)
	  maxstrlen = repeat;
	
	break;

      }

    if (psrephem::verbose && ephind[icol] == -1)
      cerr << "psrephem::fits_map unrecognized parameter='" << parstr << "'"
           << endl;

  }

  if (psrephem::verbose)
    cerr << "psrephem::fits_map exit" << endl;
}






// ///////////////////////////////////////////////////////////////////////
//
// load an ephemeris from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// row      - if specified, parse the row'th record in the binary table
//            otherwise, parse the last (most recent) row
//

void load (fitsfile* fptr, psrephem* ephem, long row)
{
  vector<int> ephind;   // ephio index for column number
  int maxstrlen = 0;

  // when fits_map returns, fptr will be set to the PSREPHEM hdu
  fits_map (fptr, ephind, maxstrlen);

  int status = 0;          // status returned by FITSIO routines

  // ask for the number of rows in the binary table
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::load", "fits_get_num_rows");

  if (row > nrows || row < 1)
    row = nrows;

  if (psrephem::verbose)
    cerr << "psrephem::load PSRFITS nrows=" << nrows << " ncols=" 
	 << ephind.size() << " loading row=" << row << endl;

  // get ready to parse the values
  ephem->zero ();

  char* strval = new char [maxstrlen+1];
  unsigned icol=0;

  for (icol=0; icol<ephind.size() && status==0; icol++) {

    int ieph = ephind[icol];
    if (ieph < 0)
      continue;

    long firstelem = 1;
    long onelement = 1;
    int anynul = 0;

    switch (parmTypes[ieph]) {

    case 0:  // string
      {
	static char* nul = strdup(" ");
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
		       &nul, &strval, &anynul, &status);

        // strip off any leading spaces
        char* start = strtok (strval, " \t\n");
        if (!start)
          anynul = true;

        if (anynul)
          break;

	if (psrephem::verbose)
	  cerr << "psrephem::load string:'" << start << "' in column "
	       << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	ephem->value_str[ieph] = start;
	break;
      }

    case 1:  // double
      {
	double nul = 0.0;

	fits_read_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
		       &nul, ephem->value_double + ieph, &anynul, &status);

        if (ephem->value_double[ieph] == 0.0 || !finite(ephem->value_double[ieph]))
          anynul = true;

        if (anynul)
          break;

	if (psrephem::verbose)
	  cerr << "psrephem::load double:'" << ephem->value_double[ieph] <<
		"' in column " << icol+1 << " (" << parmNames[ieph] << ")"
               << endl;

	if (ieph == EPH_F) {
	  // special case for F: given in mHz
	  // (Note that IF0 is in the column preceding FF0)

          long lnul = 0;
          long lval = 0;
	  fits_read_col (fptr, TLONG, icol, row, firstelem, onelement,
			 &lnul, &lval, &anynul, &status);

          if (anynul)
            break;

	  ephem->value_double[ieph] += double (lval);
          ephem->value_double[ieph] *= 1e-3;
	  ephem->value_integer[ieph] = 0;
	}

        break;
      }
    case 2:  // h:m:s
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
		       nul, &strval, &anynul, &status);

        if (anynul)
          break;

        if (psrephem::verbose)
          cerr << "psrephem::load h:m:s:'" << strval << "' in column "
               << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	if (str2ra (ephem->value_double + ieph, strval) != 0) {
	  if (psrephem::verbose)
	    cerr << "psrephem::load PSRFITS could not parse h:m:s from '" 
		 << strval << "'" << endl;
	  anynul = 1;
	}

	// convert from radians to turns
	ephem->value_double[ieph] /= 2.0 * M_PI;

	break;
      }
    case 3:  // d:m:s
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
		       nul, &strval, &anynul, &status);

        if (anynul)
          break;

        if (psrephem::verbose)
          cerr << "psrephem::load d:m:s:'" << strval << "' in column "
               << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	if (str2dec2 (ephem->value_double + ieph, strval) != 0) {
	  if (psrephem::verbose)
	    cerr << "psrephem::load PSRFITS could not parse d:m:s from '" 
		 << strval << "'" << endl;
	  anynul = 1;
	}

	// convert from radians to turns
	ephem->value_double[ieph] /= 2.0 * M_PI;

	break;
      }
    case 4:  // MJD
      {
	double nul = 0.0;

	fits_read_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
		       &nul, ephem->value_double + ieph, &anynul, &status);

        if (ephem->value_double[ieph] == 0.0 || !finite(ephem->value_double[ieph]))
          anynul = true;

        if (anynul)
          break;

	if (ieph == EPH_TZRMJD) {
	  int inul = 0;
	  // special case for TZRMJD:
	  // Assumes that TZRIMJD is in the column preceding TZRFMJD
	  fits_read_col (fptr, TINT, icol, row, firstelem, onelement,
			 &inul, ephem->value_integer + ieph, &anynul, &status);
          if (anynul)
            break;

	}
	else {
	  // separate the integer
	  ephem->value_integer[ieph] = int (ephem->value_double[ieph]);  // truncate
	  ephem->value_double[ieph] -= ephem->value_integer[ieph];
	}
	break;
      }
    case 5:  // integer
      {
	int nul = -1;
	fits_read_col (fptr, TINT, icol+1, row, firstelem, onelement,
		       &nul, ephem->value_integer + ieph, &anynul, &status);

        if (ephem->value_integer[ieph] == 0)
          anynul = true;

        if (anynul)
          break;

        if (psrephem::verbose)
          cerr << "psrephem::load integer:'" << ephem->value_integer[ieph] <<
                "' in column " << icol+1 << " (" << parmNames[ieph] << ")" 
               << endl;

	break;
      }
    default:
      delete [] strval;
      throw Error (InvalidState, "psrephem::load",
		   "invalid parmType[%d]", ieph);
    }

    if (status != 0)  {
      delete [] strval;
      throw FITSError (status, "psrephem::load", 
		       "error parsing %s", parmNames[ieph]);
    }

    if (anynul)  {
      if (psrephem::verbose)
        cerr << "psrephem::load null in column " << icol+1 
             << " (" << parmNames[ieph] << ")" << endl;

      ephem->parmStatus[ieph] = 0;
    }
    else
      ephem->parmStatus[ieph] = 1;

  }

  delete [] strval;

  if (icol < ephind.size())
    cerr << "psrephem::load PSRFITS read " << icol <<"/" << ephind.size()
	 << " elements" << endl;

  if (psrephem::verbose)
    cerr << "psrephem::load PSRFITS\n" << *ephem << endl;

}






// ///////////////////////////////////////////////////////////////////////
//
// unload an ephemeris to the PSREPHEM hdu of a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
//
// row      - if specified, unload to the row'th record in the binary table
//            otherwise, create a new row and unload to this one
//
void unload (fitsfile* fptr, const psrephem* ephem, long row)
{
  int status = 0;          // status returned by FITSIO routines
  vector<int> ephind;      // ephio index for column number
  int maxstrlen;

  // when fits_map returns, fptr will be set to the PSREPHEM hdu
  fits_map (fptr, ephind, maxstrlen);

  // check that all of the required columns exist in the table
  for (int ieph=0; ieph < EPH_NUM_KEYS; ieph++)
    if (ephem->parmStatus[ieph] &&
	find (ephind.begin(), ephind.end(), ieph) == ephind.end())  {

      //
      int colnum = ephind.size() + 1;
      char* ttype = parmNames[ieph];
      char* tform = eph_tform (ieph);

      fits_insert_col (fptr, colnum, ttype, tform, &status);

      if (status) throw Error (InvalidState, "psrephem::unload",
        "fits_insert_col (%d,%s,%s) in PSREPHEM hdu", colnum, ttype, tform);

      ephind.push_back (ieph);

    }
  
  // ask for the number of rows in the binary table
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::unload", "fits_get_num_rows");

  if (row > nrows)
    throw Error (InvalidRange, "psrephem::unload",
		 "row=%d > nrows=%d", row, nrows);

  if (row < 1) {
    // row unspecified
    nrows ++;
    row = nrows;

    if (psrephem::verbose)
      cerr << "psrephem::unload adding row " << nrows
	   << " to PSREPHEM hdu" << endl;
  }

  if (psrephem::verbose)
    cerr << "psrephem::unload PSRFITS nrows=" << nrows << " ncols=" 
	 << ephind.size() << " unload to row=" << row << endl;

  char* strval = new char [maxstrlen+1];

  for (unsigned icol=0; icol<ephind.size() && status==0; icol++) {

    int ieph = ephind[icol];
    if (ieph < 0)
      continue;

    long firstelem = 1;
    long onelement = 1;
 
    if (ephem->parmStatus[ieph] == 0) {
      // write null entry

      if (psrephem::verbose)
	cerr << "psrephem::unload null in column " 
	     << icol+1 <<  " (" << parmNames[ieph] << ")" << endl;

      fits_write_col_null (fptr, icol+1, row, firstelem, onelement, &status);
      if (status)
	throw FITSError (status, "psrephem::unload",
          "fits_write_col_null (col=%d=%s, row=%d)",
          icol+1, parmNames[ieph], row);

      continue;

    }

    switch (parmTypes[ieph]) {

    case 0:  // string
      {
	strcpy (strval, ephem->value_str[ieph].c_str());

	if (psrephem::verbose)
	  cerr << "psrephem::unload string:'" << strval << "' in column "
	       << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	fits_write_col (fptr, TSTRING, icol+1, row, 1, 1, &strval, &status);

	break;
      }

    case 1:  // double
      {
	double value = ephem->value_double[ieph];

	if (ieph == EPH_F) {
	  // special case for F: stored in mHz
	  // (Note that IF0 is in the column preceding FF0)

	  value *= 1e3;
	  long lval = long (value);

	  fits_write_col (fptr, TLONG, icol, row, 1, 1, &lval, &status);

	  value -= double (lval);
	}


	if (psrephem::verbose)
	  cerr << "psrephem::unload double:'" << value << "' in column "
	       << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	fits_write_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
			&value, &status);
	break;
      }

    case 2:  // h:m:s
      {
	// convert from turns to radians
        ra2str (strval, maxstrlen, ephem->value_double[ieph]*2.0*M_PI, 10);

	if (psrephem::verbose)
	  cerr << "psrephem::unload h:m:s:'" << strval << "' in column "
	       << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	fits_write_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
			&strval, &status);
	break;
      }
    case 3:  // d:m:s
      {
	// convert from turns to radians
        dec2str2 (strval, maxstrlen, ephem->value_double[ieph]*2.0*M_PI, 10);

	if (psrephem::verbose)
	  cerr << "psrephem::unload d:m:s:'" << strval << "' in column "
	       << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	fits_write_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
			&strval, &status);
	break;
      }
    case 4:  // MJD
      {
	double value = ephem->value_double[ieph];

	if (ieph == EPH_TZRMJD) {
	  // special case for TZRMJD:
	  // Assumes that TZRIMJD is in the column preceding TZRFMJD
	  fits_write_col (fptr, TINT, icol, row, firstelem, onelement,
			  ephem->value_integer + ieph, &status);
	}
	else
	  // add the integer
	  value += ephem->value_integer[ieph];

	if (psrephem::verbose)
	  cerr << "psrephem::unload double:'" << value << "' in column "
	       << icol+1 << " (" << parmNames[ieph] << ")" << endl;

	fits_write_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
			&value, &status);
	break;
      }
    case 5:  // integer
      {

	if (psrephem::verbose)
	  cerr << "psrephem::unload integer:'" <<  ephem->value_integer[ieph]
	       << "' in column " << icol+1 << " (" << parmNames[ieph] << ")"
	       << endl;

	fits_write_col (fptr, TINT, icol+1, row, firstelem, onelement,
			ephem->value_integer + ieph, &status);
	break;
      }

    default:
      delete [] strval;
      throw Error (InvalidState, "psrephem::unload",
		   "invalid parmTypes[%d]=%d", ieph, parmTypes[ieph]);

    }
    
    if (status != 0) {
      delete [] strval;
      throw FITSError (status, "psrephem::unload", "fits_write_col");
    }
  }
  delete [] strval;
}



char* eph_tform (int ephind)
{
  int parmtype = parmTypes[ephind];
  assert (parmtype>=0 && parmtype<6);

  // the FITSIO TFORM corresponding to each parmType
  static char* tform[7] = { "12A",  // string
                            "1D",   // double
                            "24A",  // h:m:s
                            "24A",  // d:m:s
                            "1D",   // MJD
                            "1J" }; // integer

  return tform[parmtype];
}

void datatype_match (int typecode, int ephind)
{
  int parmtype = parmTypes[ephind];
  assert (parmtype>=0 && parmtype<6);

  // the CFITSIO datatype corresponding to each parmType
  int datatype[7] = { TSTRING,  // string
		      TDOUBLE,  // double
		      TSTRING,  // h:m:s
		      TSTRING,  // d:m:s
		      TDOUBLE,  // MJD
		      TINT };   // integer
  
  if (psrephem::verbose)
    cerr << "typecode=" << typecode << " datatype[" << parmtype << "]="
	 << datatype[parmtype] << endl;

  bool match = false;

  // suffer 'I' and 'J' inconsistency
  if (datatype[parmtype] == TINT)
    match = (typecode == TSHORT || typecode == TINT || typecode == TLONG);
  else
    match = (typecode == datatype[parmtype]);

  if (!match)
    throw Error (InvalidState, "psrephem::datatype_match",
		 "PSRFITS binary table column datatype:%s\n\t" 
		 "doesn't match that required:%s\n\t"
                 "for parmType:%d = %s\n", 
		 fits_datatype_str(typecode),
                 fits_datatype_str(datatype[parmtype]),
                 parmtype, parmNames[ephind]);

}
