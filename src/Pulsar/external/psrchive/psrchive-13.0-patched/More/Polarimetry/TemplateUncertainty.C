/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TemplateUncertainty.h"
#include "MEAL/StokesError.h"

#include <iostream>

using namespace std;

// #define _DEBUG 1

complex<double> reim (double s)
{
  return complex<double> (s,s);
}

Stokes< complex<double> > duplicate (const Stokes<double>& s)
{
  return Stokes< complex<double> > (s, reim);
}

//! Default constructor
Calibration::TemplateUncertainty::TemplateUncertainty ()
{
  built = false;
}

Calibration::TemplateUncertainty*
Calibration::TemplateUncertainty::clone () const
{
  return new TemplateUncertainty (*this);
}

//! Set the uncertainty of the observation
void Calibration::TemplateUncertainty::set_variance
( const Stokes<double>& var )
{
  observation_variance = duplicate (var);
  built = false;
}

//! Set the uncertainty of the template
void Calibration::TemplateUncertainty::set_template_variance
( const Stokes<double>& var )
{
  template_variance = duplicate (var);
  built = false;
}

//! Set the uncertainty of the observation
void Calibration::TemplateUncertainty::set_variance
( const Stokes< complex<double> >& var )
{
  observation_variance = var;
  built = false;
}

//! Set the uncertainty of the template
void Calibration::TemplateUncertainty::set_template_variance
(const Stokes< complex<double> >& var)
{
  template_variance = var;
  built = false;
}

//! Set the transformation from template to observation
void Calibration::TemplateUncertainty::set_transformation (MEAL::Complex2* x)
{
  if (transformation)
    transformation->changed.disconnect (this, &TemplateUncertainty::changed);

  transformation = x;
  transformation->changed.connect (this, &TemplateUncertainty::changed);

  built = false;
}

const MEAL::Complex2* 
Calibration::TemplateUncertainty::get_transformation () const
{
  return transformation;
}


//! Given a coherency matrix, return the difference
double Calibration::TemplateUncertainty::get_weighted_norm
(const Jones<double>& matrix) const
{
  check_build ();
  return ObservationUncertainty::get_weighted_norm (matrix);
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::TemplateUncertainty::get_weighted_conjugate
(const Jones<double>& matrix) const try
{
  check_build ();
  return ObservationUncertainty::get_weighted_conjugate (matrix);
}
catch (Error& error)
{
  throw error += "Calibration::TemplateUncertainty::get_weighted_conjugate";
}

Stokes< complex<double> >
Calibration::TemplateUncertainty::get_weighted_components
( const Jones<double>& matrix ) const
{
  return ObservationUncertainty::get_weighted_components (matrix);
}

void Calibration::TemplateUncertainty::check_build () const
{
  // for now, always rebuild (MEAL changed callback is no longer used)

  // if (!built || !MEAL::Function::cache_results)
  const_cast<TemplateUncertainty*>(this)->build();
}

void Calibration::TemplateUncertainty::changed (MEAL::Function::Attribute a)
{
  if (a == MEAL::Function::Evaluation)
    built = false;

  // cerr << "Calibration::TemplateUncertainty::changed" << endl;
}

void Calibration::TemplateUncertainty::build ()
{
  MEAL::StokesError compute;

#ifdef _DEBUG
  cerr << "&xform=" << transformation.get() << endl;
  cerr << "xform=" << transformation->evaluate() << endl;
  cerr << "det(xform)=" << det(transformation->evaluate()) << endl;
#endif

  compute.set_transformation( transformation->evaluate() );

  Stokes<double> re = real( template_variance );
  Stokes<double> im = imag( template_variance );

#ifdef _DEBUG
  cerr << "in var=" << template_variance << endl;
#endif

  compute.set_variance (re);
  re = compute.get_variance();

  compute.set_variance (im);
  im = compute.get_variance();

  Stokes< complex<double> > variance;
  for (unsigned i=0; i<4; i++)
    variance[i] = complex<double>( re[i], im[i] );

#ifdef _DEBUG
  cerr << "out var=" << variance << endl;
#endif

  variance += observation_variance;

  ObservationUncertainty::set_variance (variance);

  built = true;
}

