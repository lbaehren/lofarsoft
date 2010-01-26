/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/mini.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

/* include file for mini header */

/* This has to be compatable with fortran when written as binary to a file.
   It is *****ABSOLUTELY VITAL***** that eight byte boundaries are not crossed
   All characters should have a length equal to a multiple of 8 
   In any section of the structure (between character or real*8 declarations)
   there must be an even number of reals and integers */
 
/* If you cant do this with actual variables put in pads (eg pad1 below)
   Space is declared at the end to fill up to 128 bytes. */
 
/* If you declare new variables you should subtract the appropriate no of bytes
   real = 4, integer = 4, real*8 = 8 etc on 32 bit architecture  */
 
/* Dont forget to alter version numbers here and in all programs using this 
   there is a list in header.uses */
 
/* Version 0.99 MXB JFB 08-MAR-1994 */
/* Version 1.00 JFB     16-MAR-1994 space added to fill to 128 bytes */
/* Version 1.10 WvS     23-SEP-1999 added an integration time so sub_ints
                                    can have different times */

#ifndef __MINI_H
#define __MINI_H

struct mini{
  int    junk;     
  int    junk2;
  int    junk3;
  int    mjd;         /* integer part of mjd              */
  double fracmjd;     /* fractional part of mjd           */
  double lst_start;   /* (G) lst in seconds               */
  double pfold;       /* (G) period in seconds            */
  float  tel_zen;     /* (G) Zenith in degrees            */
  float  tel_az;      /* (G) Az in degrees                */
  float  feed_ang;    /* (G) Feed angle in degrees        */
  float  para_angle;  /* (G) Parallactic Angle in degrees */
  float  version;
  float  pulse_phase; /* expected pulse arrival phase     */
  float  flux_A;      /* flux in poln 0                   */
  float  flux_B;      /* flux in poln 1                   */
  double integration; /* time integrated, in seconds      */
  char   space[48];
};

#endif
