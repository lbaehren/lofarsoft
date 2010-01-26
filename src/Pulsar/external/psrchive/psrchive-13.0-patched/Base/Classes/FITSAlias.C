/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSAlias.h"

Pulsar::FITSAlias::FITSAlias ()
{
  add( "FD_POLN", "rcvr:basis" );
  add( "FD_HAND", "rcvr:hand" );
  add( "FD_SANG", "rcvr:sa" );
  add( "FD_XYPH", "rcvr:rph" );

  add( "BACKEND", "be:name" );
  add( "BE_PHASE","be:phase" );
  add( "BE_DCC",  "be:dcc" );

  add( "STT_IMJD", "ext:stt_imjd");
  add( "STT_SMJD", "ext:stt_smjd");
  add( "STT_OFFS", "ext:stt_offs");
}
