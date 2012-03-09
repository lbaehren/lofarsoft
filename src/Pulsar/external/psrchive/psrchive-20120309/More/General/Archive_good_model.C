/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

#include <iostream>
using namespace std;

// ///////////////////////////////////////////////////////////////////////
//
// Archive::good_model
//
/*!  This method tests if the given phase predictor applies to each
  Integration, returning false if an exception is thrown.

  \param test_model the Predictor to be tested
*/
bool Pulsar::Archive::good_model (const Predictor* test_model) const
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::good_model testing polyco on " << get_nsubint()
	 << " integrations" << endl;

  unsigned isub=0;
  for (isub=0; isub < get_nsubint(); isub++)
    try {
      test_model->phase (get_Integration(isub)->get_epoch());
    }
    catch (...) {
      if (verbose == 3) cerr << "Pulsar::Archive::good_model"
                           " Predictor throws exception" << endl;
      break;
    }
  
  if (isub < get_nsubint()) {

    if (verbose == 3)
      cerr << "Pulsar::Archive::good_model polyco failed on integration "
	   << isub << endl;

    return false;

  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::good_model polyco passes test" << endl;

  return true;
}
