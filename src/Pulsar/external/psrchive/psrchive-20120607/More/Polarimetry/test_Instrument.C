/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Instrument.h"
#include "MEAL/Polynomial.h"

using namespace std;
using namespace MEAL;
using namespace Calibration;

void banner (const char* text)
{
  cerr << endl
       << "******************************************************************"
       << endl << endl
       << text 
       << endl << endl
       << "******************************************************************"
       << endl << endl;
}

int main () try {

  Function::verbose = true;
  Function::very_verbose = true;

  banner ("constructing instrument");

  Instrument instrument;

  unsigned nparam = instrument.get_nparam();

  Instrument* clone = instrument.clone();
  clone->set_cyclic();

  for (unsigned iparam=0; iparam < nparam; iparam++)
    if (instrument.get_param_name(iparam) != clone->get_param_name(iparam))
    {
      cerr << "invalid param_name[" << iparam << "] of cyclic clone: ";
      cerr << instrument.get_param_name(iparam) 
	   << " != '" << clone->get_param_name(iparam) << "'" << endl;

      cerr << endl << "TEST FAILED" << endl;
      return -1;
    }
  
  banner ("set gain to polynomial");

  Polynomial* polynomial = new Polynomial(3);

  instrument.set_gain( polynomial );

  if (instrument.get_nparam() != nparam + 3) {
    cerr << "Chain Rule error map" << endl;
    return -1;
  }

  banner ("resize polynomial");

  polynomial->resize(6);

  if (instrument.get_nparam() != nparam + 6) {
    cerr << "Chain Rule error map" << endl;
    return -1;
  }

  Polynomial* zero = 0;

  banner ("set gain to zero");

  instrument.set_gain( zero );

  if (instrument.get_nparam() != nparam) {
    cerr << "Chain Rule error unmap" << endl;
    return -1;
  }

  banner ("nested Composite map/unmap test passed");

  for (unsigned iparam=0; iparam < nparam; iparam++)
    cerr << instrument.get_param_name(iparam) << endl;

  return 0;
}
 catch (Error& error) {
   cerr << error << endl;
   return -1;
 }
