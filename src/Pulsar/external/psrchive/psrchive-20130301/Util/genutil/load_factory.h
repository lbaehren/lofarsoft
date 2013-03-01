/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Reference.h"
#include "Error.h"
#include "FilePtr.h"

#include "strutil.h"
#include "dirutil.h"

#include <vector>

//! Factory returns a new instance of a Parent-derived class
/*! Parent must define a static member function

    static void children (std::vector<Reference::To<Parent>&);

    that returns a vector of newly constructed children.
    Parent must also declare a pure virtual member function

    virtual void load (FILE*) = 0;

    The load method of each Parent-derived class will be attempted,
    and any Error exceptions that are thrown will be caught and
    properly handled.
*/

template<class Parent>
Parent* factory (FILE* fptr)
{
  long current = ftell (fptr);

  std::vector< Reference::To<Parent> > candidates;

  Parent::children (candidates);

  for (unsigned i=0; i < candidates.size(); i++) {

    if (fseek (fptr, current, SEEK_SET) < 0)
      throw Error (FailedSys, "factory (FILE*)", "fseek");

    try {
      candidates[i]->load (fptr);
      return candidates[i].release();
    }
    catch (Error& error) { }

  }
  throw Error (InvalidParam, "factory (FILE*)",
	       "no child recognizes contents of file");
}

template<class Parent>
Parent* factory (FILE* fptr, size_t nbytes)
{
  FilePtr temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "factory (FILE*, size_t)", "tmpfile");

  ::copy (fptr, temp, nbytes);

  rewind (temp);
  return factory<Parent> (temp);
}

template<class Parent>
Parent* factory (const std::string& filename)
{
  std::string use_filename = expand (filename);

  FilePtr temp = fopen (use_filename.c_str(), "r");
  if (!temp)
    throw Error (FailedSys, "factory (std::string&)",
		 "fopen (%s)", use_filename.c_str());

  return factory<Parent> (temp);
}

//
// the following functions are not really factories, but were inspired
// by the ideas implemented above
//
template<class Any>
Any* load (const std::string& filename)
{
  std::string use_filename = expand (filename);

  FilePtr temp = fopen (use_filename.c_str(), "r");
  if (!temp)
    throw Error (FailedSys, "Any* load (std::string&)",
		 "fopen (%s)", use_filename.c_str());

  Any* any = new Any;
  any -> load (temp);
  return any;
}

template<class Any>
size_t nbytes (const Any* any)
{
  FilePtr temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "nbytes (Any*)", "tmpfile");

  any->unload (temp);
  return ftell (temp);
}
