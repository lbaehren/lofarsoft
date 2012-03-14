/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProcHistory.h"
#include "psrfitsio.h"

using namespace std;

void unload (fitsfile* fptr, const Pulsar::ProcHistory::row* hrow) try
{
  if (!hrow)
    return;

  int row = hrow->index;

  if (Pulsar::Archive::verbose == 3)
    cerr << "unload Pulsar::ProcHistory::row row=" << row << endl;

  psrfits_write_col (fptr, "DATE_PRO", row, hrow->date_pro);
  psrfits_write_col (fptr, "PROC_CMD", row, hrow->proc_cmd);
  psrfits_write_col (fptr, "POL_TYPE", row, hrow->pol_type);
  psrfits_write_col (fptr, "NSUB",     row, hrow->nsub);
  psrfits_write_col (fptr, "NPOL",     row, hrow->npol);
  psrfits_write_col (fptr, "NBIN",     row, hrow->nbin);
  psrfits_write_col (fptr, "NBIN_PRD", row, hrow->nbin_prd);
  psrfits_write_col (fptr, "TBIN",     row, hrow->tbin);
  psrfits_write_col (fptr, "CTR_FREQ", row, hrow->ctr_freq);
  psrfits_write_col (fptr, "NCHAN",    row, hrow->nchan);
  psrfits_write_col (fptr, "CHAN_BW",  row, hrow->chan_bw);
  psrfits_write_col (fptr, "DM",       row, hrow->dispersion_measure);
  psrfits_write_col (fptr, "RM",       row, hrow->rotation_measure);

  // new in version 3.5
  psrfits_write_col (fptr, "PR_CORR",  row, hrow->pr_corr);
  psrfits_write_col (fptr, "FD_CORR",  row, hrow->fd_corr);
  psrfits_write_col (fptr, "BE_CORR",  row, hrow->be_corr);

  psrfits_write_col (fptr, "RM_CORR",  row, hrow->rm_corr);
  psrfits_write_col (fptr, "DEDISP",   row, hrow->dedisp);
  psrfits_write_col (fptr, "DDS_MTHD", row, hrow->dds_mthd);
  psrfits_write_col (fptr, "SC_MTHD",  row, hrow->sc_mthd);
  psrfits_write_col (fptr, "CAL_MTHD", row, hrow->cal_mthd);
  psrfits_write_col (fptr, "CAL_FILE", row, hrow->cal_file);
  psrfits_write_col (fptr, "RFI_MTHD", row, hrow->rfi_mthd);
  psrfits_write_col (fptr, "IFR_MTHD", row, hrow->ifr_mthd);
  psrfits_write_col (fptr, "SCALE",    row, hrow->scale);

  if (Pulsar::Archive::verbose > 2)
    cerr << "FITSArchive::unload_hist_row exiting" << endl;
}
catch (Error& error)
{
  throw error += "unload Pulsar::ProcHistory::row";
}

void Pulsar::FITSArchive::unload (fitsfile* fptr, const ProcHistory* history)
try
{
  unsigned numrows = history->rows.size();

  if (verbose == 3)
    cerr << "FITSArchive::unload ProcHistory rows=" << numrows << endl;

  // Move to the HISTORY HDU

  int status = 0;

  fits_movnam_hdu (fptr, BINARY_TBL, "HISTORY", 0, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload ProcHistory",
                     "fits_movnam_hdu HISTORY");
  
  psrfits_clean_rows (fptr);

  fits_insert_rows (fptr, 0, numrows, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload ProcHistory",
                     "fits_insert_rows HISTORY");
  
  for (unsigned i = 0; i < numrows; i++) 
    ::unload (fptr, &(history->rows[i]));
  
  if (verbose == 3)
    cerr << "FITSArchive::unload ProcHistory exiting" << endl;
}
catch (Error& error)
{
  error += "Pulsar::FITSArchive::unload ProcHistory";
}

