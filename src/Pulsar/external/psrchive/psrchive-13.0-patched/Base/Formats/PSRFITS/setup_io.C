/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProfileColumn.h"

void Pulsar::FITSArchive::setup_dat_io (fitsfile* fptr) const
{
  dat_io = new ProfileColumn;

  dat_io->set_fitsfile (fptr);

  dat_io->set_data_colname   ("DATA");
  dat_io->set_offset_colname ("DAT_OFFS");
  dat_io->set_scale_colname  ("DAT_SCL");

  dat_io->set_nbin  (get_nbin());
  dat_io->set_nchan (get_nchan());
  dat_io->set_nprof (get_npol());
}

void Pulsar::FITSArchive::setup_aux_io (fitsfile* fptr, unsigned nprof) const
{
  aux_io = new ProfileColumn;

  aux_io->set_fitsfile (fptr);

  aux_io->set_data_colname   ("AUX_DATA");
  aux_io->set_offset_colname ("AUX_OFFS");
  aux_io->set_scale_colname  ("AUX_SCL");

  aux_io->set_nbin  (get_nbin());
  aux_io->set_nchan (get_nchan());
  aux_io->set_nprof (nprof);  
}
