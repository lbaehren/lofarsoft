/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef HAVE_COMPLEX_TEMPLATE

#include "MEAL/Polar.h"
#include "Error.h"

//! Polar decompose the Jones matrix
void MEAL::Polar::solve (Jones< Estimate<double> >& j)
{
  throw Error (InvalidState, "MEAL::Polar::solve",
               "faulty complex template class; method not implemented");
}

#else

#include "MEAL/Polar.h"

#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"

#include "Pauli.h"

using namespace std;

complex<MEAL::ScalarMath> 
ComplexScalar (const complex< Estimate<double> >& z)
{
  return complex<MEAL::ScalarMath>
    ( *(new MEAL::ScalarParameter( z.real() )),
      *(new MEAL::ScalarParameter( z.imag() )) );
}

//! Polar decompose the Jones matrix
void MEAL::Polar::solve (Jones< Estimate<double> >& j)
{
  Jones<ScalarMath> jones
    ( ComplexScalar(j.j00), ComplexScalar(j.j01),
      ComplexScalar(j.j10), ComplexScalar(j.j11) );

  complex<ScalarMath> determinant (0.0,0.0);
  Quaternion<ScalarMath,Hermitian> boost;
  Quaternion<ScalarMath,Unitary>   rotation;

  polar( determinant, boost, rotation, jones);
      
  set_gain( determinant.real().get_Estimate() );

  for (unsigned i=0; i<3; i++) {
    set_boost( i, boost[i+1].get_Estimate() );
    set_rotation( i, rotation[i+1].get_Estimate() );
  } 
}

#endif

