/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Projection.h"
#include "MEAL/Boost.h"

using namespace std;

using namespace MEAL;

int main (int argc, char** argv)
{ try {

  Error::verbose = true;
  MEAL::Function::verbose = true;

  cerr << "Instantiating Project<Boost>" << endl;
  Project<Boost> function;

  cerr << "Instantiating Boost" << endl;
  Boost boost;

  cerr << "Calling Project<Boost>::get_map" << endl;
  Projection* mapped = function.get_map ();
  function = &boost;

  if (mapped->get_Function() != &boost) {
    cerr << "Projecton::get_Function()=" << mapped->get_Function()
	 << " != &Boost=" << &boost << endl;
    return -1;
  }

} catch (Error& error) {
  cerr << error << endl;
  return -1;
}

  cerr << "Projection::get_Function passes test" << endl;
  return 0;
}
