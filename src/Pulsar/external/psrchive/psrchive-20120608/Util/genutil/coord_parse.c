/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

// #define _DEBUG 1

#include "coord_parse.h"

/*! Parses a string containing RA/DEC coordinates to produce ra and
  dec in radians.  The string may be of the form:

	       hhmm~ddmm    or  hhmmss~ddmmss
	       hh:mm~dd:mm  or  hh:mm:ss~dd:mm:ss

   or many other combinations, as long as:

   1) each field contains exactly two characters 
   2) text delimits a maximum of two characters (may contain one, so
      h:mm:ss works)

	       hh  hour angle of ra
	       dd  degrees of declination
	       mm  minutes
	       ss  seconds
	       ~   + or - sign of the degrees of declination

   returns a -1 on error, zero on success */
int str2coord (double *ra, double *dec, const char* coordstring) 
{
  int retval = 0;

  char* coordstr = strdup (coordstring);
  char* rastr = coordstr;   /* beginning of RA */

  /* search for the +/- sign of the DEC */
  char* decstr = strchr (coordstr, '-');
  if (decstr == NULL)
    decstr = strchr (coordstr, '+');
  if (decstr == NULL)
    decstr = strchr (coordstr, ' ');
  if (decstr == NULL)
    decstr = strchr (coordstr, '\t');
  if (decstr == NULL)
  {
    fprintf (stderr, "str2coord: No +,-,<space>, or <tab> in '%s'\n",coordstr);
    free (coordstr);
    return -1;
  }

  /* fprintf (stderr, "str2coord: decstr='%s'\n", decstr); */
  if (str2dec2 (dec, decstr) < 0)
    retval = -1;

  *decstr = '\0';

  /* fprintf (stderr, "str2coord: rastr='%s'\n", rastr); */
  if (str2ra (ra, rastr) < 0)
    retval = -1;

  free (coordstr);

  return retval;
}


/*! parses a string containing fields into a unit value.

  This function is particularly useful for parsing strings of the form
  hh:mm:ss.sssss or dd:mm:ss.ssss

  \retval unit        value parsed from the string
  \retval nfields     the number of fields in the string
  \retval field_width widths (number of characters) of each field
  \retval field_scale values with which to normalize the value in each field
  \param  unit_string string containing the value to be parsed

  If the field_width for a given field is equal to zero, that field is
  treated as a floating point number of an undetermined width (number of
  characters).  Otherwise, only field_width characters will be parsed.

  The number parsed from each field will be normalized by a scale factor
  that grows with each field read.  ie. for the i'th field, the scale factor
  will be equal to field_scale[i]*field_scale[i-1]*...*field_scale[0].

  \return the number of fields parsed from the string. */
int str2unit (double* unit, unsigned nfields,
	      const int* field_width, const double* field_scale,
	      const char* unit_string)
{
  char* unit_string_copy = strdup (unit_string);
  char* field_string_copy = strdup (unit_string);

  char* curstr = unit_string_copy;
  char* endstr = strchr (curstr, '\0');

  double field_value = 0;

  double sign = 1.0;
  double value = 0.0;
  double scale = 1.0;

  unsigned ifield;
  
  /* skip leading non-numeric characters */
  while ( curstr<endstr && !isdigit(*curstr) 
	  && *curstr != '-' && *curstr != '+' )
    curstr++;

  if (*curstr == '-')
  {
    sign = -1.0;
    curstr ++;
  }
  else if (*curstr == '+')
  {
    sign = 1;
    curstr ++;
  }

  for (ifield=0; curstr<endstr && ifield<nfields; ifield++)
  {
    /* skip leading non-numeric characters */
    while ( curstr<endstr && !isdigit(*curstr) )
      curstr++;

    if (field_width[ifield])
    {
      strncpy (field_string_copy, curstr, field_width[ifield]);
      field_string_copy [field_width[ifield]] = '\0';
      curstr += field_width[ifield];
    }
    else
    {
      strcpy (field_string_copy, curstr);
      /* skip to end of numeric characters */
      while ( curstr<endstr && (isdigit(*curstr) || *curstr=='.') )
	curstr++;
    }

    if (sscanf (field_string_copy, "%lf", &field_value) != 1)
      break;

    scale /= field_scale[ifield];
    value += sign * scale * field_value;

  }

  free (field_string_copy);
  free (unit_string_copy);

  *unit = value;
  return ifield;

} 

