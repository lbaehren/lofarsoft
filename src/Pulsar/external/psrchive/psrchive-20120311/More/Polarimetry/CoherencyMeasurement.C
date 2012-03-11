/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CoherencyMeasurement.h"
#include "Pulsar/ObservationUncertainty.h"
#include "Pauli.h"

#include <assert.h>

using namespace std;

Calibration::CoherencyMeasurement::CoherencyMeasurement (unsigned index)
{
  nconstraint = 0;
  input_index = index;
}

//! Set the index of the input to which the measurement corresponds
void Calibration::CoherencyMeasurement::set_input_index (unsigned index)
{
  input_index = index;
}

//! Get the index of the input to which the measurement corresponds
unsigned Calibration::CoherencyMeasurement::get_input_index () const
{
  return input_index;
}

//! Get the number of constraints provided by this measurement
unsigned Calibration::CoherencyMeasurement::get_nconstraint () const
{
  return nconstraint;
}

//! Set the measured Stokes parameters
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< Estimate<double> >& stokes)
{
  Stokes<double> val;

  for (unsigned ipol=0; ipol<4; ipol++) {
    val[ipol] = stokes[ipol].val;
    variance[ipol] = stokes[ipol].var;
  }

  rho = convert (val);
  uncertainty = new ObservationUncertainty (variance);

  // 4 Stokes
  nconstraint = 4;
}

//! Get the measured Stokes parameters
Stokes< Estimate<double> > 
Calibration::CoherencyMeasurement::get_stokes () const
{
  Stokes< Estimate<double> > result;
  Stokes< double > val = coherency (rho);

  for (unsigned ipol=0; ipol<4; ipol++) {
    result[ipol].val = val[ipol];
    result[ipol].var = variance[ipol];
  }

  return result;
}

//! Get the measured complex Stokes parameters
Stokes< std::complex< Estimate<double> > > 
Calibration::CoherencyMeasurement::get_complex_stokes () const
{
  Stokes< std::complex< Estimate<double> > > result;
  Stokes< std::complex<double> > val = complex_coherency (rho);
  Stokes< std::complex<double> > var = variance;

  if (uncertainty)
    var = uncertainty->get_variance ();

  for (unsigned ipol=0; ipol<4; ipol++)
  {
    Estimate<double> re ( val[ipol].real(), var[ipol].real() );
    Estimate<double> im ( val[ipol].imag(), var[ipol].imag() );

    result[ipol] = std::complex< Estimate<double> > (re, im);
  }

  return result;
}

//! Set the measured complex Stokes parameters
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< complex<double> >& stokes, const Stokes<double>& var)
{
  rho = convert (stokes);
  variance = var;

  uncertainty = new ObservationUncertainty (variance);

  // 4 Stokes, Re and Im
  nconstraint = 8;
}

//! Set the measured complex Stokes parameters and the variance functions
void Calibration::CoherencyMeasurement::set_stokes
(const Stokes< complex<double> >& stokes, const Uncertainty* var)
{
  uncertainty = var;

  rho = convert (stokes);
  variance = 0;

  // 4 Stokes, Re and Im
  nconstraint = 8;
}

//! Get the measured coherency matrix
Jones<double> Calibration::CoherencyMeasurement::get_coherency () const
{
  return rho;
}

//! Given a coherency matrix, return the difference
double Calibration::CoherencyMeasurement::get_weighted_norm
(const Jones<double>& matrix) const
{
  if (!uncertainty)
    throw Error (InvalidState,
		 "Calibration::CoherencyMeasurement::get_weighted_norm",
		 "Uncertainty policy not set");

  return uncertainty->get_weighted_norm (matrix);
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::CoherencyMeasurement::get_weighted_conjugate
(const Jones<double>& matrix) const try
{
  if (!uncertainty)
    throw Error (InvalidState,
		 "Calibration::CoherencyMeasurement::get_weighted_conjugate",
		 "Uncertainty policy not set");

  return uncertainty->get_weighted_conjugate (matrix);
}
catch (Error& error)
{
  throw error += "Calibration::CoherencyMeasurement::get_weighted_conjugate";
}

void Calibration::CoherencyMeasurement::get_weighted_components
(const Jones<double>& matrix, std::vector<double>& components) const
{
  if (!uncertainty)
    throw Error (InvalidState,
		 "Calibration::CoherencyMeasurement::get_weighted_component",
		 "Uncertainty policy not set");

  Stokes< complex<double> > wc = uncertainty->get_weighted_components (matrix);

  components.resize (nconstraint);
  unsigned index = 0;

  for (unsigned i=0; i<4; i++)
  {
    components[index] = wc[i].real();
    index ++;

    if (nconstraint == 8)
    {
      components[index] = wc[i].imag();
      index ++;
    }
  }

  if (index != nconstraint)
    throw Error (InvalidState,
		 "Calibration::CoherencyMeasurement::get_weighted_component",
		 "index=%u != nconstraint=%u", index, nconstraint);
}
