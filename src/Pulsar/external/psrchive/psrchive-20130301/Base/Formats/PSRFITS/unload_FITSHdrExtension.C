/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FITSArchive.h"
#include "Pulsar/FITSHdrExtension.h"
#include <psrfitsio.h>
#include <tostring.h>
#include <FITSUTC.h>



void Pulsar::FITSArchive::unload (fitsfile* fptr, const FITSHdrExtension* ext)
{
  // need to write the creation date as now
  time_t now = time( NULL );
  FITSUTC now_utc( now );
  psrfits_update_key( fptr, "DATE", tostring<FITSUTC>( now_utc ) );

  psrfits_update_key (fptr, "IBEAM", ext->ibeam);
  psrfits_update_key (fptr, "DATE-OBS", ext->stt_date + "T" + ext->stt_time);
  psrfits_update_key (fptr, "OBSFREQ", ext->obsfreq);
  psrfits_update_key (fptr, "OBSBW", ext->obsbw);
  psrfits_update_key (fptr, "OBSNCHAN", ext->obsnchan);
  psrfits_update_key (fptr, "COORD_MD", ext->coordmode);
  psrfits_update_key (fptr, "PNT_ID", ext->pnt_id);
  psrfits_update_key (fptr, "EQUINOX", ext->equinox);
  psrfits_update_key (fptr, "BMAJ", ext->bmaj);
  psrfits_update_key (fptr, "BMIN", ext->bmin);
  psrfits_update_key (fptr, "BPA", ext->bpa);
  psrfits_update_key (fptr, "STT_CRD1", ext->stt_crd1);
  psrfits_update_key (fptr, "STT_CRD2", ext->stt_crd2);
  psrfits_update_key (fptr, "TRK_MODE", ext->trk_mode);
  psrfits_update_key (fptr, "STP_CRD1", ext->stp_crd1);
  psrfits_update_key (fptr, "STP_CRD2", ext->stp_crd2);
  psrfits_update_key (fptr, "STT_LST", ext->stt_lst);
}

