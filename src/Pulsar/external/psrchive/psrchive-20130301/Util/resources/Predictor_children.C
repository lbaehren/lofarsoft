/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "polyco.h"

#ifdef HAVE_TEMPO2
#include "T2Predictor.h"
#endif

#include "Pulsar/SimplePredictor.h"

using namespace std;

void Pulsar::Predictor::children (vector< Reference::To<Predictor> >& child)
{
  child.resize (0);

  /*
    polyco should come first because the Tempo2::Predictor will
    successfully load a polyco, but the code differentiates between
    the two based on class type 
  */

  child.push_back (new Pulsar::SimplePredictor);

  child.push_back (new polyco);

#ifdef HAVE_TEMPO2
  child.push_back (new Tempo2::Predictor);
#endif
}
