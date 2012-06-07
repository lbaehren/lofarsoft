/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/*!
 * test_copy.C
 *
 * Owing to the sometimes complicated nature of the inheritance tree,
 * the compiler-produced copy constructors do no always work as
 * expected (i.e. they segfault).  This little program is used to
 * check that copy construction works for three "leaf" classes,
 * Calibration::Feed, MEAL::Polar, and MEAL::Polynomial
 *
 * NOTE: vector resize uses the copy constructor
 */


#include "Pulsar/SingleAxisPolynomial.h"
#include "Pulsar/Feed.h"
#include "MEAL/Polar.h"
#include "MEAL/Polynomial.h"

#include <inttypes.h>

using namespace std;

template <class Type> void test_copy (Type* copy, Type* instance, 
                                      const string& name)
{
  if (instance->get_nparam() != copy->get_nparam())
    throw Error (InvalidState, "test_copy",
                 "copy.nparam=%d != nparam=%d after %s",
                 copy->get_nparam(), instance->get_nparam(), name.c_str());

  for (unsigned i=0; i<instance->get_nparam(); i++)
    if (copy->get_param (i) != instance->get_param(i))
      throw Error (InvalidState, "test_copy",
                   "unequal param[%d] %lf!=%lf after %s",
		   i, copy->get_param(i), instance->get_param(i),
		   name.c_str());
}


template <class Type> void test_copy (Type* instance,
				      const string& name)
{
  // test if the Reference::Able class is properly initialized
  if ( !instance->__is_on_heap() )
    throw Error (InvalidState, "test_copy", name + " is not on heap");

  cerr << "test_copy: "<< name <<"::nparam="<< instance->get_nparam() << endl;

  // set the parameters of the input instance
  for (unsigned i=0; i<instance->get_nparam(); i++)
    instance->set_param (i, double(i));

  cerr << "test_copy: " << name << " default constructor" << endl;
  Reference::To<Type> default_instance = new Type;

  cerr << "test_copy: " << name << " operator =" << endl;
  *default_instance = *instance;
  test_copy (default_instance.get(), instance, name + " assignment operator");

  cerr << "test_copy: " << name << " copy constructor" << endl;

  Reference::To<Type> copy = new Type (*instance);
  test_copy (copy.get(), instance, name + " copy constructor");

  unsigned nparam = copy->get_nparam();

  // ensure that the copy has not merely mapped instance into itself!
  cerr << "test_copy: " << name << " destructor" << endl;
  delete instance;

  if (copy->get_nparam() != nparam)
    throw Error (InvalidState, "test_copy",
		 "copy.nparam=%d != nparam=%d after %s destructor", 
		 copy->get_nparam(), nparam, name.c_str());
  
  vector<Type> vinstance;

  cerr << "test_copy: resizing vector<" << name << ">" << endl;
  vinstance.resize ( 128 );
}

template <class Type> void test_leak (Type* instance,
				      const string& name)
{
  uint64_t instances = Reference::Able::get_instance_count();

  test_copy (instance, name);

  if (instances < Reference::Able::get_instance_count()) {
    Error error (InvalidState, "test_leak");
    error << "memory leak in=" << instances << " out="
	  << Reference::Able::get_instance_count();
    throw error;
  }
}

int main (int argc, char** argv)
{ try {

  // Calibration::Model::verbose = true;

  cerr << "test_copy: testing MEAL::Polynomial" << endl;
  test_leak (new MEAL::Polynomial(12), "MEAL::Polynomial");

  cerr << "test_copy: testing MEAL::Polar" << endl;
  test_leak (new MEAL::Polar, "MEAL::Polar");

  cerr << "test_copy: testing Calibration::Feed" << endl;
  test_leak (new Calibration::Feed, "Calibration::Feed");

  cerr << "test_copy: testing Calibration::SingleAxisPolynomial" << endl;
  test_leak (new Calibration::SingleAxisPolynomial(5),
	     "Calibration::SingleAxisPolynomial");

} catch (Error& error) {
  cerr << error << endl;
  return -1;
}

  cerr << "test_copy: copy constructors pass test" << endl;
  return 0;
}
