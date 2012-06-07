/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ephio.h"
#include "psrephem.h"
#include "orbital.h"
#include "Estimate.h"

#include <slalib.h>
#include <math.h>

using namespace std;
using Legacy::psrephem;

#define sqr(x) (x*x)
#define cube(x) (x*x*x)

const double au   = 1.49597870e8;   // km
const double c    = 2.99792458e5;   // km/s
const double day  = 24.0 * 3600.0;  // seconds
const double year = 365.2422 * day; // seconds
const double mas  = M_PI/(60.0*60.0*180.0*1000.0);  // radians
const double parsec = 3.085678e13;  // km

// I.H. Stairs et al. 1998, ApJ 505:352-357 use T_sol = G * M_sol / c^3
// where G = Gravitational Constant
//       M = Mass of Sun
//       c = speed of light
const double T_sol  = 4.925490947e-6; // seconds

// K. Kuijken and G. Gilmore, 1989, MNRAS, 239, 571, Table 2
// velocity of sun with respect to galactic centre
const double v_0 = 222;   // km/s
// distance of sun from galactic centre
const double R_0 = 7.7e3; // parsec


// ////////////////////////////////////////////////////////////////////////
//
// returns galactic latitude and longitude in radians (slalib)
//
int psrephem::galactic (double& l, double& b)
{
  if (parmStatus[EPH_RAJ] < 1 || parmStatus[EPH_DECJ] < 1)
    return -1;

  double alpha = value_double [EPH_RAJ] * 2*M_PI;
  // double alpha_err  = error_double [EPH_RAJ];
  double delta = value_double [EPH_DECJ] * 2*M_PI;
  // double delta_err  = error_double [EPH_DECJ];

  slaEqgal (alpha, delta, &l, &b);

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns galactic z-height in parsec
//
int psrephem::galactic_z (double& z)
{
  double l, b;
  if (galactic (l, b) < 0)
    return -1;

  if (parmStatus[EPH_PX] < 1)
    return -1;
  double px = value_double [EPH_PX];
  // double px_err = error_double [EPH_PX];

  // distance in parsec
  double d = 1e3/px;
  z = d * sin(b);
  return 0;
}


// ////////////////////////////////////////////////////////////////////////
//
// returns the acceleration along the line of sight arising
// from differential galactic rotation in km/s
//
int psrephem::acc_diffrot (double& acc)
{
  double l, b;
  if (galactic (l, b) < 0)
    return -1;

  if (parmStatus[EPH_PX] < 1)
    return -1;

  double px = value_double [EPH_PX];
  // double px_err = error_double [EPH_PX];

  // distance in parsec
  double d = 1e3/px;

  // see Damour, T. and Taylor, J.H. 1991, ApJ, 366, 501
  // first term of Equation 2.12 (R_0 and v_0 are consts, defined up top)
  double sinl = sin(l);
  double cosl = cos(l);
  double beta = d/R_0 - cosl;
  acc = -v_0*v_0/(R_0*parsec) * (cosl + beta / (sinl*sinl + beta*beta));
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::mass_function (double& mf, double& mf_err) const
{
  mf_err = mf = -1.0;

  if (parmStatus[EPH_A1] < 1 || parmStatus[EPH_PB] < 1)
    return -1;

  Estimate<double> x (value_double [EPH_A1], sqr(error_double [EPH_A1]));
  Estimate<double> pb (value_double [EPH_PB], sqr(error_double [EPH_PB]));

  Estimate<double> n = 2.0 * M_PI / (pb * 86400.0);

  Estimate<double> mass_function = sqr(n) * cube(x) / T_sol;

  mf = mass_function.val;
  mf_err = sqrt(mass_function.var);

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::sini (double& si, double& si_err) const
{
  if (parmStatus[EPH_KIN] > 0) {
    double i = value_double [EPH_KIN] * M_PI/180.0;
    si     = sin(i);
    si_err = cos(i) * error_double [EPH_KIN] * M_PI/180.0;
  }
  else if (parmStatus[EPH_SINI] > 0) {
    si     = value_double [EPH_SINI];
    si_err = error_double [EPH_SINI];
  }
  else
    return -1;

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::m1 (double& m1, double& m1_err) const
{
  if (parmStatus[EPH_M2] < 1 )
    return -1;

  double m2     = value_double [EPH_M2];
  double m2_err = error_double [EPH_M2];

  double si, si_err;
  if (sini (si, si_err) != 0)
    return -1;

  double mass_func, mass_func_err;
  if (mass_function (mass_func, mass_func_err) != 0)
    return -1;

  double mtot = sqrt (cube(m2*si)/mass_func);
  double mtot_err = mtot * sqrt( sqr(0.5 * mass_func_err/mass_func) +
				   sqr(1.5 * si_err/si) +
				   sqr(1.5 * m2_err/m2) );

  m1 = mtot - m2;
  m1_err = sqrt( sqr(mtot_err) + sqr(m2_err) );
  return 0;
}

int psrephem::m2 (double& m2, double m1) const
{
  double si, si_err;
  if (sini (si, si_err) != 0)
    return -1;

  double mf, mf_err;
  if (mass_function (mf, mf_err) != 0)
    return -1;

  m2 = companion_mass (mf, si, m1);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the composite proper motion
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::pm (double& pm, double& pm_err) const
{
  if (parmStatus[EPH_PMRA] < 1 || parmStatus[EPH_PMDEC] < 1)
    return -1;

  double covar = 0.0;   // covariance b/w mu_alpha and mu_delta

  double mu_alpha = value_double [EPH_PMRA];
  double mu_aerr  = error_double [EPH_PMRA];

  double mu_delta = value_double [EPH_PMDEC];
  double mu_derr  = error_double [EPH_PMDEC];

  pm = sqrt (sqr(mu_alpha) + sqr(mu_delta));

  pm_err = sqrt ( sqr(mu_alpha*mu_aerr) + sqr(mu_delta*mu_derr)
		  + 2.0 * covar * mu_alpha * mu_delta ) / pm;
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the proper motion celestial position angle
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::phi (double& phi, double& phi_err) const
{
  if (parmStatus[EPH_PMRA] < 1 || parmStatus[EPH_PMDEC] < 1)
    return -1;

  double covar = 0.0;   // covariance b/w mu_alpha and mu_delta

  double mu_alpha = value_double [EPH_PMRA];
  double mu_aerr  = error_double [EPH_PMRA];

  double mu_delta = value_double [EPH_PMDEC];
  double mu_derr  = error_double [EPH_PMDEC];

  double sqr_pm = sqr(mu_alpha) + sqr(mu_delta);

  phi = atan2 (mu_alpha, mu_delta);

  phi_err = sqrt ( sqr(mu_alpha*mu_derr) + sqr(mu_delta*mu_aerr)
		   + 2.0 * covar * mu_alpha * mu_delta ) / sqr_pm;
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the proper motion contribution to x-dot
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::pm_x_dot (double& xdot, double& xdot_err) const
{
  if (parmStatus[EPH_PMRA] < 1 || parmStatus[EPH_PMDEC] < 1)
    return -1;

  double mu_alpha = value_double [EPH_PMRA];
  // double mu_aerr  = error_double [EPH_PMRA];

  double mu_delta = value_double [EPH_PMDEC];
  // double mu_derr  = error_double [EPH_PMDEC];

  if (parmStatus[EPH_KIN] < 1 || parmStatus[EPH_KOM] < 1)
    return -1;

  double i     = value_double [EPH_KIN] * M_PI/180.0;
  // double i_err = error_double [EPH_KIN] * M_PI/180.0;

  double Om     = value_double [EPH_KOM] * M_PI/180.0;
  // double Om_err = error_double [EPH_KOM] * M_PI/180.0;

  if (parmStatus[EPH_A1] < 1)
    return -1;

  double x     = value_double [EPH_A1];
  // double x_err = error_double [EPH_A1];

  xdot = x * ( -mu_alpha * sin(Om) + mu_delta * cos(Om) ) / tan(i) * mas/year;
  xdot_err = -1;

  return 0;
}


// ////////////////////////////////////////////////////////////////////////
//
// returns the proper motion contribution to omdot in degrees per year
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::pm_omega_dot (double& omdot, double& omdot_err) const
{
  if (parmStatus[EPH_PMRA] < 1 || parmStatus[EPH_PMDEC] < 1)
    return -1;

  double mu_alpha = value_double [EPH_PMRA];
  // double mu_aerr  = error_double [EPH_PMRA];

  double mu_delta = value_double [EPH_PMDEC];
  // double mu_derr  = error_double [EPH_PMDEC];

  if (parmStatus[EPH_KIN] < 1 || parmStatus[EPH_KOM] < 1)
    return -1;

  double i = value_double [EPH_KIN] * M_PI/180.0;
  // double i_err = error_double [EPH_KIN] * M_PI/180.0;

  double Om = value_double [EPH_KOM] * M_PI/180.0;
  // double Om_err = error_double [EPH_KOM] * M_PI/180.0;

  omdot = ( mu_alpha * cos(Om) + mu_delta * sin(Om) ) / sin(i) * mas*180/M_PI;
  omdot_err = -1;

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the spin period in seconds
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::P (double& p, double& p_err) const
{
  if (parmStatus[EPH_F] < 1)
    return -1;

  double rf = value_double [EPH_F];
  double rf_err = error_double [EPH_F];
  p = 1.0 / rf;
  p_err = p * rf_err / rf;

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the spin period derivative
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::P_dot (double& p_dot, double& p_dot_err) const
{
  if (parmStatus[EPH_F] < 1 || parmStatus[EPH_F1] < 1)
    return -1;

  Estimate<double> rf (value_double [EPH_F], sqr(error_double [EPH_F]));
  Estimate<double> rf_dot (value_double [EPH_F1], sqr(error_double [EPH_F1]));
    
  Estimate<double> pdot = - rf_dot / (rf*rf);

  p_dot = pdot.val;
  p_dot_err = sqrt(pdot.var);

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the second spin period derivative
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::P_ddot (double& p_ddot, double& p_ddot_err) const
{
  p_ddot = p_ddot_err = 0;

  if (parmStatus[EPH_F] < 1 || parmStatus[EPH_F1] < 1 
      || parmStatus[EPH_F2] < 1)
    return -1;

  Estimate<double> rf (value_double [EPH_F], sqr(error_double [EPH_F]));

  Estimate<double> rf_dot (value_double [EPH_F1], sqr(error_double [EPH_F1]));

  Estimate<double> rf_ddot (value_double [EPH_F2], sqr(error_double [EPH_F2]));

  Estimate<double> t = (2.0 * sqr(rf_dot)/rf - rf_ddot) / (rf*rf);

  p_ddot = t.val;
  p_ddot_err = sqrt(t.var);

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::quadratic_Doppler (double& beta, double& beta_err) const
{
  if (parmStatus[EPH_PX] < 1)
    return -1;

  double mu, mu_err;
  if (pm (mu, mu_err) < 0)
    return -1;

  double mu_radsec = mu * mas/year;

  double px = value_double [EPH_PX];
  double px_err = error_double [EPH_PX];

  // distance to pulsar in km
  double dist = au / (px * mas);
  double dist_err = dist * px_err/px;

  beta = sqr(mu_radsec) * dist / c;
  beta_err = beta * sqrt (sqr(2.0*mu_err/mu) + sqr(dist_err/dist));

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the cubic Doppler shift given the radial velocity in km/s
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::cubic_Doppler (double& gamma, double& gamma_err,
			     double pmrv, double pmrv_err) const
{
  double mu, mu_err;
  if (pm (mu, mu_err) < 0)
    return -1;

  double mu_radsec = mu * mas/year;

  gamma = sqr(mu_radsec) * pmrv / c;
  gamma_err = gamma * sqrt (sqr(2.0*mu_err/mu) + sqr(pmrv_err/pmrv));
  return 0;
}

int psrephem::Doppler_P_dotdot (double& P_dotdot, double& P_dotdot_err,
				double pmrv, double pmrv_err) const
{
  double beta=0, beta_err=0;
  if (quadratic_Doppler (beta, beta_err) < 0)
    return -1;

  double gamma, gamma_err;
  if (cubic_Doppler (gamma, gamma_err, pmrv, pmrv_err) < 0)
    return -1;

  double p=0, p_err=0;
  if (P (p, p_err) < 0)
    return -1;

  double p_dot=0, p_dot_err=0;
  if (P_dot (p_dot, p_dot_err) < 0)
    return -1;

  P_dotdot = 2.0 * p_dot * beta - 3.0 * p * gamma;
  P_dotdot_err = 0; // for now
  return 0;
}

int psrephem::intrinsic_P_dotdot (double& P_dotdot, double& P_dotdot_err,
				  float braking_index) const
{
  double p=0, p_err=0;
  if (P (p, p_err) < 0)
    return -1;

  double p_dot=0, p_dot_err=0;
  if (P_dot (p_dot, p_dot_err) < 0)
    return -1;

  // solve n=2-P*P_dot_dot/sqr(P_dot)
  P_dotdot = (2.0 - braking_index) * sqr (p_dot) / p;
  P_dotdot_err = P_dotdot * sqrt (sqr(2.0*p_dot_err/p_dot) + sqr(p_err/p));

  return 0;
}

int psrephem::Doppler_F2 (double& f2, double& f2_err,
			  double pmrv, double pmrv_err) const
{
  double beta=0, beta_err=0;
  if (quadratic_Doppler (beta, beta_err) < 0)
    return -1;

  double gamma, gamma_err;
  if (cubic_Doppler (gamma, gamma_err, pmrv, pmrv_err) < 0)
    return -1;

  double p=0, p_err=0;
  if (P (p, p_err) < 0)
    return -1;

  double p_dot=0, p_dot_err=0;
  if (P_dot (p_dot, p_dot_err) < 0)
    return -1;

  double p_dotdot = 2.0 * p_dot * beta - 3.0 * p * gamma;

  f2 = (2.0*sqr(p_dot)/p - p_dotdot) / sqr(p);
  f2_err = 0; // for now
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_f_e (double& f_e) const
{
  f_e = 1.0;
  if (parmStatus[EPH_E] > 0) {
    double ecc2 = sqr(value_double [EPH_E]);
    f_e = (1.0 + 73.0/24.0*ecc2 + 37.0/96.0*sqr(ecc2)) / pow(1.0-ecc2,3.5);
  }
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the intrinsic x_dot due to gravitational wave emission
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_x_dot (double& x_dot) const
{
  if (parmStatus[EPH_A1] < 1)
    return -1;
  double x = value_double [EPH_A1];
  // double x_err = error_double [EPH_A1];

  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB] * day;
  // double pb_err = error_double [EPH_PB] * day;

  double pb_dot_gr;
  GR_Pb_dot (pb_dot_gr);

  // Kopeikin, 1996, ApJ 467:L93-L95 Eqn 15
  // and van Straten, PhD thesis
  x_dot = 2.0/3.0 * x/pb * pb_dot_gr;

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the intrinsic Pb_dot due to gravitational wave emission
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_Pb_dot (double& Pb_dot) const
{
  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB];
  // double pb_err = error_double [EPH_PB];

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2     = value_double [EPH_M2];
  // double m2_err = error_double [EPH_M2];

  double f_e;
  GR_f_e (f_e);

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;
  
  double n = 2.0 * M_PI / (pb * 86400.0);
  
  // I.H. Stairs et al. 1998, ApJ 505:352-357 Eqn 10
  Pb_dot = - 192.0 * M_PI / 5.0 * pow (n*T_sol, 5.0/3.0) * f_e * mp * m2 
    / pow(mp+m2, 1.0/3.0);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the intrinsic omega_dot due to gravitational wave emission
// in degrees per year
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_omega_dot (double& w_dot) const
{
  double ecc2 = sqr(value_double [EPH_E]);

  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB];
  // double pb_err = error_double [EPH_PB];

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2     = value_double [EPH_M2];
  // double m2_err = error_double [EPH_M2];

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;

  double n = 2.0 * M_PI / (pb * 86400.0);

  // I.H. Stairs et al. 1998, ApJ 505:352-357 Eqn 8
  w_dot = 3.0 * pow(n, 5.0/3.0) * pow((mp+m2)*T_sol, 2.0/3.0) / (1.0-ecc2);

  w_dot *= year * 180.0 / M_PI;
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::GR_omega_dot_mtot (double& mtot, double& mtot_err) const
{
  if (parmStatus[EPH_OMDOT] < 1 )
    return -1;

  double omega_dot     = value_double [EPH_OMDOT] * M_PI/180.0 / year;
  double omega_dot_err = error_double [EPH_OMDOT] * M_PI/180.0 / year;

  if (parmStatus[EPH_PB] < 1 )
    return -1;

  double Pb     = value_double [EPH_PB];
  double Pb_err = error_double [EPH_PB];

  double ecc     = value_double [EPH_E];
  double ecc_err = error_double [EPH_E];

  double invn = Pb * 86400.0 / (2.0 * M_PI);

  double term = omega_dot * (1.0-sqr(ecc)) / 3.0;

  mtot = pow(term, 1.5) * pow(invn, 2.5) / T_sol;

  mtot_err = mtot * sqrt ( sqr(1.5 * omega_dot_err/omega_dot) +
			   sqr(omega_dot*ecc/term * ecc_err) +
			   sqr(2.5 * Pb_err/Pb) );
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::GR_omega_dot_m2 (double& m2, double& m2_err) const
{
  double mt, mt_err;
  if (GR_omega_dot_mtot (mt, mt_err) != 0)
    return -1;

  cerr << "psrephem::GR_omega_dot_m2 mtot " << mt << " +/- " << mt_err << endl;

  double si, si_err;
  if (sini (si, si_err) != 0)
    return -1;

  double mf, mf_err;
  if (mass_function (mf, mf_err) != 0)
    return -1;

  m2 = pow (mf * sqr(mt), 1.0/3.0) / si;
  m2_err = m2 * sqrt ( sqr (2.0/3.0 * mt_err/mt) +
		       sqr (1.0/3.0 * mf_err/mf) +
		       sqr (si_err/si) );

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the general relativistic prediction of the rate of precession 
// of the pulsar's spin axis due to spin-orbit coupling
//
// See wrt89
// 
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_Omega_p (double& Omega_p) const
{
  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB] * day;

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2 = value_double [EPH_M2];

  if (parmStatus[EPH_E] < 1 )
    return -1;
  double ecc = value_double [EPH_E];

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;

#ifdef _DEBUG
  cerr << "psrephem::GR_Omega_p checking values for 1913+16" << endl;
  pb = 27907;
  ecc = 0.6171;
  m2 = 1.442;
  mp = 1.386;
#endif

  double w = 2.0 * M_PI / pb;

  // Equation 1 in Weisberg, Romani, and Taylor 1989. ApJ 347, 1030
  Omega_p = 0.5 * pow(T_sol, 2.0/3.0) * pow (w, 5.0/3.0) / (1-ecc*ecc)
    * m2 * (4.0*mp + 3.0*m2) * pow (mp+m2, -4.0/3.0);

  // Or, this will give the same answer:
  // Equation 4 in Barker and O'Connell 1975. ApJ 199, L25

  double mu = mp*m2/(mp+m2);
  Omega_p = 1.5 * pow(T_sol, 2.0/3.0) * pow (w, 5.0/3.0) / (1-ecc*ecc)
    * (m2 + mu/3.0) * pow (mp+m2, -1.0/3.0);

  // convert to degrees per year
  Omega_p *= 180.0/M_PI * year;

#ifdef _DEBUG
  cerr << "psrephem::GR_Omega_p (1913+16) = " << Omega_p << " deg/yr. "
    "(should be ~1.21 deg/yr)" << endl;
#endif

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the general relativistic prediction of gamma
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_gamma (double& gamma) const
{
  double ecc = value_double [EPH_E];

  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB];
  // double pb_err = error_double [EPH_PB];

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2     = value_double [EPH_M2];
  // double m2_err = error_double [EPH_M2];

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;

  double n = 2.0 * M_PI / (pb * 86400.0);

  // I.H. Stairs et al. 1998, ApJ 505:352-357 Eqn 3
  gamma = ecc * pow(sqr(T_sol)/(n*pow(mp+m2,4.0)), 1.0/3.0) * m2*(mp + 2.0*m2);

  return 0;
}

#if 0

int psrephem::Delta_t (double& delta_t) const
{
  delta_t = 0;

  if (parmStatus[EPH_F] < 1 || parmStatus[EPH_F2] < 1)
    return -1;

  double rf = value_double [EPH_F];

  double rf2 = value_double [EPH_F2];
  double rf2_err = error_double [EPH_F2];

  cerr << "f=" << rf << " f2=" << rf2 << " " << rf2_err << endl;

  double use = fabs(rf2) + rf2_err;

  delta_t = log10 (use*1e24/(6.0*rf));
 return 0;
}

#endif

