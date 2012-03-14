/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* 
   ADVICE!:  Do not include this file directly.  Instead, #include "ephio.h"

   $Source: /cvsroot/psrchive/psrchive/Util/tempo/ephio_func.h,v $
   $Revision: 1.6 $
   $Date: 2008/02/15 02:03:58 $
   $Author: straten $ 
*/

#ifndef __EPHIO_FUNC_H
#define __EPHIO_FUNC_H

#define EPH_TYPE_STRING 0
#define EPH_TYPE_REAL 1
#define EPH_TYPE_HMS 2
#define EPH_TYPE_DMS 3
#define EPH_TYPE_MJD 4
#define EPH_TYPE_INT 5

#ifdef __cplusplus
extern "C" {
#endif

  extern int eph_rd (char* filename, int* parmStatus,
		     char strs[EPH_NUM_KEYS][EPH_STR_LEN],
		     double* value, int* integer, double* error);
  
  extern int eph_rd_lun (int lun, int* parmStatus,
			 char strs[EPH_NUM_KEYS][EPH_STR_LEN],
			 double* value, int* integer, double* error);
  
  extern int eph_wr (char* filename, int* parmStatus,
		     char strs[EPH_NUM_KEYS][EPH_STR_LEN],
		     double* value, int* integer, double* error);
  
  extern int eph_wr_lun (int lun, int* parmStatus,
			 char strs[EPH_NUM_KEYS][EPH_STR_LEN],
			 double* value, int* integer, double* error);
  
  /* ***********************************************************************
     eph_rd_str - parse from a string
     
     Use this function to parse one string at a time.
     As with eph_rd*, the user must supply arrays of size EPH_NUM_KEYS.
     This function will parse the values into the appropriate element of the
     arrays.
     
     The user must initialize the arrays before repeatedly calling this routine
     and call 'convertUnits_' to correct the units after completion
  */
  int eph_rd_str (int* parmStatus,
		  char* strs,
		  double* value, int* integer,
		  double* error, int* convert,
		  int* isOldEphem, char* parsethis);
  
  /* writes a string into buffer that could be parsed by eph_rd_str */
  int eph_wr_str (char *buffer, int buflen, int ephind, int parmStatus, 
		  char* value_str, double value, int integer, double error);

#ifdef __cplusplus
	   }
#endif
  
#endif
