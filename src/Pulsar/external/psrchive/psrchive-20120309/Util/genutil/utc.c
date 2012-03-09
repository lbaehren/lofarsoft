/***************************************************************************
 *
 *   Copyright (C) 1998 by mbritton Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "utc.h"

/* *************************************************************************
   str2utc - converts a string of characters into a UTC time.

   "str"  contains a string representation of UT time.  This string 
          will be parsed as follows:

	  Instead of typing a long-winded explanation, here are some examples:

	  str == "1997251043045"                ->  1997-251-04-30-45
	  
	  str == "345"                          ->  0000-000-00-03-45
	  
	  str == "1997-51-4-3:45"               ->  1997-051-04-03-45

	  str == "test1997hard251a4is30this45"  ->  1997-251-04-30-45

   "time" will return the UT time represented by "str"
   ************************************************************************* */
int str2utc (utc_t *time, const char* str)
{
  char* temp;
  int   trav;
  int   endstr;
  char  infield;
  int   field_count;
  int   digits;

  time->tm_year = 0;
  time->tm_hour = 0;
  time->tm_yday = 0;
  time->tm_min = 0;
  time->tm_sec = 0;

  temp = strdup (str);

  /* count the number of fields and cut the string off after a year, day,
     hour, minute, and second can be parsed */
  trav = 0; infield = 0;
  field_count = digits = 0;
  while (temp[trav] != '\0') {
    if (isdigit(temp[trav])) {
      digits ++;
      if (!infield) {
	/* count only the transitions from non digits to a field of digits */
	field_count ++;
      }
      infield = 1;
    }
    else {
      infield = 0;
    }
    if (field_count == 5) {
      /* currently in the seconds field */
      temp[trav+2] = '\0';
      break;
    }
    else if (digits == 13) {
      /* enough digits for a date */
      temp[trav+1] = '\0';
      break;
    }
    trav ++;
  }

  endstr = strlen(temp);
  /* cut off any trailing characters that are not ASCII numbers */
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;

  if (endstr < 0)
    return -1;

  temp [endstr+1] = '\0'; 

  /* parse UTC seconds */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_sec));

  /* cut out seconds and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC minutes */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_min));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC hours */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_hour));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC days */
  trav = endstr;
  while ((trav >= 0) && (endstr-trav < 3) && isdigit(temp[trav]))
    trav--;
  ++trav;  /* when loop ends, trav has gone too far, or points to a nondigit */
  sscanf (temp+trav, "%3d", &(time->tm_yday));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC year */
  trav = endstr;
  while ((trav >= 0) && (endstr-trav < 4) && isdigit(temp[trav]))
    trav--;
  sscanf (temp+trav+1, "%4d", &(time->tm_year));

  free (temp);
  return 0;
} 

int tm2utc (utc_t *time, struct tm calendar)
{
  int days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int month;

  time->tm_year = 1900 + calendar.tm_year;
  if (UTC_LEAPYEAR(time->tm_year)) {
    days_in_month[1] = 29;
  }
  time->tm_yday = 0;
  for (month=0; month<calendar.tm_mon; month++) {
    time->tm_yday += days_in_month[month];
  }
  time->tm_yday += calendar.tm_mday;
  time->tm_hour = calendar.tm_hour;
  time->tm_min = calendar.tm_min;
  time->tm_sec = calendar.tm_sec;

  return 0;
}

int utc2tm (struct tm *calendar, utc_t time)
{
  /* the struct tm has the following data members:
     tm_sec       Seconds after the minute [0-60]
     tm_min       Minutes after the hour [0-59]
     tm_hour      Hours since midnight [0-23]
     tm_mday      Day of the month [1-31]
     tm_mon       Months since January [0-11]
     tm_year      Years since 1900
   */

  int days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int month;
  int day_of_year;
  time_t date;

  day_of_year = time.tm_yday;

  while (day_of_year < 1) {
    time.tm_year --;
    day_of_year += UTC_JULIANDAYS (time.tm_year);
  }

  if (UTC_LEAPYEAR(time.tm_year)) {
    days_in_month[1] = 29;
  }

  for (month=0; month<12 && day_of_year>0; month++)  {
    day_of_year -= days_in_month[month];
  }
  month --;

  if (day_of_year > 0)
    return -1;

  calendar->tm_year  = time.tm_year - 1900;
  calendar->tm_mon   = month;
  calendar->tm_mday  = day_of_year + days_in_month[month];
  calendar->tm_hour  = time.tm_hour;
  calendar->tm_min   = time.tm_min;
  calendar->tm_sec   = time.tm_sec;

  calendar->tm_isdst = -1;

  date = mktime (calendar);
  /* fprintf (stderr, 
     "utc2tm: month b4:%d after:%d\n", month, calendar->tm_mon); */
  if (date == (time_t)-1)
    return -1;
  return 0;
}


