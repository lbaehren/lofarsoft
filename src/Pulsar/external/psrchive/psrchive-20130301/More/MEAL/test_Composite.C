/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Composite.h"
#include "MEAL/Rotation1.h"
#include "MEAL/Boost1.h"

using namespace std;

using namespace MEAL;

// Tests the mapping function of the Composite class
class CompositeTest: public Function
{

public:
  CompositeTest (Composite* c = 0);

  void runtest (CompositeTest& c1, CompositeTest& c2);

  void nested ();
  void shared ();

  void evaluate ();
  bool get_changed ();
  string get_name () const { return "CompositeTest"; }
  Composite* composite;

protected:
  Reference::To<Rotation1> rotation;
  Reference::To<Boost1> boost;

};

CompositeTest::CompositeTest (Composite* c)
{
  if (c)
    composite = c;
  else
    composite = new Composite (this);

  rotation = new Rotation1 (Vector<3,double>::basis(0));
  boost = new Boost1 (Vector<3,double>::basis(2));
}

void CompositeTest::evaluate ()
{
  cerr << "CompositeTest::evaluate" << endl;

  set_evaluation_changed (false);
  rotation->evaluate ();
  boost->evaluate ();
}

bool CompositeTest::get_changed ()
{
  cerr << "CompositeTest::get_changed ptr=" << this << endl;
  return get_evaluation_changed();
}

void CompositeTest::runtest (CompositeTest& m1, CompositeTest& m2)
{
  rotation->set_phi (0.0);

  Project<Rotation1> m1r (rotation);
  Project<Rotation1> m2r (rotation);

  Project<CompositeTest> m1m (&m2);

  Project<Boost1> m2b (boost);

  cerr << "********************* m1 map rotation" << endl;
  m1.composite->map (m1r);
  m1.rotation = rotation;

  if (!m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Composite not changed after mapping");

  m1.evaluate();

  if (m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Composite changed after evaluate");

  if (rotation->get_evaluation_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Component changed after evaluate");

  cerr << "********************* m2 map boost" << endl;
  m2.composite->map (m2b);
  m2.boost = boost;

  cerr << "********************* m2 map rotation" << endl;
  m2.composite->map (m2r);
  m2.rotation = rotation;

  if (!m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Parent Composite not changed after child Composite mapping");

  m1.evaluate();

  if (m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Composite changed after second evaluation");

  if (rotation->get_evaluation_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Component changed after second evaluation");

  cerr << "********************* setting component parameter" << endl;

  rotation->set_phi (4.5);

  if (!m1.get_changed())
    throw Error (InvalidState, "test_Composite", 
		 "Composite not changed after component parameter changed");

}

void CompositeTest::shared ()
{
  CompositeTest m1;

  cerr << "********************* m1 share m2" << endl;
  CompositeTest m2 (m1.composite);

  runtest (m1, m2);
}

void CompositeTest::nested ()
{
  CompositeTest m1;
  CompositeTest m2;

  cerr << "********************* m1 nest m2" << endl;
  Project<CompositeTest> m1m (&m2);
  m1.composite->map (m1m);

  runtest (m1, m2);
}

int main (int argc, char** argv) try
{
  Function::cache_results = true;
  Function::very_verbose = true;
  Function::verbose = true;

  CompositeTest test;

  test.nested ();
  test.shared ();

  cerr << 
    "\n"
    "\n"
    "Successful completion"
    "\n"
    "\n" << endl;

  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

