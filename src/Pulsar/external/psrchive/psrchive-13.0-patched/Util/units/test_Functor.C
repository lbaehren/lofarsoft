/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Functor.h"

#include <string>
#include <math.h>

using namespace std;

class parent : public Reference::Able {

public:
  bool less_than (int i1, int i2) { return i1 < i2; };
  double exponential (double x) { return exp (x); }

};

class child : public parent {

public:
  string get_text () { return "child::get_text = " + text; }
  string text;

};

void call_this () { cerr << "call_this called" << endl; }

float ignored (int integer) { return float(integer) * 5.0; }

double my_pow (double base, double exponent) { return pow (base, exponent); }

int main ()
{
  parent expression;

  //
  // test of binary method
  //

  // note that the arguments and return type need not necessarily match
  Functor< int(char, char) > lt (&expression, &parent::less_than);

  cerr << "lt(a,b)=" << lt ('a', 'b') << endl;

  if (lt ('a', 'b') != 1) {
    cerr << "ERROR" << endl;
    return -1;
  }

  child exp_child;

  //
  // test of unary method
  //

  // note that the pointer need not be cast to parent
  Functor< float(float) > my_exp (&exp_child, &parent::exponential);

  cerr << "my_exp(1)=" << my_exp(1.0) << endl;

  if (fabs(my_exp(1.0) - exp(1.0)) > 1e-7) {
    cerr << "ERROR" << endl;
    return -1;
  }

  //
  // test of generator method
  //

  exp_child.text = "some text";

  Functor< string() > ascii (&exp_child, &child::get_text);

  cerr << "ascii()='" << ascii() << "'" << endl;


  //
  // test of generator function
  //

  Functor< void() > call_something (&call_this);

  call_something ();


  //
  // test of unary function
  //

  // note that the floating point return value is cast to void (ignored)
  Functor< void(char) > ignore_result (&ignored);

  // test the set method
  my_exp.set (&ignored);

  ignore_result (3);


  //
  // test of binary function
  //

  Functor< double(double, double) > some_function (&my_pow);

  some_function.set (&my_pow);

  cerr << "my_pow(2,2)=" << some_function (2.0, 2.0) << endl;

  if (some_function (2.0, 2.0) != 4.0) {
    cerr << "ERROR" << endl;
    return -1;
  }


  cerr << "All Functors successfully tested." << endl;

  return 0;
}

