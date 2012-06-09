/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Rotation1.h"
#include "Pauli.h"

#include <iostream>
using namespace std;

static char* name = "QUV";

static complex<double> Ci (0, 1);

static Jones<double> identity (1, 0,
                               0, 1);

static Jones<double> sigma[3];

void init ()
{
  sigma[0] = Jones<double> (1, 0,
                            0, -1);

  sigma[1] = Jones<double> (0, 1,
                            1, 0);

  sigma[2] = Jones<double> (0, -Ci,
                            Ci, 0);
}

void test_Rotation (double theta, double phi, unsigned axis, unsigned perm)
{
  MEAL::Rotation1 rotation (Vector<3,float>::basis(axis));
  rotation.set_param (0, phi);

  Jones<double> xform = rotation.evaluate();

  Stokes<double> stokes (1.5);
  unsigned i = (axis+perm+1)%3;
  unsigned j = (axis+perm+2)%3;
  unsigned k = (axis+perm)%3;

  Jones<double> expect = cos(phi) * identity + sin(phi) * Ci * sigma[axis];

  if ( norm (xform - expect) != 0 )
  {
    cerr << "rotation[" << axis << "," << phi << "]=" << xform << endl;
    cerr << "expected=" << expect << endl;
    cerr << "norm=" << norm (xform - expect) << endl;
    exit (-1);
  }

  stokes[i+1] = cos(theta);
  stokes[j+1] = sin(theta);
  stokes[k+1] = 0.2;

  stokes = transform (stokes, xform);

  double result = theta - 2.0*phi;

  if (fabs (stokes[0] - 1.5) > 1e-12)
  {
    cerr << "I=" << stokes[0] << " != " << 1.5 << endl;
    exit(-1);
  }

  if (fabs (stokes[i+1] - cos(result)) > 1e-12)
  {
    cerr << name[i] << "=" << stokes[i+1] << " != " << cos(result) << endl;
    exit(-1);
  }

  if (fabs (stokes[j+1] - sin(result)) > 1e-12)
  {
    cerr << name[j] << "=" << stokes[j+1] << " != " << sin(result) << endl;
    exit(-1);
  }

  if (fabs (stokes[k+1] - 0.2) > 1e-12) {
    cerr << name[k] << "=" << stokes[k+1] << " != " << 0.2 << endl;
    exit(-1);
  }

}
 
void test_loop (unsigned axis, unsigned perm)
{
  double increment = M_PI/87.0;

  for (double theta = -2*M_PI; theta < 2*M_PI+increment; theta += increment)
    for (double phi = -M_PI; phi < M_PI+increment; phi += increment)
      test_Rotation (theta, phi, axis, perm);
}

int main ()
{
  init ();

  cerr << "Testing in linear basis" << endl;
  for (unsigned axis=0; axis < 3; axis++)
  {
    cerr << "Testing rotations about " << name[axis] << " axis" << endl;
    test_loop (axis, 0);
  }

  cerr << "Testing in circular basis" << endl;
  Pauli::basis().set_basis (Signal::Circular);

  for (unsigned axis=0; axis < 3; axis++)
  {
    cerr << "Testing rotations about " << name[(axis+2)%3] << " axis" << endl;
    test_loop (axis, 2);
  }

  cerr << "All tests passed" << endl;
  return 0;
}

