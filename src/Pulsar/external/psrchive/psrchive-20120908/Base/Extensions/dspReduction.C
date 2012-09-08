/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/dspReduction.h"
using namespace std;

//! Default constructor
Pulsar::dspReduction::dspReduction ()
  : Extension ("dspReduction")
{
  total_samples = 0;
  block_size = 0;
  overlap = 0;

  state = Signal::Nyquist;

  nsamp_fft = 0;
  nsamp_overlap_pos = 0;
  nsamp_overlap_neg = 0;

  nchan = 0;
  freq_res = 0;
  time_res = 0;

  ScrunchFactor = 0;

  scale = 0.0;
}

//! Copy constructor
Pulsar::dspReduction::dspReduction (const dspReduction& extension)
  : Extension ("dspReduction")
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::dspReduction copy constructor" << endl; 

  operator=(extension);
}

Pulsar::dspReduction* Pulsar::dspReduction::clone () const
{ 
  if (Archive::verbose == 3)
    cerr << "Pulsar::dspReduction::clone" << endl;

  return new dspReduction( *this );
}

//! Operator =
const Pulsar::dspReduction&
Pulsar::dspReduction::operator= (const dspReduction& dsp)
{
  total_samples = dsp.total_samples;
  block_size = dsp.block_size;
  overlap = dsp.overlap;

  state = dsp.state;

  nsamp_fft = dsp.nsamp_fft;
  nsamp_overlap_pos = dsp.nsamp_overlap_pos;
  nsamp_overlap_neg = dsp.nsamp_overlap_neg;

  nchan = dsp.nchan;
  freq_res = dsp.freq_res;
  time_res = dsp.time_res;

  ScrunchFactor = dsp.ScrunchFactor;

  scale = dsp.scale;

  return *this;
}

//! Destructor
Pulsar::dspReduction::~dspReduction ()
{
}

TextInterface::Parser* Pulsar::dspReduction::get_interface()
{
  return new dspReduction::Interface( this );
}
