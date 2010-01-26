/***************************************************************************
 *
 *   Copyright (C) 2003-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "CalibratorExtensionIO.h"
#include "psrfitsio.h"
#include "strutil.h"

#include <assert.h>

using namespace std;

void unload_variances (fitsfile*, const Pulsar::PolnCalibratorExtension*,
		       int ncpar, vector<float>& data);

void unload_covariances (fitsfile*, const Pulsar::PolnCalibratorExtension*,
			 int ncovar, vector<float>& data);

void unload_solver (fitsfile* fptr, const Pulsar::PolnCalibratorExtension* pce,
		    vector<float>& data);

void delete_solver (fitsfile* fptr);

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const PolnCalibratorExtension* pce) try
{
  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension entered" << endl;
  
  // Initialize the FEEDPAR Binary Table
  
  psrfits_init_hdu (fptr, "FEEDPAR");  

  int nchan = pce->get_nchan();
  int ncpar = pce->get_nparam();
  int ncovar = 0;

  if (pce->get_has_covariance())
    ncovar = ncpar * (ncpar+1) / 2;

  if (ncpar == 0)
    throw Error (InvalidState, "FITSArchive::unload PolnCalibratorExtension",
		 "number of model parameters == 0");

  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension nchan=" 
	 << nchan <<  " nparam=" << ncpar << " ncovar=" << ncovar << endl;

  string cal_mthd = pce->get_type()->get_name();

  // Write CAL_MTHD
  psrfits_update_key (fptr, "CAL_MTHD", cal_mthd);

  // Write NCPAR
  psrfits_update_key (fptr, "NCPAR", ncpar);

  // Write NCOVAR
  psrfits_update_key (fptr, "NCOVAR", ncovar);

  const PolnCalibratorExtension::Transformation* valid = 0;
  for (int ichan=0; ichan < nchan; ichan++)
    if (pce->get_valid(ichan))
      valid = pce->get_transformation(ichan);

  if (valid)
  {
    assert (valid->get_nparam() == unsigned(ncpar));
    for (unsigned iparam=0; iparam < valid->get_nparam(); iparam++)
    {
      string key = stringprintf ("PAR_%04d", iparam);
      psrfits_update_key (fptr, key.c_str(), valid->get_param_name(iparam),
			  valid->get_param_description(iparam).c_str());
    }
  }

  Pulsar::unload (fptr, pce);

  long dimension = nchan * ncpar;  
  vector<float> data( dimension, 0.0 );

  int count = 0;
  for (count = 0; count < dimension; count++)
    data[count] = fits_nullfloat;

  count = 0;
  for (int ichan = 0; ichan < nchan; ichan++)
  {
    if (pce->get_valid(ichan))
    {
      for (int j = 0; j < ncpar; j++)
      {
	data[count] = pce->get_transformation(ichan)->get_param(j);
	count++;
      }
    }
    else
    {
      if (verbose == 3)
        cerr << "FITSArchive::unload PolnCalibratorExtension ichan="
             << ichan << " flagged invalid" << endl;
      count += ncpar;
    }

  }

  assert (count == dimension);

  vector<unsigned> dimensions (2);
  dimensions[0] = ncpar;
  dimensions[1] = nchan;

  psrfits_write_col (fptr, "DATA", 1, data, dimensions);

  if (ncovar)
    unload_covariances (fptr, pce, ncovar, data);
  else
    unload_variances (fptr, pce, ncpar, data);

  if (pce->get_has_solver())
    unload_solver (fptr, pce, data);
  else
    delete_solver (fptr);

  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension exiting" << endl; 

}
catch (Error& error)
{
  throw error += "FITSArchive::unload PolnCalibratorExtension";
}

void unload_variances (fitsfile* fptr,
		       const Pulsar::PolnCalibratorExtension* pce,
		       int ncpar, vector<float>& data)
{
  unsigned nchan = pce->get_nchan();

  data.resize( ncpar * nchan );

  unsigned count = 0;
  for (unsigned i = 0; i < nchan; i++)
    if (pce->get_valid(i))
      for (int j = 0; j < ncpar; j++)
      {
	data[count] = sqrt(pce->get_transformation(i)->get_variance(j));
	count++;
      }
    else
      count += ncpar;

  assert (count == data.size());

  vector<unsigned> dimensions (2);
  dimensions[0] = ncpar;
  dimensions[1] = nchan;

  psrfits_delete_col (fptr, "COVAR");
  psrfits_write_col (fptr, "DATAERR", 1, data, dimensions);
}

void unload_covariances (fitsfile* fptr,
			 const Pulsar::PolnCalibratorExtension* pce,
			 int ncovar, vector<float>& data)
{
  unsigned nchan = pce->get_nchan();

  if (Pulsar::Archive::verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension"
      " ncovar = " << ncovar << endl;

  data.resize( ncovar * nchan );

  vector<double> covar;
  unsigned count = 0;

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    bool zero = false;

    if (!pce->get_valid(ichan))
      zero = true;

    if (!zero)
      pce->get_transformation(ichan)->get_covariance (covar);

    if (covar.size() == 0)
      zero = true;
    else
      assert (covar.size() == unsigned(ncovar));

    for (int icovar = 0; icovar < ncovar; icovar++)
    {
      if (zero)
	data[count] = 0;
      else
	data[count] = covar[icovar];
      count++;
    }
  }

  assert (count == data.size());

  vector<unsigned> dimensions (2);
  dimensions[0] = ncovar;
  dimensions[1] = nchan;

  psrfits_delete_col (fptr, "DATAERR");
  psrfits_write_col (fptr, "COVAR", 1, data, dimensions);
}

void unload_solver (fitsfile* fptr,
		    const Pulsar::PolnCalibratorExtension* pce,
		    vector<float>& data)
{
  unsigned nchan = pce->get_nchan();

  data.resize( nchan );
  vector<unsigned> nfree( nchan, 0 );

  for (unsigned i = 0; i < nchan; i++)
  {
    if (pce->get_valid(i))
    {
      data[i] = pce->get_transformation(i)->get_chisq();
      nfree[i] = pce->get_transformation(i)->get_nfree();
    }
    else
    {
      data[i] = 0.0;
      nfree[i] = 0.0;
    }
  }

  vector<unsigned> no_dimensions;

  psrfits_write_col (fptr, "CHISQ", 1, data,  no_dimensions);
  psrfits_write_col (fptr, "NFREE", 1, nfree, no_dimensions);
}

void delete_solver (fitsfile* fptr)
{
  psrfits_delete_col (fptr, "CHISQ");
  psrfits_delete_col (fptr, "NFREE");
}