int str2ra (double *ra, const char* rastring) 
{
  int field_width[3] = {2, 2, 0};
  double field_scale[3] = {24.0, 60.0, 60.0};

  if (str2unit (ra, 3, field_width, field_scale, rastring) < 0)
    return -1;

  *ra *= 2.0 * M_PI;
  return 0;
}

int str2dec2 (double *dec, const char* decstring) 
{
  int field_width[3] = {2, 2, 0};
  double field_scale[3] = {360.0, 60.0, 60.0};

  if (str2unit (dec, 3, field_width, field_scale, decstring) < 0)
    return -1;

  *dec *= 2.0 * M_PI;
  return 0;
}


/*! Produces a string of the form hh:mm:ss.sss[+|-]dd:mm:ss.sss,
  given ra and dec in radians.  The number of decimal places in ss.sss
  is controlled by places.  */
int coord2str (char* coordstring, unsigned coordstrlen, double ra, double dec,
	       unsigned places) 
{
  int decstart;

  ra2str (coordstring, coordstrlen, ra, places);

  decstart = strlen (coordstring);

  dec2str2 (coordstring+decstart, coordstrlen-decstart, dec, places);

  return 0;
}

/*! returns the number of fields successfully parsed into unit_string. */
int unit2str (char* unit_string, unsigned unit_strlen,
	      char sign, unsigned nfields,
	      const unsigned* field_width, const unsigned* field_precision,
	      const double* field_scale, char separator, double unit)
{
  char* end_string = unit_string + unit_strlen;

  double rounding = 0;
  double field_value = 0;

  unsigned ifield = 0;
  int printed = 0;

  if (unit_strlen < 1)
    return 0;

  if (unit < 0)
  {
    unit *= -1.0;
    *unit_string = '-';
    unit_string ++;
  }
  else if (sign)
  {
    *unit_string = '+';
    unit_string ++;
  }

  rounding = 0.1 / pow (10,field_precision[nfields-1]);
  for (ifield=0; ifield<nfields; ifield++)
  {
    assert (field_scale[ifield] > 0.0);
    rounding /= field_scale[ifield];
  }
  unit += rounding;

  for (ifield=0; unit_string<end_string && ifield<nfields; ifield++)
  {
    if (separator && ifield > 0)
    {
      *unit_string = separator;
      unit_string ++;
    }

    unit *= field_scale[ifield];

    if (field_precision[ifield])
    {
      field_value = unit;
      printed = snprintf (unit_string, end_string-unit_string, "%0*.*f",
			  field_width[ifield]+field_precision[ifield]+1,
			  field_precision[ifield], field_value);
    }
    else
    {
      field_value = floor (unit);
      printed = snprintf (unit_string, end_string-unit_string, "%.*d",
			  field_width[ifield], (int) field_value);
    }
    
    unit -= field_value;

    if (printed < 0)
      return ifield;

    unit_string += printed;

  }

  return ifield;
} 

/* given a value in radians, returns a string parsed into either
   hh:mm:ss.sss or (-)dd:mm:ss.sss format, as determined by setting
   scale equal to either 24 or 360, respectively */
int xms2str (char* xms_str, unsigned xms_strlen, char sign,
             double radians, double scale, unsigned places) 
{
  unsigned field_width[3] = {2, 2, 2};
  unsigned field_precision[3] = {0, 0, 0};
  double field_scale[3] = {0.0, 60.0, 60.0};

  field_precision[2] = places;
  field_scale[0] = scale;

  radians /= 2.0 * M_PI;
  return unit2str (xms_str, xms_strlen, sign,
                   3, field_width, field_precision,
		   field_scale, ':', radians);
}

int ra2str (char* rastring, unsigned rastrlen, double ra, unsigned places) 
{
  return xms2str (rastring, rastrlen, 0, ra, 24.0, places);
}

int dec2str2 (char* dstring, unsigned dstrlen, double dec, unsigned places) 
{
  return xms2str (dstring, dstrlen, 1, dec, 360.0, places);
}


