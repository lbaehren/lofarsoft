/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/*
  This program tests that:
  1) the argument to a Univariate<> derived class is handled proplery
  2) the argument is propagated when part of a Composite
*/

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"

#include "MEAL/NoParameters.h"

using namespace std;
using namespace MEAL;

/* This class tests simple use of Univariate<Scalar>.  The class has
   no free parameters and its return value is simply twice its
   argument value. */
class test_UnivariateScalar : public Univariate<Scalar>
{
public:
  test_UnivariateScalar () { parameter_policy = new NoParameters; }

  std::string get_name () const { return "test_Univariate"; }

  void calculate (double& result, std::vector<double>*) 
  { result = get_abscissa() * 2; }
};

#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"

/* This class tests slightly more complicated use of
   Univariate<Scalar>.  Here, the evaluation expression is calculated
   using ScalarMath and the policies of the result are copied. */
class test_UnivariateScalar2 : public Univariate<Scalar>
{
public:

  test_UnivariateScalar2 () {

    ScalarArgument* argument = new ScalarArgument; 
    
    ScalarMath result = *argument + my_a + my_b;
    expression = result.get_expression();

    copy_parameter_policy  (expression);
    copy_evaluation_policy (expression);
    copy_univariate_policy (argument);

  }

  std::string get_name () const { return "test_Univariate2"; }

  void calculate (double& result, std::vector<double>*) { }

  test_UnivariateScalar my_a;
  test_UnivariateScalar my_b;

protected:
  Reference::To<Scalar> expression;
};

#include "MEAL/Axis.h"
#include "MEAL/SumRule.h"
#include "MEAL/ProductRule.h"

int main () try {

  test_UnivariateScalar  u_scalar1;
  test_UnivariateScalar2 u_scalar2;

  SumRule<Scalar> s_scalar;

  s_scalar += &u_scalar1;
  s_scalar += &u_scalar2;

  u_scalar1.set_abscissa( 3.4 );
  if (u_scalar1.get_abscissa() != 3.4) {
    cerr << "simple set_abscissa failed" << endl;
    return -1;
  }
  if (u_scalar2.get_abscissa() == 3.4) {
    cerr << "simple set_abscissa somehow confused in Composite" << endl;
    return -1;
  }

  Axis<double> axis;
  s_scalar.set_argument( 0, &axis );

  axis.set_value( 5.6 );

  if (u_scalar1.get_abscissa() != 5.6) {
    cerr << "Composite set_abscissa failed 1" << endl;
    return -1;
  }
  if (u_scalar2.get_abscissa() != 5.6) {
    cerr << "Composite set_abscissa failed 2" << endl;
    return -1;
  }
  if (u_scalar2.my_a.get_abscissa() != 5.6) {
    cerr << "Composite set_abscissa failed 2a" << endl;
    return -1;
  }
  if (u_scalar2.my_b.get_abscissa() != 5.6) {
    cerr << "Composite set_abscissa failed 2b" << endl;
    return -1;
  }

  double expect = 5.6 /* argument */ + 5.6 * 2 /* my_a */ + 5.6 * 2 /* my_b */;

  if (u_scalar2.evaluate () != expect) {
    cerr << "after Axis<double>::set_value, evaluate failed" << endl;
    cerr << "  test_UnivariateScalar2::evaluate=" << u_scalar2.evaluate()
	 << endl;
    cerr << "  expected " << expect << endl;
    return -1;
  }

  cerr << "all tests of Univariate<Scalar> passed" << endl;

  return 0;
}
catch (Error& error) {
  cerr << "test_Univariate: " << error << endl;
}
