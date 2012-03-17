/***************************************************************************
 *
 *   Copyright (C) 2007-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "T2Parameters.h"

using namespace std;

//! Default constructor
Tempo2::Parameters::Parameters ()
{
  if (verbose)
    cerr << "Tempo2::Parameters constructor" << endl;

  psr = 0;
}

//! Destructor
Tempo2::Parameters::~Parameters ()
{
  if (psr)
    destroyOne (psr);
}

//! Copy constructor
Tempo2::Parameters::Parameters (const Parameters&)
{
  throw Error (InvalidState, "Tempo2::Parameters",
	       "copy constructor not implemented");
}

Pulsar::Parameters* Tempo2::Parameters::clone () const
{
  return new Parameters (*this);
}

//! Return true if *this == *that
bool Tempo2::Parameters::equals (const Pulsar::Parameters* that) const
{
  // const Parameters* like = dynamic_cast<const Parameters*>( that );
  throw Error (InvalidState, "Tempo2::Parameters::equals", "not implemented");
}

//! Load from an open stream
void Tempo2::Parameters::load (FILE* fptr)
{
  if (verbose)
    cerr << "Tempo2::Parameters::load (FILE*)" << endl;

  if (!psr) {

    psr = new pulsar;

    int noWarnings = 1;
    int fullSetup = 0;

    initialiseOne (psr, noWarnings, fullSetup);

    if (verbose)
      cerr << "Tempo2::Parameters::load initialiseOne completed" << endl;

    if (setupParameterFileDefaults (psr) < 0)
      throw Error (InvalidState, "Tempo2::Parameters::load", TEMPO2_ERROR);

    if (verbose)
      cerr << "Tempo2::Parameters::load setupParameterFileDefaults completed"
	   << endl;

  }

  readSimpleParfile (fptr, psr);
  if (verbose)
    cerr << "Tempo2::Parameters::load readSimpleParfile completed" << endl;

#ifdef HAVE_TEMPO2_SIMPLE
  preProcessSimple (psr);
#else
  preProcess (psr, 1, 0, 0);
#endif

  if (verbose)
    cerr << "Tempo2::Parameters::load preProcess completed" << endl;

}

//! Unload to an open stream
void Tempo2::Parameters::unload (FILE*) const
{
  throw Error (InvalidState, "Tempo2::Parameters::unload", "not implemented");
}
