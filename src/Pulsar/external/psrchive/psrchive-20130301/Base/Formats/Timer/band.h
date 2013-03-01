/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/band.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __BAND_H
#define __BAND_H

struct band {
  double lo1;                 /* (T) MHz                                     */
  double lo2;                 /* (T) MHz                                     */
  double loUP;                /* (T) MHz                                     */
  double loDOWN;              /* (T) MHz                                     */
  double centrefreq;          /* (T/A) MHz CF of whole band                  */
  double bw;                  /* (T/A) Bandwidth of whole band               */
  float flux_A;               /* (A) average flux value in mJy (poln 0)      */
  int inv_mode;               /* 1=Q inverted, 2=U inverted, 3=V inverted    */
  int auto_atten;             /* (T) 1=true, 0 =false                        */

  /* this poln_storage_type thing has gone too far - WvS, 12 Aug, 2002 */
  /* (T) 0=I, 1=AA BB, 2=AA AB BA BB, 3=AA, 4=BB, 5=Inv, 6=IQUV */
  int correlator_mode;

  float f_atten_A;	      /* (T) New float attenuator values             */
  float f_atten_B;            /* (T) 0.0 - 31.5 db                           */
  int polar;                  /* (T) 0=circular (A=L,B=R),1=linear(A=0,B=90) */
  float feed_offset;          /* (T) (degrees)                               */
  int nlag;                   /* (T) no lags per poln                        */
  float flux_B;               /* (A) average flux value in mJy (poln 1)      */
  float flux_err;             /* (CA) error in flux in mJy                   */
  int npol;		      /* (T/Fixheader) npolarisations 1-4            */
};

/* defined in timer++.C */
void band_init (struct band * bd);

#endif
