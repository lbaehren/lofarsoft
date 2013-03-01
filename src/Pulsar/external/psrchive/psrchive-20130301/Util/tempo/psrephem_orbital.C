/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "psrephem_orbital.h"
#include "tempo++.h"
#include "coord.h"
#include "orbital.h"
#include "file_cast.h"

#include <slalib.h>

#include <iostream>

using namespace std;
using Legacy::psrephem;

const psrephem* new_psrephem (const Pulsar::Parameters* parameters)
{
  return dynamic_file_cast<const psrephem>( parameters );
}

// Binary phase wrt periastron at UTC MJD
double get_binphs_peri(double mjd, const psrephem& eph, double freq, char site)
{
  // mjd to TDB 
  double bcmjd;
  if((bcmjd = get_bcmjd(mjd, eph, freq, site)) < 0.){
    return (bcmjd);
  }

  // Phase at mjd 
  double binphs = (bcmjd - eph.t0())/eph.pb(); // Note: pbdot not included
  binphs = fmod(binphs,1.0);                   // Return between 0 and 1
  if(binphs < 0.0)binphs += 1.0;

  return(binphs);
}

// Binary phase wrt ascending node at UTC MJD
double get_binphs_asc(double mjd, const psrephem& eph, double freq, char site)
{
  // mjd to TDB 
  double bcmjd;
  if((bcmjd = get_bcmjd(mjd, eph, freq, site)) < 0.){
    return (bcmjd);
  }

  // Phase of periastron wrt ascending node
  double periphs;
  periphs = get_periphs(bcmjd, eph);
  
  // Phase at mjd wrt ascending node
  double binphs = (bcmjd - eph.t0())/eph.pb(); // Note: pbdot not included
  binphs += periphs;                           // Refer to ascending node
  binphs = fmod(binphs,1.0);                   // Return between 0 and 1
  if(binphs < 0.0)binphs += 1.0;

  return(binphs);
}

// Binary longitude wrt periastron at UTC MJD
double get_binlng_peri(double mjd, const psrephem& eph, double freq, char site)
{
  double bcmjd, ma, ea, ta, binlng;

  // mjd to TDB 
  if((bcmjd = get_bcmjd(mjd, eph, freq, site)) < 0.){
    return (bcmjd);
  }

  // mean anomaly at bcmjd (rad)
  ma = 2.0*M_PI*(bcmjd - eph.t0())/eph.pb(); // Note: pbdot not included

  // Get eccentric anomaly (rad) by solving Kepler's equation
  ea = eccentric_anomaly (ma, eph.ecc());

  // True anomaly 
  ta = 2.0*atan(sqrt((1.0+eph.ecc())/(1.0-eph.ecc()))*tan(ea/2.0));

  // Longitude at bcmjd (deg)
  binlng = ta * 180.0/M_PI;
  binlng = fmod(binlng,360.0);
  if(binlng < 0.0)binlng += 360.0;

  return(binlng);
}

double get_binlng_asc(double mjd, const psrephem& eph, double freq, char site)
{
  double bcmjd, ma, ea, ta, omega, binlng;

  // mjd to TDB 
  if((bcmjd = get_bcmjd(mjd, eph, freq, site)) < 0.){
    return (bcmjd);
  }

  // mean anomaly at bcmjd (rad)
  ma = 2.0*M_PI*(bcmjd - eph.t0())/eph.pb(); // Note: pbdot not included

  // Get eccentric anomaly (rad) by solving Kepler's equation
  ea = eccentric_anomaly (ma, eph.ecc());

  // True anomaly 
  ta = 2.0*atan(sqrt((1.0+eph.ecc())/(1.0-eph.ecc()))*tan(ea/2.0));
  ta *= 180.0/M_PI;      // (deg)

  // Longitude of periastron at bcmjd (deg)
  omega = eph.omega() + (bcmjd - eph.t0())/365.25 * eph.omdot();
  
  // Longitude at bcmjd (deg)
  binlng = ta + omega;
  binlng = fmod(binlng,360.0);
  if(binlng < 0.0)binlng += 360.0;

  return(binlng);
}

