/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluxCalibratorData.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarConstant.h"

#include "templates.h"

#include <iostream>
using namespace std;

Pulsar::FluxCalibrator::Data::Data ()
{
  calculated = false;
  valid = true;
}

//! Construct from known flux densities
Pulsar::FluxCalibrator::Data::Data ( const std::vector< Estimate<double> >& s,
				     const std::vector< Estimate<double> >& c )
{
  S_sys = s;
  S_cal = c;
  calculated = true;
}

void 
Pulsar::FluxCalibrator::Data::get ( std::vector< Estimate<double> >& s,
				    std::vector< Estimate<double> >& c ) const
try {
  calculate();
  s = S_sys;
  c = S_cal;
}
catch (Error& error) {
  s = S_sys;
  c = S_cal;
  throw error += "Pulsar::FluxCalibrator::Data::get";
}

//! Set the number of receptors
void Pulsar::FluxCalibrator::Data::set_nreceptor (unsigned nreceptor)
{
  S_sys.resize( nreceptor );
  S_cal.resize( nreceptor );
}

//! Get the number of receptors
unsigned Pulsar::FluxCalibrator::Data::get_nreceptor () const
{
  return S_sys.size();
}

//! Return the sum of all elements in a container
template< typename T, template<typename> class C >
T sum (const C<T>& container)
{
  typename C<T>::const_iterator i = container.begin();
  T ret = *i;
  for (i++; i != container.end(); i++)
    ret += *i;
  return ret;
}

//! Return the total system equivalent flux density
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_sys () const
{
  if (!valid)
    return 0;
  calculate ();
  return sum (S_sys);
}

//! Return the total calibrator flux density
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_cal () const
{
  if (!valid)
    return 0;
  calculate ();
  return sum (S_cal);
}

//! Return the system equivalent flux density in the specified receptor
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_sys (unsigned ir) const
{
  if (!valid)
    return 0;
  calculate ();
  return S_sys[ir];
}

//! Return the calibrator flux density in the specified receptor
Estimate<double> Pulsar::FluxCalibrator::Data::get_S_cal (unsigned ir) const
{
  if (!valid)
    return 0;
  calculate ();
  return S_cal[ir];
}

//! Add to the mean hi/lo ratio on source for the specified receptor
void Pulsar::FluxCalibrator::Data::add_ratio_on (unsigned ir,
						 Estimate<double>& ratio)
{
  mean_ratio_on.resize( get_nreceptor() );
  mean_ratio_on[ir] += ratio;
  calculated = false;
}

//! Add to the mean hi/lo ratio on source for the specified receptor
void Pulsar::FluxCalibrator::Data::add_ratio_off (unsigned ir,
						  Estimate<double>& ratio)
{
  mean_ratio_off.resize( get_nreceptor() );
  mean_ratio_off[ir] += ratio;
  calculated = false;
}

//! Set the flux of the standard candle
void Pulsar::FluxCalibrator::Data::set_S_std (double S)
{
  S_std = S;
  calculated = false;
}

//! Get the flux of the standard candle
double Pulsar::FluxCalibrator::Data::get_S_std () const
{
  return S_std;
}

void Pulsar::FluxCalibrator::Data::compute ()
{
  if (calculated)
    return;

  /* 
     Use the ScalarMath class to calculate the variances.
     These are static because they cost a bit during construction.
  */
  static MEAL::ScalarParameter ratio_on;
  static MEAL::ScalarParameter ratio_off;
  static MEAL::ScalarConstant unity (1.0);

  static MEAL::ScalarMath flux_cal = unity/(unity/ratio_on - unity/ratio_off);
  static MEAL::ScalarMath flux_sys = flux_cal / ratio_off;

  unsigned ir, nreceptor = get_nreceptor();

  for (ir=0; ir<nreceptor; ir++)
    S_cal[ir] = S_sys[ir] = 0;

  if (mean_ratio_on.size() != nreceptor)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::Data::calculate",
		 "no on-source observations available");

  if (mean_ratio_off.size() != nreceptor)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::Data::calculate",
		 "no off-source observations available");

  // the flux density of the standard candle in each receptor
  double S_std_i = S_std / nreceptor;

  valid = true;

  for (ir=0; ir<nreceptor; ir++) {

    Estimate<double> on  = mean_ratio_on[ir].get_Estimate();
    Estimate<double> off = mean_ratio_off[ir].get_Estimate();

    if (on==0 || off==0) {
      S_cal[ir] = S_sys[ir] = 0;
      valid = false;
      continue;
    }

    ratio_on.set_value (on);
    ratio_off.set_value (off);

    S_sys[ir] = S_std_i * flux_sys.get_Estimate();
    S_cal[ir] = S_std_i * flux_cal.get_Estimate();

    if (S_cal[ir].val < S_cal[ir].get_error() ||
	S_sys[ir].val < S_sys[ir].get_error() ) {

      S_cal[ir] = S_sys[ir] = 0;
      valid = false;

    }
    
  }  // end for each receptor

  calculated = true;
}

void Pulsar::FluxCalibrator::Data::calculate () const 
{
  if (!calculated)
    const_cast<Data*>(this)->compute();
}

