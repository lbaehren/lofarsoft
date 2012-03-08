/***************************************************************************
 *
 *   Copyright (C) 2010 by Jonathan Khoo, Albert Teoh, Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Barycentre.h"
#include "slalib.h"

Barycentre::Barycentre ()
{
  built = false;
  verbose = false;
}

void Barycentre::set_epoch (const MJD& mjd)
{
  epoch = mjd;
  built = false;
}

void Barycentre::set_coordinates (const sky_coord& sky)
{
  coordinates = sky;
  built = false;
}

void Barycentre::set_observatory_xyz (double x, double y, double z)
{
  observatory = Vector<3,double> (x,y,z);
  built = false;
}

double Barycentre::get_Doppler () const
{
  if (!built)
    build ();

  return Doppler;
}

MJD Barycentre::get_barycentric_epoch () const
{
  if (!built)
    build ();

  return barycentric_epoch;
}


/* ******************************************** */
/* lmst2                                         */
/* Author:  G. Hobbs (06 May 2003)              */
/* Purpose: Calculates the local mean sidereal  */
/*          time (tsid) and its derivative      */
/*                                              */
/* Inputs:                                      */
/* Outputs: tsid, tsid_der                      */
/*                                              */
/* Notes: based on lmst.f                       */
/*        NONE of this has been checked         */
/* Changes:                                     */
/* ******************************************** */

double lmst2(double mjd,double olong,double *tsid,double *tsid_der)
{
  double xlst,sdd;
  double gmst0;
  double a = 24110.54841;
  double b = 8640184.812866;
  double c = 0.093104;
  double d = -6.2e-6;
  double bprime,cprime,dprime;
  double tu0,fmjdu1,dtu,tu,seconds_per_jc,gst;
  int nmjdu1;

  nmjdu1 = (int)mjd;
  fmjdu1 = mjd - nmjdu1;

  tu0 = ((double)(nmjdu1-51545)+0.5)/3.6525e4;
  dtu  =fmjdu1/3.6525e4;
  tu = tu0+dtu;
  gmst0 = (a + tu0*(b+tu0*(c+tu0*d)))/86400.0;
  seconds_per_jc = 86400.0*36525.0;

  bprime = 1.0 + b/seconds_per_jc;
  cprime = 2.0 * c/seconds_per_jc;
  dprime = 3.0 * d/seconds_per_jc;

  sdd = bprime+tu*(cprime+tu*dprime);

  gst = gmst0 + dtu*(seconds_per_jc + b + c*(tu+tu0) + d*(tu*tu+tu*tu0+tu0*tu0))/86400;
  xlst = gst - olong/360.0;

  xlst = fmod(xlst,1.0);

  if (xlst<0.0)xlst=xlst+1.0;

  *tsid = xlst;
  *tsid_der = sdd;
  return 0.0;
}

double sqr (double x) { return x*x; }