/* *************************************************************************
   utc2str -
   This function returns a string representation of the time contained
   in "time".  The format of this string is given in "fmt" and the
   string is returned in "tmstr".

   tmstr - the returned string
   time  - a utc_t struct
   fmt   - the format of the string returned in tmstr

   Any characters in the set {y, d, h, m, s} will be replaced
   by the respective character of the time.  For instance:

   if   "fmt"  == "yyyy-dddd-hhhh-mm-ss"
   and  "time" == tm_year = 1995
                  tm_yday = 273
                  tm_hour = 13
                  tm_min  = 14
                  tm_sec  = 30
   then "tmstr will be "1995-0273-0013-14-30"

   Notice that any extra characters will be filled with zeroes.  The
   charaters are filled so that the least significant digits of each
   value are enterred first,  so fmt == "yy" would return "95" in
   the above example.  All other characters are copied literally.
   NOTE: "tmstr" must point to an area as large as or larger than
   the space to which "fmt" points or a sementation fault may occur.
   ************************************************************************* */
char* utc2str (char* tmstr, utc_t time, const char* fmt)
{
   strcpy (tmstr, fmt);
   return (utcstrfill (tmstr, time, strlen(fmt)));
}

/* *************************************************************************
   utcstrfill - does the work for "utc2str" as described above.

   tmstr - a string of "numch" characters containing the format of
           the desired output string.  It's characters will be replaced
           with the string representation of "time" as described above.
   time  - the UTC time
   numch - the number of characters in "tmstr"
   ************************************************************************* */
char* utcstrfill (char* tmstr, utc_t time, int numch)
{
  int pos;                 /* position in "tmstr" */
  
  for (pos = numch -1; pos >= 0; pos--)  {
    switch (tmstr[pos])  {
    case 'y':
      tmstr[pos] = (char) (time.tm_year % 10) + '0';
      time.tm_year /= 10;
      break;
    case 'd':
      tmstr[pos] = (char) (time.tm_yday % 10) + '0';
      time.tm_yday /= 10;
      break;
    case 'h':
      tmstr[pos] = (char) (time.tm_hour % 10) + '0';
      time.tm_hour /= 10;
      break;
    case 'm':
      tmstr[pos] = (char) (time.tm_min % 10) + '0';
      time.tm_min /= 10;
      break;
    case 's':
      tmstr[pos] = (char) (time.tm_sec % 10) + '0';
      time.tm_sec /= 10;
      break;
    default:
      break;
    }
  }
  return (tmstr);
}

#if 0
char* calstrfill (char* tmstr, cal_t date, int numch);

char* cal2str (char* tmstr, cal_t date, const char* fmt)
{
  strcpy (tmstr, fmt);
  return (calstrfill (tmstr, date, strlen(fmt)));
}

char* calstrfill (char* tmstr, cal_t date, int numch)
{
  int pos;                 /* position in "tmstr" */

  for (pos = numch -1; pos >= 0; pos--)  {
    switch (tmstr[pos])  {
    case 'y':
      tmstr[pos] = (char) (date.tm_year % 10) + '0';
      date.tm_year  /= 10;
      break;
    case 'M':
      tmstr[pos] = (char) (date.tm_month % 10) + '0';
      date.tm_month /= 10;
      break;
    case 'd':
      tmstr[pos] = (char) (date.tm_day % 10) + '0';
      date.tm_day   /= 10;
      break;
    case 'h':
      tmstr[pos] = (char) (date.tm_hour % 10) + '0';
      date.tm_hour  /= 10;
      break;
    case 'm':
      tmstr[pos] = (char) (date.tm_min  % 10) + '0';
      date.tm_min   /= 10;
      break;
    case 's':
      tmstr[pos] = (char) (date.tm_sec  % 10) + '0';
      date.tm_sec   /= 10;
      break;
    default:
      break;
    }
  }
  return (tmstr);
}

#endif

/* *************************************************************************
   utc_diff - returns the number of seconds between
              time1 and time2 (ie. time2 - time1).
   ************************************************************************* */
int utc_diff (utc_t time1, utc_t time2)
{
   while (time2.tm_year > time1.tm_year)  {
#ifdef UTC_DEBUG
         printf ("1: Julian Days in %i: %i\n", time2.tm_year -1,
                               UTC_JULIANDAYS (time2.tm_year -1) );
#endif
      time2.tm_year --;
      time2.tm_yday += UTC_JULIANDAYS (time2.tm_year);
   }
   while (time1.tm_year > time2.tm_year)  {
#ifdef UTC_DEBUG
         printf ("2: Julian Days in %i: %i\n", time1.tm_year -1,
                               UTC_JULIANDAYS (time1.tm_year -1) );
#endif
      time1.tm_year --;
      time1.tm_yday += UTC_JULIANDAYS (time1.tm_year);
   }

   return((time2.tm_sec - time1.tm_sec)
           + ((time2.tm_min - time1.tm_min) * 60)
           + ((time2.tm_hour - time1.tm_hour) * 3600)
           + ((time2.tm_yday - time1.tm_yday) * 86400));
}

