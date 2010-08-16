/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/malloc16.h,v $
   $Revision: 1.2 $
   $Date: 2009/03/11 18:38:37 $
   $Author: straten $ */

#ifndef __Utils_genutil_malloc16_h
#define __Utils_genutil_malloc16_h

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* return an newly allocated array with base pointer % 16 == 0 */
void* malloc16 (size_t n);

/* free an array allocated with malloc16 */
void free16 (void* p);

#ifdef __cplusplus
}

template<typename T>
class Array16
{
public:

  Array16 (size_t n) { ptr = (T*) malloc16 (sizeof(T) * n); }
  ~Array16 () { if (ptr) free16 (ptr); }
  operator T* () { return ptr; }
  bool operator ! () { return ptr == 0; }

protected:
  T* ptr;
};

#endif

#endif

