/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProcHistory.h"

using Pulsar::ProcHistory;

ProcHistory::Interface::Interface( ProcHistory *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &ProcHistory::get_nrow, "nrow", "Number of rows in history" );
  add( &ProcHistory::get_last_nbin_prd, "nbin_prd", "Nr of bins per period" );
  add( &ProcHistory::get_last_tbin, "tbin", "Time per bin or sample" );
  add( &ProcHistory::get_last_chan_bw, "chan_bw", "Channel bandwidth" );
  add( &ProcHistory::get_cal_file, "cal_file", "Calibrator filename" );
}



