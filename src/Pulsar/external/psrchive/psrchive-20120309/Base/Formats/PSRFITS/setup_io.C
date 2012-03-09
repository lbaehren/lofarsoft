/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProfileColumn.h"

void Pulsar::FITSArchive::setup_dat (fitsfile* fptr,
                                     Reference::To<ProfileColumn>& dat) const
{
  if (!dat)
    dat = new ProfileColumn;

  dat->set_fitsfile (fptr);

  dat->set_data_colname   ("DATA");
  dat->set_offset_colname ("DAT_OFFS");
  dat->set_scale_colname  ("DAT_SCL");

  dat->set_nbin  (get_nbin());
  dat->set_nchan (get_nchan());
  dat->set_nprof (get_npol());

  dat->verbose = verbose > 2;
}

void Pulsar::FITSArchive::setup_aux (fitsfile* fptr,
                                     Reference::To<ProfileColumn>& aux,
                                     unsigned nprof) const
{
  if (!aux)
    aux = new ProfileColumn;

  aux->set_fitsfile (fptr);

  aux->set_data_colname   ("AUX_DATA");
  aux->set_offset_colname ("AUX_OFFS");
  aux->set_scale_colname  ("AUX_SCL");

  aux->set_nbin  (get_nbin());
  aux->set_nchan (get_nchan());
  aux->set_nprof (nprof);

  aux->verbose = verbose > 2;
}

