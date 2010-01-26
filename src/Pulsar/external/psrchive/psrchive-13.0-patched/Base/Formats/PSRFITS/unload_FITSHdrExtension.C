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
// #include <ctime>



void Pulsar::FITSArchive::unload (fitsfile* fptr, const FITSHdrExtension* ext)
{
  psrfits_update_key (fptr, "COORD_MD", ext->coordmode);
  psrfits_update_key (fptr, "EQUINOX", ext->equinox);
  //psrfits_update_key (fptr, "DATE", ext->creation_date);
  // need to write the creation date as now
  time_t now = time( NULL );
  FITSUTC now_utc( now );
  psrfits_update_key( fptr, "DATE", tostring<FITSUTC>( now_utc ) );

  psrfits_update_key (fptr, "TRK_MODE", ext->trk_mode);
  psrfits_update_key (fptr, "DATE-OBS", ext->stt_date + "T" + ext->stt_time);
  psrfits_update_key (fptr, "STT_LST", ext->stt_lst);
  psrfits_update_key (fptr, "STT_TIME", ext->stt_time);
  psrfits_update_key (fptr, "STT_DATE", ext->stt_date);
  psrfits_update_key (fptr, "OBSFREQ", ext->obsfreq );
}

