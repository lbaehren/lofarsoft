/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Boost1.h"
#include "MEAL/OneParameter.h"
#include "Pauli.h"
#include "stringtok.h"

using namespace std;

void MEAL::Boost1::init ()
{
  OneParameter* param = new OneParameter (this);
  param->set_name ("boost");
}

MEAL::Boost1::Boost1 (const Vector<3,double>& _axis) 
{
  init ();
  set_axis (_axis);
}

MEAL::Boost1::Boost1 (const Boost1& copy) 
{
  init ();
  set_axis( copy.get_axis() );
  set_beta( copy.get_beta() );
}

MEAL::Boost1*  MEAL::Boost1::clone () const
{
  return new Boost1(*this);
}

void MEAL::Boost1::set_param_name (const string& name) 
{
  OneParameter* current = dynamic_kast<OneParameter>(parameter_policy);
  if (current)
    current->set_name (name);
}

void MEAL::Boost1::set_param_description (const string& name) 
{
  OneParameter* current = dynamic_kast<OneParameter>(parameter_policy);
  if (current)
    current->set_description (name);
}

//! Return the name of the class
string MEAL::Boost1::get_name () const
{
  return "Boost1";
}

void MEAL::Boost1::set_axis (const Vector<3,double>& _axis)
{
  double norm = _axis * _axis;
  axis = _axis / sqrt(norm);
}

//! Get the unit-vector along which the boost occurs
Vector<3, double> MEAL::Boost1::get_axis () const
{
  return axis;
}

void MEAL::Boost1::set_beta (const Estimate<double>& beta)
{
  set_Estimate (0, beta);
}

Estimate<double> MEAL::Boost1::get_beta () const
{
  return get_Estimate (0);
}

//! Return the Jones matrix and its gradient
void MEAL::Boost1::calculate (Jones<double>& result,
			      vector<Jones<double> >* grad)
{
  double beta = get_param(0);

  if (verbose)
    cerr << "MEAL::Boost1::calculate axis=" << axis 
	 << " beta=" << beta << endl;

  double sinh_beta = sinh (beta);
  double cosh_beta = cosh (beta);

  Quaternion<double, Hermitian> boost (cosh_beta, sinh_beta*axis);
  result = convert (boost);

  if (!grad)
    return;

  Quaternion<double, Hermitian> dboost_dbeta (sinh_beta, cosh_beta*axis);
  (*grad)[0] = convert (dboost_dbeta);

  if (verbose) {
    cerr << "MEAL::Boost1::calculate gradient" << endl;
    cerr << "   " << (*grad)[0] << endl;
  }
  
}


//! Parses the values of model parameters and fit flags from a string
void MEAL::Boost1::parse (const string& line)
{
  string temp = line;

  // the key should be the name of the parameter to be set
  string key = stringtok (temp, " \t");

  if (verbose)
    cerr << "MEAL::Boost1::parse key '" << key << "'" << endl;

  if (key == "axis") {

    if (verbose)
      cerr << "MEAL::Boost1::parse value " << temp << endl;

    Vector<3,double> v = fromstring< Vector<3,double> >( temp );
    set_axis (v);

  }
  else
    Function::parse (line);
}

//! Prints the values of model parameters and fit flags to a string
void MEAL::Boost1::print (string& text) const
{
  text = get_name ();
  text += "\n axis " + tostring(axis);
  print_parameters (text, "\n ");
}
