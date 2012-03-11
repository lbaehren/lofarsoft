/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolynomialInfo.h"
#include "templates.h"

#include <assert.h>

using namespace std;

//! Constructor
Pulsar::PolynomialInfo::PolynomialInfo (const SystemCalibrator* c, Which w)
  : VariationInfo (c, w)
{
  if (Calibrator::verbose)
    cerr << "Pulsar::PolynomialInfo::PolynomialInfo" << endl;

  unsigned nchan = calibrator->get_nchan ();
  ncoef = 0;

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    const MEAL::Polynomial* poly = get_Polynomial( ichan );
    if (poly) {
      ncoef = poly->get_nparam();
      break;
    }
  }

  cerr << "Pulsar::PolynomialInfo ncoef=" << ncoef << endl;
}

const MEAL::Polynomial*
Pulsar::PolynomialInfo::get_Polynomial (unsigned ichan) const
{
  return dynamic_cast<const MEAL::Polynomial*>( get_Scalar(ichan) );
}

string Pulsar::PolynomialInfo::get_title () const
{
  return "Polynomial Coefficients";
}

//! Return the number of parameter classes
unsigned Pulsar::PolynomialInfo::get_nclass () const
{
  // a separate panel for each coefficient
  return ncoef;
}
    
//! Return the name of the specified class
string Pulsar::PolynomialInfo::get_name (unsigned iclass) const
{
  return VariationInfo::get_name (iclass) + "\\d" + tostring(iclass) + "\\u";
}

//! Return the number of parameters in the specified class
unsigned Pulsar::PolynomialInfo::get_nparam (unsigned iclass) const
{
  return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::PolynomialInfo::get_param (unsigned ichan, unsigned iclass,
				   unsigned iparam) const
{
  assert( iclass < ncoef );

  const MEAL::Polynomial* poly = get_Polynomial( ichan );
  if (!poly)
    return 0;

  return poly->get_Estimate (iclass);
}