/* *************************************************************************
   utc_inc - increments "time" by "seconds"
   ************************************************************************* */
int utc_inc (utc_t *time, int seconds)
{
   time->tm_sec += seconds;

   time->tm_min += time->tm_sec / 60;
   time->tm_sec = time->tm_sec % 60;

   time->tm_hour += time->tm_min / 60;
   time->tm_min = time->tm_min % 60;

   time->tm_yday += time->tm_hour / 24;
   time->tm_hour = time->tm_hour % 24;

   while (time->tm_yday >= UTC_JULIANDAYS(time->tm_year))  {
#ifdef UTC_DEBUG
         printf ("Julian day in %i: %i\n",time->tm_year,
                                          UTC_JULIANDAYS(time->tm_year));
#endif
      time->tm_year ++;
      time->tm_yday -= UTC_JULIANDAYS(time->tm_year);
   }
   return (0);
}

/* *************************************************************************
   utc_dec - decrements "time" by "seconds"
   ************************************************************************* */
int utc_dec (utc_t *time, int seconds)
{
   time->tm_sec -= seconds;

   if (time->tm_sec < 0)  {
      time->tm_min += ((time->tm_sec / 60) - 1);
      time->tm_sec = (time->tm_sec % 60) + 60;
   }

   if (time->tm_min < 0)  {
      time->tm_hour += ((time->tm_min / 60) - 1);
      time->tm_min = (time->tm_min % 60) + 60;
   }

   if (time->tm_hour < 0)  {
      time->tm_yday += ((time->tm_hour / 24) - 1);
      time->tm_hour = (time->tm_hour % 24) + 24;
   }

   while (time->tm_yday < 0)  {
      time->tm_year --;
      time->tm_yday += UTC_JULIANDAYS(time->tm_year);
   }

   return (0);
}

int utc_leapyear (int year)
{
   return ((year%4 == 0) && (year%400 != 0));
}

int utc_daysinyr (int year)
{
   return ( 364 + ( (utc_leapyear (year))?1:0 ) );
}

int str2LST (double* lst, char* timestr, float longitude)
{
  utc_t timeutc;

  str2utc (&timeutc, timestr);
  return utc2LST (lst, timeutc, longitude);
}

/* longitude is given in degrees */
int utc2LST (double* lst, utc_t timeutc, float longitude)
{
  return utc_f2LST (lst, timeutc, 0.0, longitude);
}

#define UTC2LST_FIRST_YEAR   1996
#define UTC2LST_TOTAL_YEARS  4

int utc_f2LST (double* lst, utc_t timeutc, double fracsec, float longitude)
{
  /* Greenwich mean sidereal time (hours) at 12AM (UTC) day zero.
     see ASTRO ALMANAC pg B6 */
  double gmst0 [UTC2LST_TOTAL_YEARS] = { 6.5967564, /* 1996 */
					 6.6465521, /* 1997 */
					 6.6306380, /* 1998 */
					 6.6147239  /* 1999 */
  };
  double F1 = 0.0;
  double F2 = 0.0657098244;     /* (hours) */
  double F3 = 1.00273791;       /* (hours) */

  double UT = 0.0;
  double GMST = 0.0;

  /* convert longitude in degrees to east_long in hours */
  double east_long = longitude * (24.0 / 360.0);
  
  if ( (timeutc.tm_year < UTC2LST_FIRST_YEAR) ||
       (timeutc.tm_year >= UTC2LST_FIRST_YEAR + UTC2LST_TOTAL_YEARS) )  {
    fprintf(stderr,"Hello, this a utc2LST WARNING\n");
    fprintf(stderr,"The year %d is out of range\n",timeutc.tm_year);
    fprintf(stderr,"Enter revised conversion formula\n");
    fprintf(stderr,"from page B6 of current ASTRO ALMANAC\n");
    return -1;
  }
  F1 = gmst0 [timeutc.tm_year - UTC2LST_FIRST_YEAR];
  
  UT = (double)timeutc.tm_hour + (double)timeutc.tm_min/60.0
    + ((double)timeutc.tm_sec + fracsec)/3600.0;

  GMST = F1 + F2*(double)timeutc.tm_yday + F3*UT;
  
  *lst =  GMST + east_long;
  while (*lst<0.0) *lst+=24.0;
  if(*lst>=24.0) *lst=(float) fmod((double) *lst,24.0);
  return 0;
}
