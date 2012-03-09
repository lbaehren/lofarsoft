/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>

using namespace std;

#include "Callback.h"

// A class "klass" must inherit the Reference::Able base class
// in order to register its callbacks with a Callback
// Otherwise, its destruction would lead to a dangling reference inside
// the callback sender.
class tester : public Reference::Able {

protected:
  int value;

public:
  tester (int val) { value = val; };

  int getval () const { return value; };
  void setval (int val) { value = val; };
};

// because Callback<Type>::connect is a template method, any class
// can register its methods for callback
class testmore : public Reference::Able {
public:

  void echo (int val) { cerr << "testmore::echo " 
			     << val << " called" << endl; };

  int good (float val) { cerr << "testmore::good " 
			     << val << " called" << endl; return -1; };

};

// callback class must inherit Reference::Able
class testfail {
public:
  void cannot (const int& junk);
};


template <class Type> class mysig : public Callback<Type> 
{
  public:
    int get_value () { return 4; }
};

void test_function (int arg)
{
  cerr << "test function " << arg << endl;
}

int main (int argc, char** argv)
{
  // the callback sender
  Callback<int> signal;

  signal.connect (&test_function);

  // a test of Callback inheritance
  mysig<double> child_signal;

  //#define TEST_COMPILE_TIME_ERROR
#ifdef TEST_COMPILE_TIME_ERROR
  testfail fail;
  signal.connect (fail, &testfail::cannot);
#endif

  // to instances of "tester" to connect to signal
  tester a(5);
  tester b(6);

  cerr << "value of a:" << a.getval() << endl;
  cerr << "value of b:" << b.getval() << endl;

  signal.connect (&a, &tester::setval);
  signal.connect (&b, &tester::setval);

  // a dynamic instance of a "testmore"
  testmore* c = new testmore;
  signal.connect (c, &testmore::echo);
  signal.connect (c, &testmore::good);

  // and a dynamic reference to the above instance
  Reference::To<testmore>* dr = new Reference::To<testmore>(c);
  // by calling connect twice on the same instance, 
  // c->echo should be called twice
  signal.connect (*dr, &testmore::echo);

  cerr << "Sending callback 3" << endl;
  signal (3);

  // after sending the signal, the values of a and b should have
  // been modified, and echo should have printed its message twice
  cerr << "value of a:" << a.getval() << endl;
  cerr << "value of b:" << b.getval() << endl << endl;

  // Another reference to c
  Reference::To<testmore> sr (*dr);
  // since dr and sr are references to c, c should be completely disconnected
  // by the following line
  cerr << "Disconnecting by Reference::To<testmore>" << endl;
  signal.disconnect (sr, &testmore::echo);

  cerr << "Sending callback 4" << endl << endl;
  signal (4);

  // reconnect c
  signal.connect (c, &testmore::echo);
  // c should not get disconnected by deletion of any references
  delete dr;

  cerr << "Sending callback 5" << endl;
  signal (5);
  cerr << "value of a:" << a.getval() << endl;
  cerr << "value of b:" << b.getval() << endl << endl;

  // implicitly disconnect c
  delete c;
  // explicitly disconnect a
  signal.disconnect (&a, &tester::setval);

  // the reference to c now should be invalidated
  if (!sr)
    cerr << "Reference::To<testmore> invalidated.\n" << endl;
  else
    cerr << "ERROR: Reference::To<testmore> not invalidated on destruction.\n"
	 << endl;

  cerr << "Sending callback 800" << endl;

  try {
    signal (800);
  }
  catch (Error& error) {
    cerr << "Caught expected exception" << endl;
  }

  cerr << "value of a:" << a.getval() << endl;
  cerr << "value of b:" << b.getval() << endl;

  cerr << endl << "Success." << endl;
  return 0;
}