double get_bcmjd(double mjd, const psrephem& eph, double freq, char site)
{
  // MJD to TDT
  double tdt = mjd + slaDtt(mjd)/86400.;

  // TDT to TDB
  double g = (357.53 + 0.9856003*(mjd - 51544.5))*M_PI/180.;
  double tdb = tdt + (0.001658*sin(g) + 0.000014*sin(2.0*g))/86400.;

  string site1 = "a";
  site1[0] = site;
  const Tempo::Observatory* obs = Tempo::observatory (site1);

  // Observatory coordinates
  double xx, yy, zz;
  obs->get_xyz (xx,yy,zz);

  const double AU_M = 1.495979E11;
  double re = sqrt(xx*xx + yy*yy)/AU_M;
  double alng = atan2(-yy, xx);      // West longitude
  double ph = slaGmst(mjd) - alng;

  double eeq[3], eej[3];   // Obs locn in mean coords of date, J2000.
  eeq[0] = re * cos(ph);
  eeq[1] = re * sin(ph);
  eeq[2] = zz/AU_M;

  // Precession matrix to J2000
  double epj, rmatp[3][3], ep2000;
  epj = slaEpj(mjd);
  ep2000=2000.0;

  slaPrec(epj,ep2000,rmatp);
  slaDmxv(rmatp,eeq,eej);

  // Source vector
  double dps[3], raj, decj;
  raj = 2.0*M_PI*eph.jra();
  decj= 2.0*M_PI*eph.jdec();
  slaDcs2c(raj,decj,dps);

  // Earth centre position wrt barycentre
  double dvb[3], dpb[3], dvh[3], dph[3];
  slaEvp(tdb,ep2000,dvb,dpb,dvh,dph);

  // Observatory position wrt barycentre
  dpb[0] += eej[0];
  dpb[1] += eej[1];
  dpb[2] += eej[2];

  // Light time to barycentre
  double aus = 499.0047837;
  double dpt = aus * slaDvdv(dpb,dps);

  // Correct to infinite frequency
  double dmdt = 4149.38 * eph.get_dm()/(freq*freq);

  double bcmjd = tdb + (dpt-dmdt)/86400.;

  return(bcmjd);
}

double get_periphs(double bcmjd, const psrephem& eph)
{
  double omega   = 0.0;
  double tgE2    = 0.0;
  double sinE    = 0.0;
  double E       = 0.0;
  double ma_asc  = 0.0;
  double periphs = 0.0;
  
  omega = eph.omega() + (bcmjd - eph.t0())/365.25 * eph.omdot();
  if(omega != 180.0){
    tgE2 = tan(omega/2.0*M_PI/180.)*sqrt((1.0-eph.ecc())/(1.0+eph.ecc()));
    sinE = 2.0 * tgE2/(1.0 + tgE2*tgE2);

    if(tgE2 <= 1.0 && tgE2 >= 0.0){
      E = asin(sinE);                  // 0 <= E <= 90
    }
    else if(tgE2 > 1.0){
      E = M_PI - asin(sinE);           // 90 < E < 180
    }
    else if(tgE2 < -1.){
      E = -M_PI -asin(sinE);           // 180 < E < 270
    }
    else if(tgE2 >= -1. && tgE2 < 0.0){
      E = asin(sinE);                  // 270 <= E < 0
    }

    ma_asc = E - eph.ecc()*sin(E);     // Mean anomaly of ascending node
  }
  else{
    E = M_PI;
    ma_asc = E;
  }

  periphs = ma_asc/(2.0*M_PI);
  if(periphs < 0.0){
    periphs = 1.0 + periphs;
  }

  return(periphs);
}

