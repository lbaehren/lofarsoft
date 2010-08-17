/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _psrdada_median_smooth_zap_h
#define _psrdada_median_smooth_zap_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef TEMPLATE_TYPE
#error TEMPLATE_TYPE must be defined before including median_smooth_zap.h
#endif

static int less_than (const void* aptr, const void* bptr)
{
  TEMPLATE_TYPE a = *( (TEMPLATE_TYPE*) aptr );
  TEMPLATE_TYPE b = *( (TEMPLATE_TYPE*) bptr );

  return a < b;
}

static int median_smooth (unsigned ndat, TEMPLATE_TYPE* data, unsigned wsize)
{
  if (ndat < 4)
    return -1;

  if (wsize < 3)
    return -1;
    
  if (ndat < wsize + 1)
    return -1;

  // make sort length an odd number
  if (wsize%2 == 0)
    wsize ++;

  unsigned ipt = 0, jpt = 0;

  unsigned middle = wsize/2;
  TEMPLATE_TYPE* window = malloc (wsize * sizeof(TEMPLATE_TYPE));

  unsigned rsize = ndat - wsize + 1;
  TEMPLATE_TYPE* result = malloc (ndat * sizeof(TEMPLATE_TYPE));

  unsigned truncated = 0;
  unsigned tmid = 0;

  // deal with leading edge
  for (ipt=0; ipt < middle; ipt++)
  {
    truncated = middle + 1 + ipt;
    tmid = truncated / 2;
    for (jpt=0; jpt < truncated; jpt++)
      window[jpt] = data[jpt];

    qsort (window, truncated, sizeof (TEMPLATE_TYPE), less_than);
    result[ipt] = window[tmid];
  }
  
  for (ipt=0; ipt < rsize; ipt++)
  {
    for (jpt=0; jpt<wsize; jpt++)
      window[jpt] = data[ipt+jpt];
    qsort (window, wsize, sizeof (TEMPLATE_TYPE), less_than);
    result[ipt+middle] = window[middle];
  }

  // deal with trailing edge
  for (ipt=rsize; ipt < ndat-middle; ipt++)
  {
    truncated = ndat - ipt;
    tmid = truncated / 2;
    for (jpt=0; jpt < truncated; jpt++)
      window[jpt] = data[ipt+jpt];
    qsort (window, truncated, sizeof (TEMPLATE_TYPE), less_than);
    result[ipt+middle] = window[tmid];
  }
  
  for (ipt=0; ipt < ndat; ipt++)
    data[ipt] = result[ipt];

  free (window);
  free (result);
}

static int median_smooth_zap (unsigned ndat, TEMPLATE_TYPE* data,
			      unsigned window)
{
  TEMPLATE_TYPE* copy = malloc (ndat * sizeof(TEMPLATE_TYPE));
  memcpy (copy, data, ndat * sizeof(TEMPLATE_TYPE));

  median_smooth (ndat, copy, window);

  double variance = 0.0;
  unsigned idat;

  for (idat=0; idat < ndat; idat++)
  {
    copy[idat] -= data[idat];
    copy[idat] *= copy[idat];
    // p0ptr[idat] = spectrum[idat];
    variance += copy[idat];
  }

  variance /= ndat;

  char zapped = 1;
  unsigned round = 1;
  unsigned total_zapped = 0;

  while (zapped)
  {
    float cutoff = 16.0 * variance;

    zapped = 0;
    round ++;

    for (idat=0; idat < ndat; idat++)
    {
      if (copy[idat] > cutoff ||
          (idat && fabs(copy[idat]-copy[idat-1]) > 2*cutoff))
      {
        variance -= copy[idat]/ndat;
        copy[idat] = data[idat] = 0.0;
	total_zapped ++;
        zapped = 1; 
      }
    }
  }
#ifdef _DEBUG
  fprintf (stderr, "zapped %u spikes in %u iterations\n", total_zapped, round);
#endif
  free (copy);
}

#endif
