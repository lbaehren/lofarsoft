//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/ReferenceVector.h,v $
   $Revision: 1.1 $
   $Date: 2008/07/25 23:31:12 $
   $Author: straten $ */

#ifndef __ReferenceVector_h
#define __ReferenceVector_h

#include "ReferenceAble.h"
#include <typeinfo>
#include <string>

#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

namespace Reference {

  //! A smart pointer that gets invalidated when its target is destroyed
  template<class Type, bool active = true> class Vector
  {

  public:

    //! Default constructor
    Vector ()
    { }
    
    //! Copy constructor
    Vector (const Vector& v)
    { elements = v.elements; }
    
    //! Destructor
    ~Vector ()
    { }
    
    //! Assignment operator
    Vector& operator = (const Vector& v)
    { elements = v.elements; return *this; }

    //! element operator
    Reference::To<Type,active>& operator [] (unsigned index)
    { check (index); return elements[index]; }

    //! const element operator
    const Reference::To<Type,active>& operator [] (unsigned index) const
    { check (index); return elements[index]; }

    //! Resize
    void resize (unsigned n)
    { elements.resize (n); }

    //! Size
    unsigned size () const
    { return elements.size(); }

  private:

    //! The vector of pointers to objects
    std::vector< Reference::To<Type,active> > elements;

    //! Check that index is valid
    void check (unsigned index) const
    {
      if (index >= elements.size())
	throw Error (InvalidParam, "Reference::Vector::check",
		     "index=%u >= size=%u", index, elements.size());
    }

  };
}

#endif // #ifndef __ReferenceVector_h

