
/****************************************************************/
/*                                                              */
/*         Convert BAT to UT in string form.                    */
/*                                                              */
/*   If BAT < 0 the time is assumed to be a time difference     */
/*   and the output is in the form  DDDD HH:MM:SS.sss           */
/*   where DDDD repesents the number of days.                   */
/*                                                              */
/*   If BAT > 0 the time is assumed to be absolute time         */
/*   and the output format is determined by len.                */
/*                                                              */
/*   The len parameter on input defines both:                   */
/*          the length of the output string                     */
/*     AND  the format of the output string                     */
/*                                                              */
/*     If len <  24 output format is HH:MM:SS.sss               */
/*     if len >= 24 output format is YYYY-MM-DD HH:MM:SS.sss    */
/*                                                              */
/****************************************************************/

/* $Id: bat_to_utc.c,v 1.3 2009/08/24 18:50:03 straten Exp $ */

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Number of sec from BAT zero to 1-JAN-1970 00:00:00 UTC */
#define TOD2BAT 3506716800LL

/* No. of usec in a day */
#define USECinDAY 86400000000LL

#define STR_LEN 40

/* If bat is negative it is interpreted as a delta time and
 the result is in elapsed DAYS HH:MM:SS, otherwise only absolute HH:MM:SS */

int bat_to_ut_str( char *str, int len, long long bat )
{
  /*
    struct timeval ttv;
    struct timezone ttz;
  */

  char *pch;
  long long li, lj, lldutc, days;
  time_t tt;
  int n;
  double xs;
  char l_str[STR_LEN];

  /* BEGIN */

  str[0] = '\0';
  n = 0;
  
  if( bat < 0 ) {
    
    lj = -bat;
    days = lj / USECinDAY;
    lj -= ( days * USECinDAY );
    
    sprintf( l_str, "%lld ", days );
    n += strlen( l_str );
    if( n < len ) strcat( str, l_str );
    else return( -1 );
    
    li = ( lj / 1000000LL );
    xs = (double) ( (long long)( lj - li * 1000000LL ) ) / 1000000.0;
      
  }
  else {
    
    if( ( pch = getenv( "DUTC" ) ) == NULL ) 
      lldutc = 34;
    else if( sscanf( pch, "%lld", &lldutc ) != 1 )
      return -3;

    li = ( bat / 1000000LL );
    xs = (double) ( (long long)( bat - li * 1000000LL ) ) / 1000000.0;
  
    li = li - TOD2BAT - lldutc;
    
  }
  
  tt = (int) li;

  if( xs > 0.999 ) {
    xs = 0.0;
    tt++;
  }

  if( len < 24 ) {
    
    n += strftime( l_str, STR_LEN, "%H:%M:%S", gmtime( &tt ) );
    if( n < len ) strcat( str, l_str );
    else return( -4 );
    
  }
  else {
    
    n += strftime( l_str, STR_LEN, "%Y-%m-%d-%H:%M:%S", gmtime( &tt ) );
    if( n < len ) strcat( str, l_str );
    else return( -4 );
    
  }
  
  return( strlen( str ) );
  
}




int main (int argc, char** argv)
{
  /* first BAT argument */
  unsigned long bat0;
  /* last BAT argument */
  unsigned long bat1;
  /* BAT */
  long long bat;
  /* UTC string */
  char utc [STR_LEN];

  if (argc < 3)
  {
    fprintf (stderr,
	     "USAGE: bat_to_utc <BAT0> <BAT1>\n"
	     "BAT values are two halves of 64-bit number in hexadecimal\n");
    return -1;
  }

  sscanf (argv[2], "%lu", &bat0);
  sscanf (argv[1], "%lu", &bat1);

  bat = bat0;
  bat <<= 32;
  bat |= bat1;

  if (bat_to_ut_str( utc, STR_LEN, bat ) < 0)
  {
    fprintf (stderr, "bat_to_utc: failed to parse %lu %lu\n", bat0, bat1);
    return -1;
  }

  fprintf (stdout, "%s\n", utc);
  return 0;
}

