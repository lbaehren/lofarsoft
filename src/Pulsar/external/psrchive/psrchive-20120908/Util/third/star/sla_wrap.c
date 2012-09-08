/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This file provides C wrappers to the Fortran SLALIB bindings.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* *********************************************************************** */

#define SLA_altaz F77_FUNC_(sla_altaz,slaAltaz)

void SLA_altaz (double*, double*, double*, double*, double*, double*,
		double*, double*, double*, double*, double*, double*);

void slaAltaz (double ha, double dec, double phi,
                double *az, double *azd, double *azdd,
                double *el, double *eld, double *eldd,
                double *pa, double *pad, double *padd)
{
  SLA_altaz (&ha, &dec, &phi,
	     az, azd, azdd,
	     el, eld, eldd,
	     pa, pad, padd);
}


/* *********************************************************************** */

#define SLA_dcs2c F77_FUNC_(sla_dcs2c,slaDcs2c)

void SLA_dcs2c (double* ra, double* dec, double* v);

void slaDcs2c (double a, double b, double v[3])
{
  SLA_dcs2c (&a, &b, v);
}


/* *********************************************************************** */

#define SLA_dmxv F77_FUNC_(sla_dmxv,slaDmxv)

void SLA_dmxv (double* dm, double* va, double* vb);

void slaDmxv (double dm[3][3], double va[3], double vb[3])
{
  SLA_dmxv (&(dm[0][0]), va, vb);
}

/* *********************************************************************** */

#define SLA_dsep F77_FUNC_(sla_dsep,slaDsep)

double SLA_dsep (double *, double *, double *, double*);

double slaDsep (double a1, double b1, double a2, double b2)
{
  return SLA_dsep (&a1, &b1, &a2, &b2);
}

/* *********************************************************************** */

#define SLA_dtt F77_FUNC_(sla_dtt,slaDtt)

double SLA_dtt (double* mjd);

double slaDtt (double dju)
{
  return SLA_dtt (&dju);
}

/* *********************************************************************** */

#define SLA_dvdv F77_FUNC_(sla_dvdv,slaDvdv)

double SLA_dvdv (double* va, double* vb);

double slaDvdv (double va[3], double vb[3])
{
  return SLA_dvdv (va, vb);
}

/* *********************************************************************** */

#define SLA_ecleq F77_FUNC_(sla_ecleq,slaEcleq)

void SLA_ecleq (double *, double *, double *, double *, double *);

void slaEcleq (double dl, double db, double mjd, double *dr, double *dd)
{
  SLA_ecleq (&dl, &db, &mjd, dr, dd);
}

/* *********************************************************************** */

#define SLA_epj F77_FUNC_(sla_epj,slaEpj)

double SLA_epj (double* mjd);

double slaEpj (double date)
{
  return SLA_epj (&date);
}

/* *********************************************************************** */

#define SLA_eqecl F77_FUNC_(sla_eqecl,slaEqecl)

void SLA_eqecl (double *, double *, double *, double *, double *);

void slaEqecl (double dr, double dd, double mjd, double *dl, double *db)
{
  SLA_eqecl (&dr, &dd, &mjd, dl, db);
}

/* *********************************************************************** */

#define SLA_eqgal F77_FUNC_(sla_eqgal,slaEqgal)

void SLA_eqgal (double *, double *, double *, double *);

void slaEqgal (double dr, double dd, double *dl, double *db)
{
  SLA_eqgal (&dr, &dd, dl, db);
}

/* *********************************************************************** */

#define SLA_evp F77_FUNC_(sla_evp,slaEvp)

void SLA_evp (double* tdb, double* ep,
              double* dvb, double* dpb,
              double* dvh, double* dph);

void slaEvp (double date, double deqx,
             double dvb[3], double dpb[3],
             double dvh[3], double dph[3])
{
  SLA_evp (&date, &deqx, dvb, dpb, dvh, dph);
}

/* *********************************************************************** */

#define SLA_galeq F77_FUNC_(sla_galeq,slaGaleq)

void SLA_galeq (double *, double *, double *, double *);

void slaGaleq (double dl, double db, double *dr, double *dd)
{
  SLA_galeq (&dl, &db, dr, dd);
}

/* *********************************************************************** */

#define SLA_gmst F77_FUNC_(sla_gmst,slaGmst)

double SLA_gmst (double* mjd);

double slaGmst (double ut1)
{
  return SLA_gmst (&ut1);
}

/* *********************************************************************** */

#define SLA_pa F77_FUNC_(sla_pa,slaPa)

double SLA_pa (double* HA, double* DEC, double* PHI);

double slaPa (double ha, double dec, double phi)
{
  return SLA_pa (&ha, &dec, &phi);
}

/* *********************************************************************** */

#define SLA_prec F77_FUNC_(sla_prec,slaPrec)

void SLA_prec (double* ep0, double* ep1, double* rmatp);

void slaPrec (double ep0, double ep1, double rmatp[3][3])
{
  SLA_prec (&ep0, &ep1, &(rmatp[0][0]));
}

