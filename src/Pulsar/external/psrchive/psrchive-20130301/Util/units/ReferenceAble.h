//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/ReferenceAble.h,v $
   $Revision: 1.13 $
   $Date: 2008/02/24 22:05:30 $
   $Author: straten $ */

#ifndef __ReferenceAble_h
#define __ReferenceAble_h

#include <vector>
#include <string>

#include "HeapTracked.h"

namespace Reference {

  //! Method verbosity flag
  extern bool verbose;

  //! Manages Reference::To references to the instance
  /*! Combined with the Reference::To template class, this class may
    be used to eliminate both memory leaks and dangling references.
    Simply inherit this class as follows:
 
    class myKlass : public Reference::Able  {
    ...
    };

    myKlass can now be managed using a Reference::To<myKlass>.
  */
  class Able : public HeapTracked {

    template<class Type, bool active> friend class To;

  public:

    //! Default constructor
    Able ();

    //! Copy constructor
    /*! Disables the compiler-generated copy of __reference_list. */
    Able (const Able&);

    //! Assignment operator
    /*! Disables the compiler-generated copy of __reference_list. */
    Able& operator = (const Able&) { return *this; }

    //! Destructor
    /*! Invalidates all Reference::To references to this instance. */
    virtual ~Able();
    
    //! Returns the number of references there are to this
    unsigned get_reference_count() const { return __reference_count; }

    //! Returns the current number instances in existence
    static size_t get_instance_count ();

  protected:

    //! Shared handle an instance
    class Handle;

    //! Add the address of a reference to this object
    Handle* __reference (bool active) const;

    //! Remove the address of a reference to this object
    void __dereference (bool auto_delete = true) const;

  private:

    //! Pointer to the shared handle to this instance
    mutable Handle* __reference_handle;

    //! Count of active references to this instance
    mutable unsigned __reference_count;

  };

  /*! Reference::To<> instances share this handle to an Able instance */
  class Able::Handle {
  public:

    //! Default constructor
    Handle ();

    //! Copy constructor
    Handle (const Handle&);

    //! Assignment operator
    Handle& operator = (const Handle&);

    //! Destructor
    ~Handle();

    //! Pointer to Able instance
    Able* pointer;

    //! Count of all references to this handle
    unsigned count;

    //! Thread-safe decrement and delete
    void decrement (bool active, bool auto_delete);

    //! Thread-safe copy and increment
    static void copy (Handle* &to, Handle* const &from, bool active);

  };

}

#endif // #ifndef __ReferenceAble_h



