/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MeasurementEquation.h"
#include "MEAL/CongruenceTransformation.h"
#include "MEAL/MuellerTransformation.h"

using namespace std;

Calibration::MeasurementEquation::MeasurementEquation () 
  : composite (this), inputs (&composite), xforms (&composite)
{
  // set_verbose(true);
}

Calibration::MeasurementEquation::~MeasurementEquation ()
{
}

/*! This method unmaps the old input before mapping the new */
void Calibration::MeasurementEquation::set_input (Complex2* input)
{
  inputs.assign (input);
}

/*!
  \post current_input will be set to the newly added input
*/
void Calibration::MeasurementEquation::add_input (Complex2* input)
{
  inputs.push_back (input);
}

MEAL::Complex2* Calibration::MeasurementEquation::get_input ()
{
  return inputs.get_current ();
}

unsigned Calibration::MeasurementEquation::get_num_input () const
{
  return inputs.size();
}

unsigned Calibration::MeasurementEquation::get_input_index () const
{
  return inputs.get_index();
}

void Calibration::MeasurementEquation::set_input_index (unsigned index)
{
  inputs.set_index( index );
}

MEAL::Transformation<MEAL::Complex2>*
Calibration::MeasurementEquation::new_transformation (Complex2* xform)
{
  MEAL::CongruenceTransformation* cong = new MEAL::CongruenceTransformation;
  cong->set_composite (&composite);
  cong->set_transformation (xform);
  return cong;
}

/*! This method unmaps the old transformation before mapping the new */
void 
Calibration::MeasurementEquation::set_transformation (Complex2* xform)
{
  xforms.assign( new_transformation(xform) );
}

/*!
  \post current_xform will be set to the newly added transformation
*/
void Calibration::MeasurementEquation::add_transformation (Complex2* xform)
{
  xforms.push_back( new_transformation (xform) );
}

MEAL::Transformation<MEAL::Complex2>*
Calibration::MeasurementEquation::new_transformation (MEAL::Real4* xform)
{
  MEAL::MuellerTransformation* cong = new MEAL::MuellerTransformation;
  cong->set_composite (&composite);
  cong->set_transformation (xform);
  return cong;
}

/*! This method unmaps the old transformation before mapping the new */
void 
Calibration::MeasurementEquation::set_transformation (MEAL::Real4* xform)
{
  xforms.assign( new_transformation(xform) );
}

/*!
  \post current_xform will be set to the newly added transformation
*/
void Calibration::MeasurementEquation::add_transformation (MEAL::Real4* xform)
{
  xforms.push_back( new_transformation (xform) );
}

MEAL::Transformation<MEAL::Complex2>*
Calibration::MeasurementEquation::get_transformation ()
{
  return xforms.get_current ();
}

unsigned Calibration::MeasurementEquation::get_num_transformation () const
{
  return xforms.size();
}

unsigned Calibration::MeasurementEquation::get_transformation_index () const
{
  return xforms.get_index();
}

void
Calibration::MeasurementEquation::set_transformation_index (unsigned index)
{
  xforms.set_index( index );
}

//! Returns \f$ \rho^\prime_j \f$ and its gradient
void 
Calibration::MeasurementEquation::calculate (Jones<double>& result,
					     std::vector<Jones<double> >* grad)
try {
  if (verbose)
    cerr << "Calibration::MeasurementEquation::calculate nparam="
	 << get_nparam() << " policy=" << parameter_policy.ptr()
	 << " composite=" << &composite << endl;

  xforms.get_current()->set_input( inputs.get_projection() );

  result = xforms.get_current()->evaluate (grad);
}
catch (Error& error)
{
  throw error += "Calibration::MeasurementEquation::calculate";
}

