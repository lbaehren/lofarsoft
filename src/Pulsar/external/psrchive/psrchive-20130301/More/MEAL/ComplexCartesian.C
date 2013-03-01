/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ComplexCartesian.h"
#include "MEAL/Parameters.h"

using namespace std;

MEAL::ComplexCartesian::ComplexCartesian ()
{
  Parameters* params = new Parameters (this, 2);
  params->set_name (0, "real");
  params->set_name (1, "imag");
}

//! Calculate the Jones matrix and its gradient
void MEAL::ComplexCartesian::calculate (Result& result,
					vector<Result>* grad)
{
  result = std::complex<double>( get_param(0), get_param(1) );

  if (verbose)
    cerr << "MEAL::ComplexCartesian::calculate z=" << result << endl;

  if (grad)
  {
    (*grad)[0] = std::complex<double>( 1, 0 );
    (*grad)[1] = std::complex<double>( 0, 1 );
  }
}
