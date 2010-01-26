/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* This file defines C wrappers to the fortran rd_eph and wr_eph functions */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ephio.h"

#define F77_rd_eph F77_FUNC_(rd_eph,RD_EPH)
#define F77_wr_eph F77_FUNC_(wr_eph,WR_EPH)

#define F77_rd_eph_lun F77_FUNC_(rd_eph_lun,RD_EPH_LUN)
#define F77_wr_eph_lun F77_FUNC_(wr_eph_lun,WR_EPH_LUN)

#define F77_rd_eph_str F77_FUNC_(rd_eph_str,RD_EPH_STR)
#define F77_wr_eph_str F77_FUNC_(wr_eph_str,WR_EPH_STR)

#define F77_length F77_FUNC(length,LENGTH)

extern int F77_rd_eph (char *, int *, char *, double *,
		       int *, double *, long int, long int);
extern int F77_wr_eph (char *, int *, char *, double *,
		       int *, double *, long int, long int);

extern int F77_rd_eph_lun (int *, int *, char *, double *,
			   int *, double *, long int);
extern int F77_wr_eph_lun (int *, int *, char *, double *,
			   int *, double *, long int);

extern int F77_rd_eph_str (int* status, char* str_val, double* double_val,
			   int* int_val, double* error, int* convert,
			   int* isOld, char* buffer,
			   long int str_val_len,
			   long int buffer_len);

extern int F77_wr_eph_str (char* string, int* ephind, char* str_val, 
			   double* double_val, int* int_val,
			   double* error, int* status, 
			   long int string_len,
			   long int str_val_len);

extern int F77_length (char *buffer, int buflen);

int rd_eph_wrap (int uselun, char *fname, int lun,
		 int parmStatus[EPH_NUM_KEYS], 
		 char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
		 double value_double[EPH_NUM_KEYS], 
		 int value_integer[EPH_NUM_KEYS],
		 double error_double[EPH_NUM_KEYS])
{
  int retval, i,j;

  /* Array passed to Fortran routines instead of two-D C array */
  char* v_str = malloc (EPH_NUM_KEYS*EPH_STR_LEN);

  /* fortran space-padded strings */
  memset (v_str, ' ', EPH_NUM_KEYS*EPH_STR_LEN);

  if (uselun)
    retval = F77_rd_eph_lun (&lun, parmStatus, v_str, value_double,
			     value_integer, error_double, EPH_STR_LEN);
  else
    retval = F77_rd_eph (fname, parmStatus, v_str, value_double,
			 value_integer, error_double, 
			 strlen(fname), EPH_STR_LEN);

  for (i=0; i < EPH_NUM_KEYS; i++)
  {
    if (parmStatus[i] && parmTypes[i] == EPH_TYPE_STRING)
    {
      j = F77_length(v_str+i*EPH_STR_LEN, EPH_STR_LEN);
      memcpy(value_str[i], v_str+i*EPH_STR_LEN, j);
    }
    else
      j = 0;

    value_str[i][j] = '\0';
  }

  free (v_str);

  return retval;
}


int eph_rd_str (int parmStatus[EPH_NUM_KEYS], 
		char value_str[EPH_NUM_KEYS*EPH_STR_LEN], 
		double value_double[EPH_NUM_KEYS], 
		int value_integer[EPH_NUM_KEYS],
		double error_double[EPH_NUM_KEYS],
		int convert[EPH_NUM_KEYS],
		int* isOldEphem,
		char* parsethis)
{
  int retval = F77_rd_eph_str (parmStatus, value_str, value_double,
			       value_integer, error_double, convert,
			       isOldEphem, parsethis, EPH_STR_LEN,
			       strlen(parsethis));
  return retval;
}

int eph_rd (char *fname, int parmStatus[EPH_NUM_KEYS], 
	    char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
	    double value_double[EPH_NUM_KEYS], int value_integer[EPH_NUM_KEYS],
	    double error_double[EPH_NUM_KEYS])
{
  return rd_eph_wrap(0, fname, 0, parmStatus, value_str,
		     value_double, value_integer, error_double);
}

int eph_rd_lun (int lun, int parmStatus[EPH_NUM_KEYS], 
		char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
		double value_double[EPH_NUM_KEYS],
		int value_integer[EPH_NUM_KEYS],
		double error_double[EPH_NUM_KEYS])
{
  return rd_eph_wrap(1, NULL, lun, parmStatus, value_str,
		     value_double, value_integer, error_double);
}


int eph_wr_str (char *buffer, int buflen, int ephind, int parmStatus, 
		char* value_str, double value_double,
		int value_integer, double error_double)
{
  int str_len = 0;
  int retval = 0;
  int c;

  if ((ephind < 0) || (ephind > EPH_NUM_KEYS)) {
    fprintf (stderr, "eph_wr_str:: invalid index=%d\n", ephind);
    return -1;
  }

  /* initialize the buffer for call to Fortran */
  for (c=0; c<buflen; c++)
    buffer[c] = ' ';

  if (parmTypes[ephind] == 0)
    str_len = strlen (value_str);

  ephind ++;  /* Fortran counts from 1; C from 0 */
  retval = F77_wr_eph_str (buffer, &ephind, value_str, &value_double,
			   &value_integer, &error_double, &parmStatus, 
			   buflen, str_len);

  str_len = F77_length(buffer, buflen);
  buffer[str_len] = '\0';

  return retval;
}

int wr_eph_wrap (int uselun, char *fname, int lun,
		 int parmStatus[EPH_NUM_KEYS], 
		 char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
		 double value_double[EPH_NUM_KEYS],
		 int value_integer[EPH_NUM_KEYS],
		 double error_double[EPH_NUM_KEYS])
{
  int i,j, retval;

  /* Array passed to Fortran routines instead of two-D C array */
  char* v_str = malloc (EPH_NUM_KEYS*EPH_STR_LEN);

  /* fortran space-padded strings */
  memset (v_str, ' ', EPH_NUM_KEYS*EPH_STR_LEN);

  /* fix up the 2-d array. */
  for (i=0; i < EPH_NUM_KEYS; i++)
  {
    if ( parmStatus[i] && parmTypes[i]==EPH_TYPE_STRING )
    {
      j = strlen(value_str[i]);
      memcpy(v_str+i*EPH_STR_LEN, value_str[i], j);
    }
  }

  if (uselun)
    retval = F77_wr_eph_lun (&lun, parmStatus, v_str, value_double,
			     value_integer, error_double, EPH_STR_LEN);
  else
    retval = F77_wr_eph (fname, parmStatus, v_str, value_double,
			 value_integer, error_double,
			 strlen(fname), EPH_STR_LEN);

  free (v_str);

  return retval;
}

int eph_wr (char *fname, int parmStatus[EPH_NUM_KEYS], 
	    char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
	    double value_double[EPH_NUM_KEYS],
	    int value_integer[EPH_NUM_KEYS],
	    double error_double[EPH_NUM_KEYS])
{
  return wr_eph_wrap (0, fname, 0, parmStatus, value_str,
		      value_double, value_integer, error_double);
}

int eph_wr_lun (int lun,
		int parmStatus[EPH_NUM_KEYS], 
		char value_str[EPH_NUM_KEYS][EPH_STR_LEN], 
		double value_double[EPH_NUM_KEYS],
		int value_integer[EPH_NUM_KEYS],
		double error_double[EPH_NUM_KEYS])
{
  return wr_eph_wrap (1, NULL, lun, parmStatus, value_str,
		      value_double, value_integer, error_double);
}
