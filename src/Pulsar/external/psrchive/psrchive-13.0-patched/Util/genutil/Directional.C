/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Directional.h"

// #define _DEBUG

#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

Directional::Directional ()
{
  vertical = 0;
}

double Directional::get_vertical () const
{
  build ();
  return vertical;
}

double Directional::get_parallactic_angle () const
{
  return get_vertical ();
}

void Directional::build () const
{
  if (get_built())
    return;

  Mount::build ();

  /*
    vector from source in reference frame of observatory
    note that source_basis[2] points from source to centre of Earth
  */
  Vector<3,double> from_source = observatory_basis * source_basis[2];

  Matrix<3,3,double> basis = get_basis (from_source);

  double alignment = basis[2] * from_source;
  if ( fabs( alignment - 1.0 ) > 1e-12 )
  {
    Error error (InvalidState, "Directional::build",
		 "basis does not point at source");
    error << "\n\tbasis[2]=" << basis[2]
	  << "\n\tfrom_source=" << from_source;
    throw error;
  }

  /*
    vector to north in reference frame of receptors
  */
  Vector<3,double> north = basis * observatory_basis * source_basis[0];

  vertical = - atan2 (north[1], north[0]);

#ifdef _DEBUG
  cerr << "Directional::build vertical=" << vertical << endl;
#endif
}
