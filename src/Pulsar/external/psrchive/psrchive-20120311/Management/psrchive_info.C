/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This simple program reports on the optional features that are built
  into the library.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Archive.h"
#include "FTransform.h"

using namespace std;

int main ()
{
  /***************************************************************************
   *
   *   File format 'plugins'
   *
   ***************************************************************************/

  Pulsar::Archive::agent_report ();


  /***************************************************************************
   *
   *   FFT libraries
   *
   ***************************************************************************/

  unsigned nlib = FTransform::get_num_libraries ();

  string lib = (nlib == 1) ? "library" : "libraries";

  cout << "\nFTransform::report " << nlib << " available FFT " << lib << ":";

  for (unsigned ilib=0; ilib < nlib; ilib++)
    cout << " " << FTransform::get_library_name (ilib);
	
  cout << "\nFTransform::default FFT library: " 
       << FTransform::get_library() << endl;


  /***************************************************************************
   *
   *   TEMPO2 support
   *
   ***************************************************************************/

  cout << "\nTempo2::Predictor support ";

#ifdef HAVE_TEMPO2
  cout << "enabled" << endl;
#else
  cout << "not available" << endl;
#endif

  cout << endl;

  return 0;
}
