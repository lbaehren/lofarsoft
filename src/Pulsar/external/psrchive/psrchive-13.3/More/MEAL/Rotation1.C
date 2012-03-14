/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Rotation1.h"
#include "MEAL/OneParameter.h"
#include "Pauli.h"
#include "stringtok.h"

using namespace std;

void MEAL::Rotation1::init ()
{
  OneParameter* param = new OneParameter (this);
  param->set_name ("rotation");
}

MEAL::Rotation1::Rotation1 (const Vector<3,double>& _axis) 
{
  init ();
  set_axis( _axis );
}

MEAL::Rotation1::Rotation1 (const Rotation1& copy) 
{
  init ();
  set_axis( copy.get_axis() );
  set_phi( copy.get_phi() );
}

MEAL::Rotation1*  MEAL::Rotation1::clone () const
{
  return new Rotation1(*this);
}

//! Return the name of the class
std::string MEAL::Rotation1::get_name () const
{
  return "Rotation1";
}

void MEAL::Rotation1::set_param_name (const string& name) 
{
  OneParameter* current = dynamic_kast<OneParameter>(parameter_policy);
  if (current)
    current->set_name (name);
}

void MEAL::Rotation1::set_param_description (const string& name) 
{
  OneParameter* current = dynamic_kast<OneParameter>(parameter_policy);
  if (current)
    current->set_description (name);
}

void MEAL::Rotation1::set_axis (const Vector<3,double>& _axis)
{
  if (normsq(_axis) == 0.0)
    throw Error (InvalidState, "MEAL::Rotation1::set_axis",
		 "invalid rotation axis = 0");

  axis = _axis;
}

Vector<3,double> MEAL::Rotation1::get_axis () const
{
  return axis;
}


void MEAL::Rotation1::set_phi (const Estimate<double>& radians)
{
  if (verbose)
    cerr << "MEAL::Rotation1::set_phi " << radians << endl;

  set_Estimate (0, radians);
}

Estimate<double> MEAL::Rotation1::get_phi () const
{
  return get_Estimate (0);
}

void MEAL::Rotation1::set_parameter_policy (OneParameter* policy)
{
  if (policy) {
    OneParameter* current = dynamic_kast<OneParameter>(parameter_policy);
    if (current)
      *policy = *current;
  }
  Function::set_parameter_policy( policy );
}

//! Calculate the Jones matrix and its gradient
void MEAL::Rotation1::calculate (Jones<double>& result,
				 std::vector<Jones<double> >* grad)
{
  double phi = get_param(0);

  double sin_phi = sin (phi);
  double cos_phi = cos (phi);

  Quaternion<double, Unitary> rotation (cos_phi, sin_phi*axis);
  result = convert (rotation);

  if (verbose)
    cerr << "MEAL::Rotation1::calculate axis=" << axis 
	 << " phi=" << phi << " det(J)=" << det(result) << endl;

  if (!grad)
    return;

  Quaternion<double, Unitary> drotation_dphi (-sin_phi, cos_phi*axis);
  (*grad)[0] = convert (drotation_dphi);

  if (verbose)
    cerr << "MEAL::Rotation1::calculate gradient" << endl
	 << "   " << (*grad)[0] << endl;
}

//! Parses the values of model parameters and fit flags from a string
void MEAL::Rotation1::parse (const string& line)
{
  string temp = line;

  // the key should be the name of the parameter to be set
  string key = stringtok (temp, " \t");

  if (verbose)
    cerr << "MEAL::Rotation1::parse key '" << key << "'" << endl;

  if (key == "axis") {

    if (verbose)
      cerr << "MEAL::Rotation1::parse value " << temp << endl;

    Vector<3,double> v = fromstring< Vector<3,double> >( temp );
    set_axis (v);

  }
  else
    Function::parse (line);
}

//! Prints the values of model parameters and fit flags to a string
void MEAL::Rotation1::print (string& text) const
{
  text = get_name ();
  text += "\n axis " + tostring(axis);
  print_parameters (text, "\n ");
}
