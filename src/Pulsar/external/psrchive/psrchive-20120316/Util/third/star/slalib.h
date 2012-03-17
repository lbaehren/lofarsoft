/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/third/star/slalib.h,v $
   $Revision: 1.1 $
   $Date: 2006/12/01 14:49:32 $
   $Author: straten $ */

#ifndef __SLA_DECL_H
#define __SLA_DECL_H

/*
  These C wrappers are required in order that the PSRCHIVE software may
  be linked against either the C or Fortran version of SLALIB.
*/

#ifdef __cplusplus
extern "C" {
#endif

void slaAltaz ( double ha, double dec, double phi,
                double *az, double *azd, double *azdd,
                double *el, double *eld, double *eldd,
                double *pa, double *pad, double *padd );

void slaDcs2c ( double a, double b, double v[3] );

void slaDmxv ( double dm[3][3], double va[3], double vb[3] );

double slaDsep ( double a1, double b1, double a2, double b2 );

double slaDtt ( double dju );

double slaDvdv ( double va[3], double vb[3] );

double slaEpj ( double date );

void slaEqgal ( double dr, double dd, double *dl, double *db );

void slaEvp ( double date, double deqx,
              double dvb[3], double dpb[3],
              double dvh[3], double dph[3] );

void slaGaleq ( double dl, double db, double *dr, double *dd );

double slaGmst ( double ut1 );

double slaPa ( double ha, double dec, double phi );

void slaPrec ( double ep0, double ep1, double rmatp[3][3] );

#ifdef __cplusplus
           }
#endif

#endif

