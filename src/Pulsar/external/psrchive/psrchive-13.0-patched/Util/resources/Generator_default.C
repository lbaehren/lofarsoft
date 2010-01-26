/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG

#include "Predict.h"
#include "lazy.h"

using namespace std;

std::ostream& operator<< (std::ostream& ostr, Pulsar::Predictor::Policy p)
{
  if (p == Pulsar::Predictor::Input)
    ostr << "input";

  else if (p == Pulsar::Predictor::Default)
    ostr << "default";

  return ostr;
}

std::istream& operator>> (std::istream& istr, Pulsar::Predictor::Policy& p)
{
  std::string policy;
  istr >> policy;

  if (policy == "input")
    p = Pulsar::Predictor::Input;
  else if (policy == "default")
    p = Pulsar::Predictor::Default;
  else
    istr.setstate (std::istream::failbit);

  return istr;
}

LAZY_GLOBAL(Pulsar::Predictor, \
	    Configuration::Parameter<Pulsar::Predictor::Policy>, \
	    policy, Pulsar::Predictor::Input );

LAZY_GLOBAL(Pulsar::Generator, \
	    Configuration::Parameter<Pulsar::Generator*>, \
	    default_generator, 0);

Pulsar::Generator* Pulsar::Generator::get_default ()
{
  return get_default_generator().get_value()->clone ();
}

