/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tempo++.h"

#ifdef HAVE_TEMPO2
#include "T2Observatory.h"
#endif

using namespace std;

const Pulsar::Site* 
Pulsar::Site::location (const string& antenna) try
{
#ifdef HAVE_TEMPO2
  try {
    return Tempo2::observatory (antenna);
  }
  catch (Error& error)
  {
    if (Predictor::verbose)
      cerr << error.get_message() << endl;
  }
#endif

  return Tempo::observatory (antenna);
}
catch (Error& error)
{
  throw error += "Pulsar::Site::location";
}
