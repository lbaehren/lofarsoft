/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/dspReduction.h"

Pulsar::dspReduction::Interface::Interface
( dspReduction *s )
{
  if (s)
    set_instance (s);

  add( &dspReduction::get_software,
       &dspReduction::set_software,
       "soft", "Reduction software identifier" );

  add( &dspReduction::get_total_samples,
       &dspReduction::set_total_samples,
       "ntot", "Total number of time samples" );

  add( &dspReduction::get_block_size,
       &dspReduction::set_block_size,
       "nblock", "Number of time samples per block" );

  add( &dspReduction::get_overlap,
       &dspReduction::set_overlap,
       "nover", "Number of time samples in block overlap" );

  add( &dspReduction::get_state,
       &dspReduction::set_state,
       "state", "State of the input timeseries" );

  add( &dspReduction::get_nsamp_fft,
       &dspReduction::set_nsamp_fft,
       "nfft", "Number of time samples in each FFT" );

  add( &dspReduction::get_nsamp_overlap_pos,
       &dspReduction::set_nsamp_overlap_pos,
       "npos", "Number of time samples in positive wrap" );

  add( &dspReduction::get_nsamp_overlap_neg,
       &dspReduction::set_nsamp_overlap_neg,
       "nneg", "Number of time samples in negative wrap" );

  add( &dspReduction::get_nchan,
       &dspReduction::set_nchan,
       "nchan", "Number of frequency channels" );

  add( &dspReduction::get_freq_res,
       &dspReduction::set_freq_res,
       "fres", "Frequency resolution factor" );

  add( &dspReduction::get_time_res,
       &dspReduction::set_time_res,
       "tres", "Time resolution factor" );

  add( &dspReduction::get_ScrunchFactor,
       &dspReduction::set_ScrunchFactor,
       "tint", "Time integration factor" );

  add( &dspReduction::get_scale,
       &dspReduction::set_scale,
       "scale", "Final scale factor" );
}





