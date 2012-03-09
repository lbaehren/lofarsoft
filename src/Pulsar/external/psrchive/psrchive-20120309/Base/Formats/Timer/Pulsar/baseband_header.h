/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __BASEBAND_HEADER_H
#define __BASEBAND_HEADER_H

#include <inttypes.h>

// Q: why a C struct?
// A: to minimize the chances of discrepency between state as understood
//    in the software and state as written to disk with one big write

typedef struct baseband_header_t {

  // Endian-ness code
  uint32_t         endian;
  unsigned int   version;
  unsigned int   size;

  // How the data was read from file during processing
  unsigned int   totaljobs;
  uint64_t         gulpsize;
  uint64_t         seeksize;

  // state of voltage: analytic(complex)=2 or Nyquist(real)=1
  int  voltage_state;
  // the number of independantly sampled analog channels
  int  analog_channels;
  // number of time samples used while dynamic level setting
  int  ppweight;
  // tolerance to invalid sampling threshold
  int  dls_threshold_limit;

  // number of passbands kept (usually 2 polns, or maybe one for each of four
  // undersampled analog channels)
  int  pband_channels;

  // number of bins in passband
  int  pband_resolution;

  // # of time samples entering into convolution with the dedispersion kernel
  int  nfft;

  // # of (native voltage state) time samples discarded after convolution
  // int nsmear;

  // Version 5 change: As the impulse response function may be
  // assymetric, the old nsmear attribute is now interpreted as
  // nsmear_pos * 2.
  int  nsmear_pos;

  // scrunch detected power before folding
  int  nscrunch;

  // This space ensures that the structure appears the same regardless of
  // memory alignment convention
  int memory_align;

  // the factor by which the resulting profiles are normalized
  double power_normalization;

  // If a synthetic filterbank was created, then the following apply:
  // size of backward FFTs (into time domain)
  int  f_resolution;
  // over-sampling in the time domain
  int  t_resolution;

  // various flags for various states and operations performed.

  uint64_t time_domain;
  uint64_t frequency_domain;

  // Version 1 additions
  float mean_power_cutoff;

  // Version 2 additions
  int  hanning_smoothing;

  // Version 3 addition
  // char pcalid [16];

  // Version 4 addition
  // (note that Version 4 simply increases the size of pcalid)
  char pcalid [64];

  // Version 5 addition
  // 
  // # of time samples discarded from right hand side of convolution product
  int nsmear_neg;

  // # number of filterbank channels used during reduction
  int nchan;

} baseband_header;


// //////////////////////////////////////////////////////////////////////
// sizeof(baseband_header)
#define BASEBAND_HEADER_SIZE 184

// //////////////////////////////////////////////////////////////////////
// Endian code
#define BASEBAND_HEADER_ENDIAN   0x01234567
#define BASEBAND_OPPOSITE_ENDIAN 0x67452301

// //////////////////////////////////////////////////////////////////////
// Version
#define BASEBAND_HEADER_VERSION 6

#endif
