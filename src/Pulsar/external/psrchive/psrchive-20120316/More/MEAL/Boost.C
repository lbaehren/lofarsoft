/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Boost.h"
#include "MEAL/Parameters.h"
#include "Pauli.h"

using namespace std;

MEAL::Boost::Boost ()
{
  Parameters* params = new Parameters (this, 3);
  for (unsigned i=0; i<3; i++)
  {
    string index = tostring (i + 1);
    params->set_name (i, "b_" + index);
    params->set_description (i, "boost axis_" + index + " times sinh(boost)");
  }
}

//! Return the name of the class
string MEAL::Boost::get_name () const
{
  return "Boost";
}

//! Get the unit-vector along which the boost occurs
Vector<3, double> MEAL::Boost::get_axis () const
{
  Vector<3, double> Gibbs;
  for (unsigned i=0; i<3; i++)
    Gibbs[i] = get_param(i);
  double mod = sqrt (Gibbs * Gibbs);

  if (mod != 0.0)
    Gibbs /= mod;

  return Gibbs;
}

double MEAL::Boost::get_beta () const
{
  Vector<3, double> Gibbs;
  for (unsigned i=0; i<3; i++)
    Gibbs[i] = get_param(i);
  double mod = sqrt (Gibbs * Gibbs);

  return asinh (mod);
}

//! Calculate the Jones matrix and its gradient
void MEAL::Boost::calculate (Jones<double>& result,
			     vector<Jones<double> >* grad)
{
  Vector<3, double> Gibbs;
  for (unsigned i=0; i<3; i++)
    Gibbs[i] = get_param(i);

  if (verbose)
    cerr << "MEAL::Boost::calculate Gibbs=" << Gibbs << endl;

  // calculate the Boost component
  double norm_Gibbs = Gibbs * Gibbs;
  double cosh_beta = sqrt (1.0 + norm_Gibbs);

  // the Boost quaternion
  Quaternion<double, Hermitian> boost (cosh_beta, Gibbs);

  result = convert (boost);

  if (!grad)
    return;

  // build the partial derivatives with respect to boost Gibbs-vector
  double inv_cosh_beta = 1.0 / cosh_beta;
  
  for (unsigned i=0; i<3; i++) {
    
    // partial derivative of Boost[0]=cosh(beta) wrt Boost[i+1]=boost[i]
    double dcosh_beta_dGibbsi = Gibbs[i] * inv_cosh_beta;
    
    Quaternion<double, Hermitian> dboost_dGibbsi;
    dboost_dGibbsi[0] = dcosh_beta_dGibbsi;
    dboost_dGibbsi[i+1] = 1.0;
    
    // set the partial derivative wrt this parameter
    (*grad)[i] = convert (dboost_dGibbsi);
    
  }
  
  if (verbose) {
    cerr << "MEAL::Boost::calculate gradient" << endl;
    for (unsigned i=0; i<3; i++)
      cerr << "   " << (*grad)[i] << endl;
  }

}


