/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ObservationUncertainty.h"
#include "Pauli.h"

using namespace std;

template<typename T> T sqr (T x) { return x*x; }

//! Given a coherency matrix, return the difference
double Calibration::ObservationUncertainty::get_weighted_norm
(const Jones<double>& matrix) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );
  double difference = 0.0;
  for (unsigned ipol=0; ipol<4; ipol++)
  {
    difference += sqr(stokes[ipol].real()) * inv_variance[ipol].real();
    difference += sqr(stokes[ipol].imag()) * inv_variance[ipol].imag();
  }
  return difference;
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::ObservationUncertainty::get_weighted_conjugate
( const Jones<double>& matrix ) const try
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );

  // cerr << "inv=" << inv_variance << endl;

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] =
      complex<double>( inv_variance[ipol].real() * stokes[ipol].real(),
		      -inv_variance[ipol].imag() * stokes[ipol].imag() );

  return convert (stokes);
}
catch (Error& error)
{
  throw error += "Calibration::ObservationUncertainty::get_weighted_conjugate";
}

Stokes< complex<double> >
Calibration::ObservationUncertainty::get_weighted_components
( const Jones<double>& matrix ) const
{
  Stokes< complex<double> > stokes = complex_coherency( matrix );

  for (unsigned ipol=0; ipol<4; ipol++)
    stokes[ipol] =
      complex<double>( sqrt(inv_variance[ipol].real()) * stokes[ipol].real(),
		       sqrt(inv_variance[ipol].imag()) * stokes[ipol].imag() );

  return stokes;
}

//! Set the uncertainty of the observation
void Calibration::ObservationUncertainty::set_variance
( const Stokes< complex<double> >& variance )
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_variance[ipol] = complex<double>( 1.0 / variance[ipol].real(),
					  1.0 / variance[ipol].imag() );
}

//! Set the uncertainty of the observation
void Calibration::ObservationUncertainty::set_variance
( const Stokes<double>& variance )
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_variance[ipol] = complex<double>( 1.0 / variance[ipol],
					  1.0 / variance[ipol] );
}

//! Return the variance of each Stokes parameter
Stokes< std::complex<double> > 
Calibration::ObservationUncertainty::get_variance () const
{
  Stokes< std::complex<double> > result;

  for (unsigned ipol=0; ipol < 4; ipol++)
  {
    double re = 0.0;
    if (inv_variance[ipol].real())
      re = 1.0/inv_variance[ipol].real();

    double im = 0.0;
    if (inv_variance[ipol].imag())
      im = 1.0/inv_variance[ipol].imag();

    result[ipol] = complex<double>( re, im );
  }

  return result;
}

//! Construct with the uncertainty of the observation
Calibration::ObservationUncertainty::ObservationUncertainty
( const Stokes<double>& variance )
{
  set_variance (variance);
}

