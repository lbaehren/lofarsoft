/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// /////////////////////////////////////////////////////////////////////
//
// Given a pulsar ephemeris, output the annual-orbital parallax term
//
// /////////////////////////////////////////////////////////////////////

#include "psrephem.h"
#include "ephio.h"
#include "slalib.h"
#include "MJD.h"

using Legacy::psrephem;

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <math.h>
#include <unistd.h>

using namespace std;

void usage()
{
  cout << "\naopx: simulate annual-orbital parallax timing signature\n\n"
    "  Usage: psraux [options] JNAME\n\n"
    "  Where options are:\n"
    "  -s        special plot\n"
    "  -d days   total time to plot in days\n"
    "  -D dist   distance to pulsar in parsec\n"
    "  -e elat   ecliptic latitude in degrees\n"
    "  -i incl   inclination angle in degrees\n"
    "  -O Omega  angle to ascending node in degrees\n"
    "\n"
    "  -v        verbose\n"
       << endl;
}

extern "C" double sla_gmst_(double * ut);

const double au     = 1.5e8;    // km
const double c      = 3.0e5;    // km/s
const double parsec = 3.086e13; // km
const double year   = 365.25 * 24 * 3600; // seconds

int main (int argc, char ** argv)
{
  bool verbose = false;
  bool special = false;

  double inclination = 43;
  double ascending_node = 0;
  double days = 365.25;

  double elat = -999;
  double user_dist = -999;

  bool report = false;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "d:D:e:hi:rsO:v")) != -1) {
    switch (gotc) {

    case 's':
      special = true;
      break;

    case 'd':
      sscanf (optarg, "%lf", &days);
      break;
  
    case 'D':
      sscanf (optarg, "%lf", &user_dist);
      break;

    case 'e':
      sscanf (optarg, "%lf", &elat);
      elat *= M_PI/180;
      break;

    case 'h':
      usage ();
      return 0;
    
    case 'i':
      sscanf (optarg, "%lf", &inclination);
      break;

    case 'O':
      sscanf (optarg, "%lf", &ascending_node);
      break;

    case 'r':
      report = true;
      break;

    case 'v':
      verbose = true;
      break;

    }
  }

  if (optind >= argc)
  {
    cerr << "Please provide an ephemeris as the last argument" << endl;
    return -1;
  }

  psrephem eph (argv[optind]);

  MJD now (time(NULL));
  double mjd = now.in_days();

  double right_ascension = eph.jra() * 2*M_PI;
  double declination = eph.jdec() * 2*M_PI;

  double ecliptic_long = 0, ecliptic_lat = 0;
  slaEqecl (right_ascension, declination, mjd,
	    &ecliptic_long, &ecliptic_lat);

  if (elat != -999)
    ecliptic_lat = elat;

  slaEcleq (ecliptic_long, ecliptic_lat, mjd,
	    &right_ascension, &declination);

  double px_mas = eph.value_double[EPH_PX];
  cerr << "PX=" << px_mas << " mas" << endl;

  double dist_pc = 1/px_mas * 1e3;
  if (user_dist != -999)
    dist_pc = user_dist;

  double p_au = au * cos (ecliptic_lat);
  double px = p_au * p_au / (2 * c * dist_pc * parsec) * 1e6;

  double sini_val = 0, sini_err = 0;
  if (eph.sini(sini_val,sini_err) == 0)
    inclination = asin ( sini_val ) * 180/M_PI;

  double asini = eph.a1();
  double binary_period = eph.pb();
  double cos_i     = cos ( inclination * M_PI/180 ); 
  double sin_i     = sin ( inclination * M_PI/180 ); 
  double cos_Omega = cos ( ascending_node * M_PI/180 );
  double sin_Omega = sin ( ascending_node * M_PI/180 );
  double cos_alpha = cos ( right_ascension );
  double sin_alpha = sin ( right_ascension );
  double cos_delta = cos ( declination );
  double sin_delta = sin ( declination );

  double aopx = ( asini * au ) / ( sin_i * dist_pc * parsec ) * 1e6;

  if (report)  {
    cout << argv[optind] << " " << 2 * aopx << " us (pk-pk)" << endl;
    return 0;
  }

  cout << "\n";
  cout << "Right ascension:       "
       << right_ascension * 12/M_PI << " hr" << endl;
  cout << "Declination:           "
       << declination * 180/M_PI << " deg \n" << endl;

  cout << "Eclicptic longitude:   " << setprecision(15) << ecliptic_long * 180/M_PI << endl;
  cout << "Eclicptic latitude:    " << setprecision(15) << ecliptic_lat * 180/M_PI << endl;
  cout << "\n";
  cout << "Distance (pc):         " << dist_pc << endl;
  cout << "Parallax Term (us):    " << px << endl;
  cout << "\n";
  cout << "x = a sin(i) (sec):    " << asini << endl;
  cout << "Binary Period (d):     " << binary_period << endl;
  cout << "Inclination angle:     " << inclination << endl;
  cout << "Annual-Orbital Px:     " << 2 * aopx << " us (pk-pk)" << endl;

  double imax = 0, imax_err = 0;
  eph.imax_pm_xdot (imax, imax_err);
  cout << "\n"
       << "i_max (xdot):  " << imax*180/M_PI << endl;

  eph.imax_pm_omdot (imax, imax_err);
  cout << "\n"
       << "i_max (omdot): " << imax*180/M_PI << endl;

  cout << "\n"
       << "Ascending node:      " << ascending_node << endl;

  double resolution = 2 * 3600;  // every two hours
  double time = 0;

  double omega_earth = 2 * M_PI / year;
  double omega_pulsar = 2 * M_PI / (binary_period * 24.0 * 3600.0);

  vector<double> t_roemer;   // Roemer delay: x - in seconds
  vector<double> t_aopx;     // Annual-orbital parallax: o - in microseconds
  vector<double> combined;   // x + o: seconds + microseconds (to exaggerate)

  vector<double> times;
  vector<double> total;
  
  double end = days * 24 * 3600;

  double cos_epsilon = 0.91748213149438;
  double sin_epsilon = 0.39777699580108;

  double mean=0;
  double meansq=0;

  while (time < end) {

    double ex = cos (omega_earth * time);
    double ey = sin (omega_earth * time);

    double X = ex;
    double Y = cos_epsilon * ey;
    double Z = sin_epsilon * ey;

    double i_not = -X*sin_alpha + Y*cos_alpha;
    double j_not = -X*sin_delta*cos_alpha -Y*sin_delta*sin_alpha +Z*cos_delta;

    double Q = cos (omega_pulsar * time);
    double R = sin (omega_pulsar * time);

    double A = i_not * sin_Omega - j_not * cos_Omega;
    double B = i_not * cos_Omega + j_not * sin_Omega;

    double aopx_val = aopx * ( A * R * cos_i - B * Q );

    //cerr << "ARcos(i)=" <<A*R*cos_i<< " BQ=" <<B*Q
         //<< " Qsin(i)=" << asini * Q << " aopx=" << aopx_val <<endl;

    if (special) {
#if Works
      double sin_st = sin((omega_earth + omega_pulsar) * time);
      double cos_st = cos((omega_earth + omega_pulsar) * time);
      double st = atan2(sin_st, cos_st);
      t_aopx.push_back (aopx_val);
      times.push_back (st);
      //times .push_back (Q);
      //t_aopx.push_back (aopx_val);
      //times.push_back (sin((omega_earth + omega_pulsar) * time));
#endif
      double bfac = 1.0;
      // bfac = 1.0 - 2.0 * omega_earth / omega_pulsar;
      double sin_st = sin((omega_earth*bfac + omega_pulsar) * time);
      double cos_st = cos((omega_earth*bfac + omega_pulsar) * time);
      double st = atan2(sin_st, cos_st);


      double sin_ph = sin(2.0 * omega_earth * time);
      double cos_ph = cos(2.0 * omega_earth * time);
      double ph = atan2(cos_ph, sin_ph);
      ph = atan2(sin_ph, cos_ph);

      t_aopx.push_back (aopx_val);
      times.push_back (st);
      //times .push_back (Q);
      //t_aopx.push_back (aopx_val);
      //times.push_back (sin((omega_earth + omega_pulsar) * time));

    }
    else {
      t_aopx.push_back (aopx_val);
      times .push_back (time / (24 * 3600));
      // t_px  .push_back (px * i_not * i_not);
      t_roemer.push_back (asini * Q);
      combined.push_back (t_aopx.back() + t_roemer.back());
    }

    mean += aopx_val;
    meansq += aopx_val * aopx_val;

    // double A = Q * cos_Omega - R * sin_Omega * cos_i;
    // double B = Q * sin_Omega + R * cos_Omega * cos_i;
    // t_aopx.push_back (aopx * ( A * i_not + B * j_not ));


    // total .push_back (t_px.back() + t_aopx.back());
    time += resolution;
  }

  meansq /= t_aopx.size();
  mean /= t_aopx.size();

  double rms = sqrt(meansq - mean*mean);
  cerr << "RMS of aopx " << rms << endl;

  if (verbose)
    cerr << "aopx: Setting up plots..." << endl;

  if (verbose)
    cerr << "aopx: Plotting..." << endl;

  return 0;
}
