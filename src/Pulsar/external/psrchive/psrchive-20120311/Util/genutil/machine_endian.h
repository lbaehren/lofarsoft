/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/machine_endian.h,v $
   $Revision: 1.10 $
   $Date: 2006/11/11 19:04:58 $
   $Author: straten $ */

#ifndef __M_ENDIAN_H
#define __M_ENDIAN_H

/* the main byte-swapping function */
#ifdef __cplusplus
extern "C" {
#endif

void changeEndian (void *p, int size);
void array_changeEndian (int count, void *p, int element_size);

#ifdef __cplusplus
}
#endif

/* MACRO to save you some typing */
#define ChangeEndian(p) changeEndian (&(p), sizeof(p))

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WORDS_BIGENDIAN

#define MACHINE_LITTLE_ENDIAN 0

#define toBigEndian(p,s)
#define toLittleEndian(p,s)	changeEndian(p,s)
#define fromBigEndian(p,s)		
#define fromLittleEndian(p,s)	changeEndian(p,s)

/* short forms that save typing */
#define ToBigEndian(p)
#define ToLittleEndian(p)       changeEndian (&(p), sizeof(p))
#define FromBigEndian(p)
#define FromLittleEndian(p)     changeEndian (&(p), sizeof(p))

/* short forms that work on arrays */
#define N_ToBigEndian(N,p) 
#define N_ToLittleEndian(N,p)   array_changeEndian (N, p, sizeof(*p))
#define N_FromBigEndian(N,p) 
#define N_FromLittleEndian(N,p) array_changeEndian (N, p, sizeof(*p))

#else

#define MACHINE_LITTLE_ENDIAN 1

#define toBigEndian(p,s) 	changeEndian(p,s)
#define toLittleEndian(p,s)
#define fromBigEndian(p,s)	changeEndian(p,s)
#define fromLittleEndian(p,s)

/* short forms that save typing */
#define ToBigEndian(p)          changeEndian (&(p), sizeof(p))
#define ToLittleEndian(p)
#define FromBigEndian(p)        changeEndian (&(p), sizeof(p))
#define FromLittleEndian(p)   

/* short forms that work on arrays */
#define N_ToBigEndian(N,p)      array_changeEndian (N, p, sizeof(*(p)))
#define N_ToLittleEndian(N,p)
#define N_FromBigEndian(N,p)    array_changeEndian (N, p, sizeof(*(p)))
#define N_FromLittleEndian(N,p)

#endif

#endif  /* !def __M_ENDIAN_H */
