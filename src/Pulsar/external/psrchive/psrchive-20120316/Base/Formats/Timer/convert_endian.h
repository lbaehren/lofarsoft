/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/convert_endian.h,v $
   $Revision: 1.7 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __CONVERT_ENDIAN_H
#define __CONVERT_ENDIAN_H

#include "machine_endian.h"

#include "timer.h"
#include "band.h"
#include "mini.h"

#ifdef __cplusplus
extern "C" {
#endif
void timerChangeEndian (struct timer * hdr);
void bandChangeEndian  (struct band *b);
void miniChangeEndian  (struct mini * m);
#ifdef __cplusplus
}
#endif

#if MACHINE_LITTLE_ENDIAN
#define timer_toBigEndian(p)       timerChangeEndian(p)
#define timer_toLittleEndian(p)
#define timer_fromBigEndian(p)     timerChangeEndian(p)
#define timer_fromLittleEndian(p)

#define mini_toBigEndian(p)        miniChangeEndian(p)
#define mini_toLittleEndian(p)
#define mini_fromBigEndian(p)      miniChangeEndian(p)
#define mini_fromLittleEndian(p)

#define band_toBigEndian(p)        bandChangeEndian(p)
#define band_toLittleEndian(p)
#define band_fromBigEndian(p)      bandChangeEndian(p)
#define band_fromLittleEndian(p)
#else
#define timer_toBigEndian(p)
#define timer_toLittleEndian(p)    timerChangeEndian(p)
#define timer_fromBigEndian(p)
#define timer_fromLittleEndian(p)  timerChangeEndian(p)

#define mini_toBigEndian(p)
#define mini_toLittleEndian(p)     miniChangeEndian(p)
#define mini_fromBigEndian(p)
#define mini_fromLittleEndian(p)   miniChangeEndian(p)

#define band_toBigEndian(p)
#define band_toLittleEndian(p)     bandChangeEndian(p)
#define band_fromBigEndian(p)
#define band_fromLittleEndian(p)   bandChangeEndian(p)
#endif

#endif /* ndef __CONVERT_ENDIAN_H */

