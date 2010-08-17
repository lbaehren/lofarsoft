/* $Source: /cvsroot/psrdada/psrdada/src/tmutil.h,v $
   $Revision: 1.2 $
   $Date: 2009/07/16 02:05:04 $
   $Author: ajameson $ */

#ifndef DADA_UTC_H
#define DADA_UTC_H

#define _XOPEN_SOURCE /* glibc2 needs this for strptime  */
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

  /*! parse a string into struct tm; return equivalent time_t */
  time_t str2tm (struct tm* time, const char* str);

  /*! parse a string and return equivalent time_t */
  time_t str2time (const char* str);

  /*! parse a UTC string and return equivalent time_t */
  time_t str2utctime (const char* str);

  /*! parse a UTC time string into struct tm; return equivalent time_t */
  time_t str2utctm (struct tm* time, const char* str);

#ifdef __cplusplus
}
#endif

#endif

