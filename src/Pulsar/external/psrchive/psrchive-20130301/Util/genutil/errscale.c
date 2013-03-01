/***************************************************************************
 *
 *   Copyright (C) 1993 by Paul Harrison
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

double errscale(char * valstr, char * errstr)
{
  double temp=1;
  int i=0;
  
  /* test for the cases where the error */
  /* represents the absolute value i.e. */
  /* if negative or if there is a */
  /* decimal point in string */

  if (errstr[0] == '-' || strchr(errstr,'.')!=NULL) return 1.0;
    
  if((valstr=strchr(valstr,'.'))!=NULL)
    {
      while (isdigit(valstr[++i]));
      if(valstr[i] == 'E' || valstr[i] == 'e')
	{
	  /* redwards 24 Mar 98 added 'e' case below since it was absent!*/
	  if (valstr[i]=='E')
	    sscanf(valstr+i,"E%lf",&temp);
	  else
	    sscanf(valstr+i,"e%lf",&temp);
	  temp = pow(10.0,temp);
	}
      else
	{
	  temp = 1;
	}
      temp *= pow(10.0,(double)(-(--i)));
      if ( temp>1.0 && (i=strlen(errstr))>1 && errstr[0] != '-') temp *=
	pow(10.0,(double)(--i));
      return temp;
    }
  else
    {
      return 1;
    }
}


