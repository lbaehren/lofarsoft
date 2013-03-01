/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ProductRule.h"
#include "MEAL/Complex2Value.h"
#include "MEAL/Rotation1.h"
#include "MEAL/Boost1.h"

using namespace std;

// Template class provides public access to MEAL::get_evaluation_changed
template <class T>
class Test : public T
{
public:
  template<class U>
  Test (const U& constructor) : T(constructor) {}

  Test () {}

  bool get_changed() const { return this->get_evaluation_changed(); }
};

int main (int argc, char** argv)
{
  MEAL::Function::cache_results = true;
  MEAL::Function::verbose = true;

  Test< MEAL::ProductRule<MEAL::Complex2> > meta;

  Test< MEAL::Rotation1 > rotation (Vector<3,double>::basis(0));

  meta.add_model (&rotation);
  if (!meta.get_changed()) {
    cerr << "FAIL: ProductRule<Complex2>::get_changed=false"
       " after add_model" << endl;
    return -1;
  }

  Test< MEAL::Boost1 > boost (Vector<3,double>::basis(2));

  meta.add_model (&boost);

  cerr << "Evaluate meta" << endl;
  Jones<double> temp = meta.evaluate();

  if (temp != Jones<double>::identity())  {
    cerr << "FAIL: ProductRule<Complex2>::evaluate returns\n"
        "   " << temp << " != " << Jones<double>::identity() << endl;
    return 0;
  }

  double value = 1.23;

  cerr << "Set rotation" << endl;
  rotation.set_phi (value);
  if (!rotation.get_changed()) {
    cerr << "FAIL: Rotation::get_changed = false" << endl;
    return -1;
  }
  if (!meta.get_changed()) {
    cerr << "FAIL: ProductRule<Complex2>::get_changed = false" << endl;
    return -1;
  }

  cerr << "Set boost" << endl;
  boost.set_beta (value);
  if (!boost.get_changed()) {
    cerr << "FAIL: Boost::get_changed = false" << endl;
    return -1;
  }


  cerr << "Evaluate meta" << endl;
  temp = meta.evaluate();

  if (meta.get_changed()) {
    cerr << "FAIL: ProductRule<Complex2>::get_changed = true" << endl;
    return -1;
  }
  if (rotation.get_changed()) {
    cerr << "FAIL: Rotation::get_changed = true" << endl;
    return -1;
  }
  if (boost.get_changed()) {
    cerr << "FAIL: Boost::get_changed = true" << endl;
    return -1;
  }

  cerr << "Set rotation" << endl;
  rotation.set_phi (2.0*value);

  if (!rotation.get_changed()) {
    cerr << "FAIL: Rotation::get_changed = false" << endl;
    return -1;
  }
  if (!meta.get_changed()) {
    cerr << "FAIL: ProductRule<Complex2>::get_changed = false" << endl;
    return -1;
  }

  cerr << "Evaluate rotation" << endl;
  temp = rotation.evaluate();

  if (rotation.get_changed()) {
    cerr << "FAIL: Rotation::get_changed = true" << endl;
    return -1;
  }

  //
  // Here is the main trick: After rotation has been re-computed, its
  // get_changed method will return false.  However, OptimizedFunction
  // has been modified to emit the MEAL::changed signal, which is now
  // connected to Composite::set_changed (true).
  //

  if (!meta.get_changed()) {
    cerr << "FAIL: ProductRule<Complex2>::get_changed = false" << endl;
    return -1;
  }

  cerr << "Construct MEAL::Complex2Value" << endl;
  Test< MEAL::Complex2Value > Cvalue;

  meta *= &Cvalue;

  meta.evaluate ();

  Cvalue.set_value ( rotation.evaluate() );

  if (!meta.get_changed()) {
    cerr << "FAIL: ProductRule<Complex2>::get_changed = false" << endl;
    return -1;
  }

  cerr << "Successful completion" << endl;

  return 0;
}
