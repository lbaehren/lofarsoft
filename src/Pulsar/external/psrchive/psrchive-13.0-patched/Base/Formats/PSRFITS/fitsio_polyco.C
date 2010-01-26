/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "fitsio_tempo.h"
#include "fitsutil.h"
#include "FITSError.h"

#include <iostream>
#include <assert.h>

using namespace std;

// #define _DEBUG 1

// ///////////////////////////////////////////////////////////////////////
//
// load a polyco from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// back     - if specified, go back in history  NOT IMPLEMENTED
//

void load (fitsfile* fptr, polyco* model, int back)
{
#ifdef _DEBUG
  cerr << "load polyco PSRFITS 1" << endl;
#endif

  int status = 0;          // status returned by FITSIO routines

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);
  if (status != 0) 
    throw FITSError (status, "load polyco", "fits_movnam_hdu");

  // ask for the number of rows in the binary table
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "load polyco", "fits_get_num_rows");

  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;
  int anynul = 0;

  // ask for the number of coefficient sets belonging to the last polyco
  int npoly = 0;
  fits_get_colnum (fptr, CASEINSEN, "NPBLK", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, nrows, firstelem, onelement,
		 NULL, &npoly, &anynul, &status);
  if (status != 0)
    throw FITSError (status, "load polyco", "fits_read_col NPBLK");

  model->pollys.resize(npoly);

  long row = nrows - npoly + 1;

  for (int ipol=0; ipol<npoly; ipol++)
    load (fptr, &model->pollys[ipol], row+ipol);

}