void Barycentre::build () const
{
  double in_days = epoch.in_days();
  double in_seconds = epoch.in_seconds();

  // Astronomical unit in light seconds
  double AUS     = 499.004786;

  double TWOPI   = 2*M_PI;
  double RTOD    = (double)360/TWOPI;
  
  Angle ra_angle = coordinates.ra();
  Angle dec_angle = coordinates.dec();

  int hours, minutes;
  double seconds;
  ra_angle.getHMS(hours, minutes, seconds);
  if (verbose)
    fprintf (stderr, "Barycentre::build epoch = %3.10g"
	     " ra_hours = %d, ra_min = %d, ra_sec = %3.10g\n",
	     in_days, hours, minutes, seconds);

  double ra_secs = hours * 3600 + minutes * 60 + seconds;

  int degrees;
  dec_angle.getDMS(degrees, minutes, seconds);

  double dec_degs = dec_angle.getDegrees();

  if (verbose)
    fprintf (stderr, "Barycentre::build ra_secs = %3.15g, dec_degs = %d," 
	     " dec_min = %d, dec_sec = %3.10g, dec.getDegrees() = %3.15g\n",
	     ra_secs, degrees, minutes, seconds, dec_degs);

  // Convert it to degrees
  double ra_degs = ra_secs / (double)240; // == (ra_secs/(24*3600)) * 360

  // Convert to radians
  double R2000 = ra_degs/(double)RTOD;
  double D2000 = dec_degs/(double)RTOD;

  if (verbose)
    fprintf(stderr, "Barycentre::build RTOD = %3.15g,"
	    " R2000 = %3.15g, D2000 = %3.15g\n",
	    RTOD, R2000, D2000 );

  double dps[3];
  double dvb[3];
  double dpb[3];
  double dvh[3];
  double dph[3];

  // Convert to DCS
  slaDcs2c (R2000, D2000, dps);

  // Convert the time to TDT.
  double tdt = in_seconds + slaDtt (in_days);
  
  // Convert UTC to TDB.
  double g = (357.53 + 0.9856003 * (in_days - 51544.5)) / RTOD;
  double tdb = tdt + 0.001658 * sin (g) + 0.000014 * sin(2 * g);

  slaEvp (tdb/(double)86400, (double)2000, dvb, dpb, dvh, dph);

  double sitera = 0;

  // The velocity of the observatory
  double site_vel[3];

  // Cylindrical coordinates of observatory
  double site[3];

  // ///////////////////////////////
  // Based on tempo's setup.f
  // //////////

  double x = observatory[0];
  double y = observatory[1];
  double z = observatory[2];

  double erad = sqrt( sqr(x) + sqr(y) + sqr(z) );
    
  double siteLatitude = asin(z/erad);
  double siteLongitude = atan2(-y, x);
  double sphericalRadius = erad/(2.99792458e8*AUS);
  
  if (verbose)
    fprintf(stderr, "Barycentre::build tmjdctr = %3.10g, radius = %3.10g, siteLatitude = %3.10g, siteLongitude = %3.10g, sphericalRadius = %3.10g\n" ,
	    in_days, erad, siteLatitude, siteLongitude, sphericalRadius);

  // ///////////////////////////////
  // Based on tempo's arrtim.f
  // //////////

  site[0] = sphericalRadius * cos(siteLatitude) * AUS;
  site[1] = site[0] * tan(siteLatitude);
  site[2] = siteLongitude;

  if (verbose)
    fprintf(stderr, "Barycentre::build site[0] = %3.10g, site[1] = %3.10g, site[2] = %3.10g\n" ,
	    site[0], site[1], site[2]);
  
  // ///////////////////////////////
  // Based on tempo's obsite.f
  // //////////

  double LST_der;
  double LST;

  // Set longitude to 0 because we want the
  lmst2 (in_days, 0, &LST, &LST_der);
  
  LST *= TWOPI;

  double ph = LST - site[2];

  double EEQ[3];

  EEQ[0] = site[0] * cos(ph);
  EEQ[1] = site[0] * sin(ph);
  EEQ[2] = site[1];

  sitera = atan2(EEQ[1], EEQ[0]);

  if (verbose) {
    printf("pdmp: getDopplerFactor: LST = %3.10g, ph = %3.10g, EEQ[0] = %3.10g, EEQ[1] = %3.10g, EEQ[2] = %3.10g, sitera = %3.10g\n" ,
	   LST, ph, EEQ[0], EEQ[1], EEQ[2], sitera);
  }
  
  double speed = TWOPI*site[0]/((double)86400/ 1.00273);
  
  site_vel[0] = -speed * sin(sitera);
  site_vel[1] =  speed * cos(sitera);
  site_vel[2] = 0;
  
  if (verbose) {
    printf("pdmp: getDopplerFactor: site_vel[0] = %3.10g, site_vel[1] = %3.10g, site_vel[2] = %3.10g, speed = %3.10g\n" ,
	   site_vel[0], site_vel[1], site_vel[2], speed);
  }
  
  // ///////////////////////////////
  // /// Based on psrephd.f
  ////////////
  
  // Add the EC-observatory vectors to the barycentre-EC vectors.
  for (int i = 0; i < 3; i++)
  {
    // Convert from AUS per second to metres per second
    dvb[i] *= AUS;
    dvb[i] = dvb[i] + site_vel[i];
  }

  double evel = slaDvdv(dvb, dps);
  const double edelay = AUS * slaDvdv(dpb, dps);
  const double btdb = tdb + edelay;

  barycentric_epoch = btdb / 86400;
  Doppler = 1 - evel;
  
  if (verbose)
    printf("pdmp: getDopplerFactor: dopps = %3.20g\n", Doppler);
}
