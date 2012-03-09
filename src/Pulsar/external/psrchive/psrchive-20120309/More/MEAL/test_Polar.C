/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Polar.h"
#include "MEAL/Boost.h"
#include "MEAL/Rotation.h"

#include <iostream>

using namespace std;

int main ()
{
  MEAL::Function::verbose = true;

  double beta = 0.013;
  MEAL::Boost boost;
  boost.set_param (0, sinh(beta));

  double b00 = boost.evaluate()(0,0).real();
  double b11 = boost.evaluate()(1,1).real();
  double d0 = fabs(b00 - exp(beta));
  double d1 = fabs(b11 - exp(-beta));

  if ( d0 > 1e-10 || d1 > 1e-10) {
    cerr << "MEAL::Boost w/ beta=" << beta << " = " << boost.evaluate();
    cerr << "\nexp("<<beta<<")=" << exp(beta)
         << "  exp("<<-beta<<")=" << exp(-beta) 
         << "  d0=" << d0 << " d1=" << d1 << endl;
    return -1;
  }


  double phi = 0.145;
  MEAL::Rotation rotation;
  rotation.set_param (0, phi);

  if ( fabs( rotation.evaluate()(0,0).real() - cos(phi) ) > 1e-10 ||
       fabs( rotation.evaluate()(0,0).imag() - sin(phi) ) > 1e-10 )
    {
      cerr << "MEAL::Rotation w/ phi=" << phi << " = " << rotation.evaluate();
      cerr << "\ncos("<<phi<<")=" << cos(phi)
	   << "  sin("<<phi<<")=" << sin(phi) << endl;
      return -1;
    }


  MEAL::Polar polar;

  if (polar.evaluate() != Jones<double>::identity()) {
    cerr << "MEAL::Polar default != identity" << endl;
    return -1;
  }

  double gain = 5.4;
  polar.set_param (0, gain);
  if (polar.evaluate() != Jones<double>::identity() * gain) {
    cerr << "MEAL::Polar w/ gain="<<gain<<" != identity*gain" << endl;
    return -1;
  }

  polar.set_param (1, sinh(beta));
  if ( norm(polar.evaluate() - boost.evaluate() * gain) > 1e-10 ) {
    cerr << "MEAL::Polar w/ gain="<<gain<<",beta="<<beta
	 <<" = " << polar.evaluate()
	 << " != " << boost.evaluate()*gain << endl;
    return -1;
  }

  polar.set_param (4, phi);
  double diff;
  diff = norm(polar.evaluate() - boost.evaluate()*rotation.evaluate()*gain);
  if (diff > 1e-10 ) {
    cerr << "MEAL::Polar w/ gain="<<gain<<",beta="<<beta
	 <<" = " << polar.evaluate()
	 << " != " << boost.evaluate()*rotation.evaluate()*gain << endl;
    return -1;
  }

  cerr << "MEAL::Polar passes all tests" << endl;

  return 0;
}