void load (fitsfile* fptr, polynomial* poly, long row)
{
  // these are used to pull out the data from a cell
  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;
  int anynul = 0;
  char* nul = "-";

  int status = 0;          // status returned by FITSIO routines

#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 2" << endl;
#endif

  int nspan=0;
  fits_get_colnum (fptr, CASEINSEN, "NSPAN", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &nspan, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse NSPAN");

  polynomial::Expert set (poly);

  // set the attribute
  set.set_nspan (nspan);

#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 2.3" << endl;
#endif

  int ncoef=0;
  fits_get_colnum (fptr, CASEINSEN, "NCOEF", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &ncoef, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse NCOEF");

#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 2.4" << endl;
#endif


#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 2.5" << endl;
#endif

  static char* site = new char[2];
  fits_get_colnum (fptr, CASEINSEN, "NSITE", &colnum, &status);
  fits_read_col (fptr, TSTRING, colnum, row, firstelem, onelement,
		 nul, &site, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse NSITE");

  // set the attribute
  set.set_telescope (site[0]);

#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 2.6" << endl;
#endif

  double freq;
  fits_get_colnum (fptr, CASEINSEN, "REF_FREQ", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &freq, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse REF_FREQ");

  // set the attribute
  set.set_freq (freq);

#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 3" << endl;
#endif

  double ref_mjd=0;
  fits_get_colnum (fptr, CASEINSEN, "REF_MJD", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		   NULL, &ref_mjd, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse REF_MJD");

  // set the attribute
  set.set_reftime (ref_mjd);

#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 4" << endl;
#endif

  double ref_phs = 0;
  fits_get_colnum (fptr, CASEINSEN, "REF_PHS", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &ref_phs, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse REF_PHS");

  // set the attribute
  set.set_refphase (ref_phs);
 
  double f0 = 0; 
  fits_get_colnum (fptr, CASEINSEN, "REF_F0", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &f0, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse REF_F0");

  // set the attribute
  set.set_reffrequency (f0);

  //! dispersion measure is not included in POLYCO binary table

  int  typecode = 0;
  long repeat = 0;
  long width = 0;
  
#ifdef _DEBUG
  cerr << "load polynomial PSRFITS 5" << endl;
#endif

  fits_get_colnum (fptr, CASEINSEN, "COEFF", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  
  if (repeat < ncoef)
    throw FITSError (status, "load polynomial COEFF",
	             " vector repeat count=%ld < NCOEF=%d", repeat, ncoef);

  set.set_ncoef (ncoef);

  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, ncoef,
		 NULL, set.get_coefs(), &anynul, &status);

  if (anynul || status)
    throw FITSError (status, "load polynomial failed to parse COEFF");

  set.set_tempov11 (false);
}


// ///////////////////////////////////////////////////////////////////////
//
// unload a polyco to a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// back     - if specified, go back in history  NOT IMPLEMENTED
//
void unload (fitsfile* fptr, const polyco* model, int back)
{
#ifdef _DEBUG
  cerr << "unload polyco PSRFITS 1" << endl;
#endif

  int status = 0;          // status returned by FITSIO routines

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);
  if (status != 0) 
    throw FITSError (status, "unload polyco", "fits_movnam_hdu");

  // ask for the number of rows in the binary table
  long rows = 0;
  fits_get_num_rows (fptr, &rows, &status);
  if (status != 0)
    throw FITSError (status, "unload polyco", "fits_get_num_rows");

  if (back == 0)
    rows ++;
  else {
    cerr << "unload polyco WARNING back not implemented. deleting old rows" 
	 << endl;
    fits_delete_rows (fptr, 1, rows, &status);
    if (status != 0)
      throw FITSError (status, "unload polyco", "fits_delete_rows");
    rows = 0;
  }

  int npoly = model->pollys.size();

  for (int ipol=0; ipol<npoly; ipol++) {
    unload (fptr, &model->pollys[ipol], rows+ipol);
    
    int colnum = 0;
    fits_get_colnum (fptr, CASEINSEN, "NPBLK", &colnum, &status);
    fits_write_col (fptr, TINT, colnum, rows+ipol, 1, 1, &npoly, &status);
    if (status != 0)
      throw FITSError (status, "unload polyco", "fits_write_col NPBLK");
  }
}

void unload (fitsfile* fptr, const polynomial* poly, long row)
{
  // these are used to pull out the data from a cell
  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;

  int status = 0;          // status returned by FITSIO routines

  int nspan = (int) poly->get_nspan();
  fits_get_colnum (fptr, CASEINSEN, "NSPAN", &colnum, &status);
  fits_write_col (fptr, TINT, colnum, row, firstelem, onelement,
		  &nspan, &status);
  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col NSPAN");

  int ncoef = poly->get_ncoeff();
  fits_get_colnum (fptr, CASEINSEN, "NCOEF", &colnum, &status);
  fits_write_col (fptr, TINT, colnum, row, firstelem, onelement,
		 &ncoef, &status);
  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col NCOEF");

  static char* site = new char[2];
  site[0] = poly->get_telescope();
  site[1] = '\0';

  fits_get_colnum (fptr, CASEINSEN, "NSITE", &colnum, &status);
  fits_write_col (fptr, TSTRING, colnum, row, firstelem, onelement,
		  &site, &status);
  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col NSITE");

  double temp = poly->get_freq();
  fits_get_colnum (fptr, CASEINSEN, "REF_FREQ", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 &temp, &status);
  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col REF_FREQ");

  double ref_mjd = poly->get_reftime().in_days();
  fits_get_colnum (fptr, CASEINSEN, "REF_MJD", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		   &ref_mjd, &status);
  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col REF_MJD");

  double ref_phs = poly->get_refphase().in_turns();
  fits_get_colnum (fptr, CASEINSEN, "REF_PHS", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 &ref_phs, &status);
  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col REF_PHS");
 
  temp = poly->get_reffrequency();
  fits_get_colnum (fptr, CASEINSEN, "REF_F0", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 &temp, &status);
  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col REF_F0");

  //! dispersion measure is not included in POLYCO binary table

  int  typecode = 0;
  long repeat = 0;
  long width = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "COEFF", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  
  if (repeat < ncoef)
    throw Error (InvalidState, "unload polynomial",
	       "COEFF vector repeat count=%ld < NCOEF=%d", repeat, ncoef);

  polynomial::Expert expert (const_cast<polynomial*>(poly));

  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, ncoef,
		  expert.get_coefs(), &status);

  if (status)
    throw FITSError (status, "unload polynomial", "fits_write_col COEFF");
}

