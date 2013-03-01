/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "malloc16.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#include <stdlib.h>

/* return an newly allocated array with base pointer % 16 == 0 */
void* malloc16 (size_t n)
{
#ifdef HAVE_MALLOC_H
  return memalign (16, n);
#else

  /*
   * The following code is ripped out of fftw-3.1.2/kernel/kalloc.c
   * Copyright (c) 2003, 2006 Matteo Frigo
   * Copyright (c) 2003, 2006 Massachusetts Institute of Technology
   * Licensed under the GNU General Public License
   *
   */
  void *p0, *p;
  p0 = malloc(n + 16);
  if (!p0)
    return 0;
  p = (void *) (((uintptr_t) p0 + 16) & (~((uintptr_t) 15)));
  *((void **) p - 1) = p0;
  return p;

#endif
}

/* free an array allocated with malloc16 */
void free16 (void* p)
{
#ifdef HAVE_MALLOC_H
  free (p);
#else
  if (p) free(*((void **) p - 1));
#endif
}

