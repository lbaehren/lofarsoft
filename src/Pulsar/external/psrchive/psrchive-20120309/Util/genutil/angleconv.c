/***************************************************************************
 *
 *   Copyright (C) 1993 by Paul Harrison
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>

int dms2rad ( char * instr, double * radval)
{
    auto int isign = 1, 
	     id,                     
	     im,
	     nfound;
    char    * ptr ;
    double secs ;

/*
   find negative sign if present, and advance past it storing the negative sign
*/
    if ((ptr = strstr (instr, "-")) != NULL)
    {
    	isign = -1;  
	instr = ++ptr;
    }
    else if((ptr = strstr (instr, "+")) != NULL)
    {
	isign = 1;
	instr = ++ptr;
    }

    if ((nfound = sscanf (instr, "%d%*c%2d%*c%lf", &id, &im, &secs)) > 0)
    {
	*radval = id;
	if (--nfound)
	{
	    *radval += (double)im / 60;
	    if (--nfound)
		*radval += secs / 3600;
	}
	*radval *= isign * M_PI / 180;
	return 1;                           /* conversion succeeded */
    }
    else
    {                   
    	return 0;			    /* conversion failed */
    }
}

/*--- end of function */

/*
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*N Name:
*N       dmserr2rad
*P Purpose:
*P       convert degrees minutes seconds into radians
*F Call From:
*F       C
*C Call:
*C       dms2rad ( instr, inerr, radval )
*A Arguments:
*A       - char * instr (Input)
*A           the character string containing the sexadecimal notation
*A       - char * inerr (Input)
*A           the character string containing the error
*A	 - double * radval (Output)
*A           the equivalent value in radians
*R Return value:
*R       1 for successful completion, 0 for unsuccessful
*E Externals:
*E       {@tbs@}...
*I Initial Release:
*I       Paul Harrison  3-MAR-1991
*H Modification Record:
*H       {@history_entry@}...
*O [@optional prelude items@]...
-----------------------------------------------------------------------
*/


int dmserr2rad ( char * instr, char * inerr, double * radval)
{
    auto int id,                     
	     im,
	     nfound;
    char    * ptr ;
    double secs ;
    extern double errscale(char*,char*);

    if ((nfound = sscanf (instr, "%d%*c%2d%*c%lf", &id, &im, &secs)) > 1)
      {
	if (sscanf(inerr,"%lf",radval)!=1)
	{
	  fprintf(stderr,"BAD position/ error");
	  *radval=0.0;
	  return 0;    /* no error provided.. */
	}
	if(nfound <=2)
	  {
	    *radval *=  M_PI / 180.0 / pow(60.0, (double)(nfound-1));
	  }
	else
	  {
	    ptr=strtok(instr," :");
	    ptr=strtok(NULL," :");
	    ptr=strtok(NULL," :");
	    *radval *= errscale(ptr,inerr);
	    *radval *= M_PI / 180.0 / 3600.0;
	  }
      }
    else
      {
	fprintf(stderr,"BAD position/ error");
	*radval=0.0;
      }
    return 0;
}


void rad2dms ( double * radval, char * sign, int * deg,
	      int * min, double * sec)
{
  double radt = * radval / M_PI * 180.0;

  *sign = (radt < 0.0) ?  '-' : '+';
  radt = fabs(radt);
  *deg = (int) floor( radt + 1e-11 );
  radt -= *deg;
  radt *= 60;
				/* rounding error correction */
  *min = (int) floor(radt + 1e-11);
  radt -= *min;
  *sec = radt < 0.0 ? 0.0 : (radt * 60) ;
}

void makebname( double * ra, double * raerr, double * dec, double * decerr, char * name)
{
  double sec, tmp = *ra/15;
  int deg, min;
  char isign;

  name[0]='B';
  rad2dms(&tmp, &isign, &deg, &min, &sec);
  sprintf(name+1,"%02d%02d",deg,min);
  rad2dms(dec, &isign, &deg, &min, &sec);
  sprintf(name+5,"%c%02d",isign,deg);
  name[8]='\0';
}
 

void makejname( double * ra, double * raerr, double * dec, double * decerr, char * name)
{
  double sec, tmp = *ra/15;
  int deg, min;
  char isign;

  name[0]='J';
  rad2dms(&tmp, &isign, &deg, &min, &sec);
  sprintf(name+1,"%02d%02d",deg,min);
  rad2dms(dec, &isign, &deg, &min, &sec);
  sprintf(name+5,"%c%02d%02d",isign,deg,min);
  if(*decerr > 0.0 && (*decerr / M_PI * 180.0 * 60) > 1.0 ) /* truncate */
    fprintf(stderr,"Recommend renaming to %s\n",name);						       /* name if necessary */
    /*    name[8]='\0'; */
    name[10]='\0';
}
 
char * crad2dms( double pos, int ra, char * name )
{
  double sec, tmp;
  int deg, min;
  char isign;

  tmp = ra ? pos/15 : pos;

  rad2dms(&tmp, &isign, &deg, &min, &sec);
  if(ra)
    {
      sprintf(name,"%02d:%02d:%06.3f",deg,min,sec);
    }
  else
    {
      sprintf(name,"%c%02d:%02d:%06.3f",isign,deg,min,sec);
    }
  return name;
}
      
char * crad2dmse( double pos, double err, int ra, char * name )
{
  double sec, sece, tmp, tmpe;
  int deg, min, ip;
  char isign;
  char * it, fmt[16];
  int expo,i;
  double mant;
  int  ierr;

  tmp = ra ? pos/15 : pos;
  tmpe = ra ? err/15 : err;


  rad2dms(&tmp, &isign, &deg, &min, &sec);
  if(ra)
    {
      ip=sprintf(name,"%02d:%02d:%06.3f",deg,min,sec);
    }
  else
    {
      ip=sprintf(name,"%c%02d:%02d:%06.3f",isign,deg,min,sec);
    }

  rad2dms( &tmpe, &isign, &deg, &min, &sece );
  if(tmpe > 0.0)
    {

      /* assume that the error will be less than degrees!! */
      if(min)
	{
	  for( it = name + ip -7; *it; *(it++)=' ');
	  sprintf(name+ip," %2d",min);
	}
      else
	{
	  mant = log10 ( sece );
	  expo = (int) fabs(floor(mant));
	  if(sece < 1.0 )
	    {
	      mant = sece * pow ( 10.0, (double)expo );
	      ierr =
		mant - floor ( mant ) < .5 ? (int) floor(mant) : (int) ceil(mant);
	      sprintf(fmt,"%%0%d.%dlf %%%dd",3 + expo - (expo == 0),
		      expo, 5-expo- (expo==0));
	      i=sprintf(name+ip-6,fmt,sec,ierr);
	    }
	  else 
	    {
	      ierr =
		sece - floor ( sece ) < .5 ? (int) floor(sece) : (int) ceil(sece);
	      sprintf(name+ip-6,"%02.0lf     %2d",sec,ierr);
	    }
	}
    }
  else
    {
      ierr=(int)err;
      sprintf(name+ip," %2d",ierr);
    }
  return name;
}

/* function to grab the suffix from one string to another, eg in
  a name like 1224-5432A */
void add_name_suffix(char * inname, char *outname)
{
  size_t i;
  
  i=strspn(inname+1,"0123456789+-");
  strcat(outname,inname+1+i);
}











