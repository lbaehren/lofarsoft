/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/SingleAxisPolynomial.h"
#include "Pulsar/SingleAxis.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"
#include "random.h"

using namespace std;

/*! Each of the free parameters will be constrained by a Polynomial Function */
const unsigned Calibration::SingleAxisPolynomial::npoly = 3;

void Calibration::SingleAxisPolynomial::init (unsigned ncoef)
{
  // name = "SingleAxisPolynomial";

  xform = new Calibration::SingleAxis;
  set_model (xform);

  if (xform->get_nparam() != npoly)
    throw Error (InvalidState, "Calibration::SingleAxisPolynomial",
		 "SingleAxis::nparam=%d != npoly=%d", 
		 xform->get_nparam(), npoly);

  polynomial.resize (npoly);

  for (unsigned ipoly=0; ipoly<npoly; ipoly++)  {

    polynomial[ipoly] = new MEAL::Polynomial(ncoef);

    // at t==0, the backend should represent the identity matrix
    if (ncoef && ipoly==0)
      polynomial[ipoly]->set_param (0, 1.0);  // Gain
    
    set_constraint (ipoly, polynomial[ipoly]);
    
  }
}

Calibration::SingleAxisPolynomial::SingleAxisPolynomial (unsigned ncoef)
{
  init (ncoef);
}

//! Copy constructor
Calibration::SingleAxisPolynomial::SingleAxisPolynomial
(const SingleAxisPolynomial& sa)
{
  init (sa.polynomial[0]->get_nparam());
  operator = (sa);
}

//! Assignment operator
Calibration::SingleAxisPolynomial&
Calibration::SingleAxisPolynomial::operator = (const SingleAxisPolynomial& sa)
{
  if (this == &sa)
    return *this;

  *(xform) = *(sa.xform);

  for (unsigned ipoly=0; ipoly<npoly; ipoly++)
    *(polynomial[ipoly]) = *(sa.polynomial[ipoly]);

  return *this;
}

//! Destructor
Calibration::SingleAxisPolynomial::~SingleAxisPolynomial ()
{
}

//! Return the name of the class
string Calibration::SingleAxisPolynomial::get_name () const
{
  return "SingleAxisPolynomial";
}

void Calibration::SingleAxisPolynomial::set_argument (unsigned dimension,
						      MEAL::Argument* argument)
{
  if (dimension != 0)
    return;

  MEAL::Axis<double>* axis = dynamic_cast< MEAL::Axis<double>* > (argument);
  if (!axis)
    return;

  axis->signal.connect (this,
			&Calibration::SingleAxisPolynomial::set_abscissa);
}

/*! 
  \param max_result the maximum value produced by the polynomials
  \param max_abscissa the maximum absolute value of the abscissa
*/
void Calibration::SingleAxisPolynomial::random (double max_result,
						double max_abscissa)
{
  for (unsigned ipoly=0; ipoly<npoly; ipoly++)  {

    unsigned ncoef = polynomial[ipoly]->get_nparam();

    // initialize a random Polynomial
    float max_coef = max_result / double (ncoef);

    for (unsigned icoef=1; icoef<ncoef; icoef++) {

      max_coef /= max_abscissa;

      float coef = 0.0;
      random_value (coef, max_coef);

      polynomial[ipoly]->set_param (icoef, coef);
      
      if (verbose)
	cerr << "Calibration::SingleAxisPolynomial::random"
	  " polynomial[" << ipoly << "][" << icoef << "]=" 
	     << polynomial[ipoly]->get_param (icoef) << endl;

    }

  }
}

//! Set the abscissa of each of the polynomial functions
void Calibration::SingleAxisPolynomial::set_abscissa (double value)
{
  for (unsigned ipoly=0; ipoly<npoly; ipoly++)
    polynomial[ipoly]->set_abscissa (value);
}


void Calibration::SingleAxisPolynomial::set_infit (unsigned coef, bool fit)
{
  for (unsigned ipoly=0; ipoly<npoly; ipoly++)
    polynomial[ipoly]->set_infit (coef, fit);
}
