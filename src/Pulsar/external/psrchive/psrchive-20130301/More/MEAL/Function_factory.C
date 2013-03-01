/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Agent.h"
#include "MEAL/Function.h"

using namespace MEAL;

#include "MEAL/Polynomial.h"
static Registry::List<Agent>::Enter< Advocate<Polynomial> >
polynomial;

#include "MEAL/Gaussian.h"
static Registry::List<Agent>::Enter< Advocate<Gaussian> >
gaussian;

#include "MEAL/SumRule.h"
static Registry::List<Agent>::Enter< Advocate< SumRule<Scalar> > >
scalar_sum;

#include "MEAL/Rotation1.h"
static Registry::List<Agent>::Unary< Advocate<Rotation1>, Vector<3,double> >
rotation1( Vector<3,double>::basis(0) );

#include "MEAL/Boost1.h"
static Registry::List<Agent>::Unary< Advocate<Boost1>, Vector<3,double> >
boost1( Vector<3,double>::basis(0) );

#include "MEAL/Depolarizer.h"
static Registry::List<Agent>::Enter< Advocate<Depolarizer> >
depolarizer;

using namespace std;

//! Construct a new model instance from a string
MEAL::Function* MEAL::Function::factory (const std::string& text)
{
  Registry::List<Agent>& registry = Registry::List<Agent>::get_registry();

  for (unsigned agent=0; agent<registry.size(); agent++)
  {
    if (registry[agent]->get_name () == text)
    {
      if (verbose)
        cerr << "MEAL::Function::factory using " 
	     << registry[agent]->get_name() << endl;

      return registry[agent]->new_Function(); 
    }
  }

  throw Error (InvalidParam, "MEAL::Function::factory",
	       "no match for '%s' in %d agents", text.c_str(),
	       registry.size());
}
