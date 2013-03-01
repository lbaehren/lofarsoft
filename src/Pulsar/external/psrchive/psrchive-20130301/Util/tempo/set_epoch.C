/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "psrephem.h"
#include "ephio.h"
#include "Cartesian.h"
#include "Error.h"

using Legacy::psrephem;

void pospm_conv (double alpha, double delta, double pmra, double pmdec,
		 Cartesian& r, Cartesian& u)
{
  //  INPUT:	alpha: RA  in turns
  //            delta: DEC in turns
  //            pmra: proper motion in RA  in turns per year
  //            pmdec: proper motion in DEC in turns per year
  //  OUTPUT:   r(3): unit vector in the direction of the pulsar
  //            u(3): vector in the direction of the transverse velocity of 
  //                  the pulsar in turns per year
  //
  // N.W. 8/1997
  // Converted to C++ by Willem van Straten  1/2001

  double ca = cos(2.0*M_PI*alpha);
  double sa = sin(2.0*M_PI*alpha);
  double cd = cos(2.0*M_PI*delta);
  double sd = sin(2.0*M_PI*delta);

  if (cd < 0.0)
    throw Error (InvalidParam, "pospm_conv", "|delta| > pi/2");

  r.x = ca*cd;
  r.y = sa*cd;
  r.z = sd;
    
  u.x = -pmra*sa*cd - pmdec*ca*sd;
  u.y =  pmra*ca*cd - pmdec*sa*sd;
  u.z =  pmdec*cd;
}

void psrephem::set_epoch (const MJD& new_epoch, bool binary)
{
  MJD current_epoch;

  if (parmStatus[EPH_PEPOCH] > 0)
    current_epoch = MJD (value_integer[EPH_PEPOCH],
			 value_double [EPH_PEPOCH]);
  else
    throw Error (InvalidParam, "psrephem::set_epoch",
		 "current PEPOCH unknown");

  double seconds = (new_epoch - current_epoch).in_seconds();

  double rf = value_double [EPH_F];
  double rf_dot = 0;
  double rf_dot_dot = 0;

  if (parmStatus[EPH_F1] > 0)
    rf_dot = value_double [EPH_F1];

  if (parmStatus[EPH_F2] > 0)
    rf_dot_dot = value_double [EPH_F2];

  if (seconds != 0.0) {
    value_double[EPH_F] = rf + rf_dot*seconds + rf_dot_dot * seconds*seconds;
    
    value_integer[EPH_PEPOCH] = new_epoch.intday();
    value_double [EPH_PEPOCH] = new_epoch.fracday();
  }

  if (parmStatus[EPH_POSEPOCH] > 0) {
    current_epoch = MJD (value_integer[EPH_POSEPOCH],
			 value_double [EPH_POSEPOCH]);
    value_integer[EPH_POSEPOCH] = new_epoch.intday();
    value_double [EPH_POSEPOCH] = new_epoch.fracday();
  }

  double years = (new_epoch - current_epoch).in_days() / 365.25;

  double mas = 1.0/(60.0*60.0*360.0*1000.0);  // turns

  // EPH_PMDEC  - mu_delta  - proper motion in declination (mas/yr)
  double mu_alpha = 0.0;
  if (parmStatus[EPH_PMRA] > 0)
    mu_alpha = value_double [EPH_PMRA] * mas;
  
  // EPH_PMRA   - mu_alpha  - proper motion in right ascension (mas/yr)
  double mu_delta = 0.0;
  if (parmStatus[EPH_PMDEC] > 0)
    mu_delta = value_double [EPH_PMDEC] * mas;

  double alpha = value_double[EPH_RAJ];
  double delta = value_double[EPH_DECJ];

  Cartesian r, u;
  pospm_conv (alpha, delta, mu_alpha/cos(delta), mu_delta, r, u);

  Cartesian pos = r + years * u;

  double sin_delta = pos.z;
  double cos_delta = cos(asin(sin_delta));
  double sin_alpha = pos.y / cos_delta;
  double cos_alpha = pos.x / cos_delta;

  if (years != 0.0) {
    value_double[EPH_RAJ]  += mu_alpha * years;
    value_double[EPH_DECJ] += mu_delta * years;
    
    value_double[EPH_RAJ]  = atan2 (sin_alpha, cos_alpha) / (2.0*M_PI);

    if (value_double[EPH_RAJ] < 0)
      value_double[EPH_RAJ] += 1.0;

    value_double[EPH_DECJ] = atan2 (sin_delta, cos_delta) / (2.0*M_PI);
  }

  if (!binary)
    return;

  if (parmStatus[EPH_T0] == 0)
    return;

  current_epoch = MJD (value_integer[EPH_T0], value_double [EPH_T0]);
  double days = (new_epoch - current_epoch).in_days();

  double pb = value_double[EPH_PB];
  double bin_phase = days/pb;

  double pb_dot = 0.0;
  if (parmStatus[EPH_PBDOT] > 0)
    pb_dot = value_double [EPH_PBDOT] * 1e-12;

  bin_phase -= bin_phase*bin_phase * pb_dot;
  int iphase = int(bin_phase);
  bin_phase -= double(iphase);

  // set the requested epoch to the next binary phase zero

  pb += pb_dot * days;
  MJD day_from_zero ((1.0 - bin_phase) * pb);

  MJD close_epoch = new_epoch + day_from_zero;

  days = (close_epoch - current_epoch).in_days();
  seconds = (close_epoch - current_epoch).in_seconds();
  years = days / 365.25;

  value_double [EPH_PB] += pb_dot * days;

  double x_dot = 0.0;
  if (parmStatus[EPH_XDOT] > 0)
    x_dot = value_double [EPH_XDOT] * 1e-12;

  value_double [EPH_A1] += x_dot * seconds;

  double om_dot = 0.0;
  if (parmStatus[EPH_OMDOT] > 0)
    om_dot = value_double [EPH_OMDOT];

  value_double [EPH_OM] += om_dot * years;

  double e_dot = 0.0;
  if (parmStatus[EPH_EDOT] > 0)
    e_dot = value_double [EPH_EDOT] * 1e-12;

  value_double [EPH_E] += e_dot * years;

  value_integer[EPH_T0] = close_epoch.intday();
  value_double [EPH_T0] = close_epoch.fracday();

}
