/***************************************************************************
 *
 *   Copyright (C) 1998 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/utc.h,v $
   $Revision: 1.7 $
   $Date: 2007/09/24 08:53:28 $
   $Author: straten $ */

/* ************************************************************************
   UTC time structure - a subset of 'struct tm' used when you don't know
   or want to calculate neither the day of month nor month of year.
   ************************************************************************ */

#ifndef UTC_H
#define UTC_H

#include <time.h>

typedef struct {
   int tm_sec;
   int tm_min;
   int tm_hour;
   int tm_yday;
   int tm_year;
} utc_t;

#define UTC_INIT {0,0,0,0,0}

#ifdef __cplusplus
extern "C" {
#endif

int    str2utc    (utc_t *time, const char* str);
char*  utc2str    (char* str, utc_t time, const char* fmt);
char*  utcstrfill (char* str, utc_t time, int fill_chars);
int    utc_diff   (utc_t time1, utc_t time2);
int    utc_inc    (utc_t *time, int seconds);
int    utc_dec    (utc_t *time, int seconds);

int tm2utc (utc_t *time, struct tm caltime);
int utc2tm (struct tm *caltim, utc_t time);

int utc_f2LST (double* lst, utc_t timeutc, double fracsec, float longitude);
int utc2LST (double* lst, utc_t timeutc, float east_longitude);

#ifdef __cplusplus
}
#endif

#define UTC_LEAPYEAR(year)   ( ((year)%4 == 0) && \
			       (((year)%100 != 0) || ((year)%400 == 0)) )
#define UTC_JULIANDAYS(year) ( 365 + ((UTC_LEAPYEAR(year))?1:0) )

#endif

