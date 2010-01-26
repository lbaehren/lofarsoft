//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/ReferenceTo.h,v $
   $Revision: 1.10 $
   $Date: 2008/07/07 00:27:06 $
   $Author: straten $ */

#ifndef __ReferenceTo_h
#define __ReferenceTo_h

#include "ReferenceAble.h"
#include <typeinfo>
#include <string>

#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

namespace Reference {

  //! A smart pointer that gets invalidated when its target is destroyed
  template<class Type, bool active = true> class To {

  public:

    //! Returns true if reference is null
    bool operator ! () const;
    
    //! Returns true if reference is other than null
    operator bool () const;
    
    //! Default constructor
    To (Type* ptr = 0);
    
    //! Copy constructor
    To (const To&);
    
    //! Destructor
    ~To ();
    
    //! Assignment operator
    To& operator = (const To&);

    //! Assignment operator
    To& operator = (Type *);
    
    //! Object dereferencing operator
    Type& operator * () const { return *get(); }
    
    //! Member dereferencing operator
    Type* operator -> () const { return get(); }
    
    //! Cast to Type* operator
    operator Type* () const { return get(); }

    //! Return the pointer
    Type* get () const;
    
    //! Return the pointer and unhook without deleting the object
    Type* release ();
    
    //! Return pointer without testing for validity
    const Type* ptr () const;

    //! Return pointer without testing for validity
    Type* ptr ();

    //! Return the name of the object, as returned by typeid
    std::string name () const;

  private:

    //! Set to an existing instance
    void hook (const Able*);

    //! Unhandle
    void unhook (bool auto_delete = true);

    //! The handle to the object
    Able::Handle* the_handle;
    
  };
}

template<class Type, bool active>
void Reference::To<Type,active>::unhook (bool auto_delete)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::unhook handle=" << the_handle << endl;
#endif

  if (!the_handle)
    return;

  Able::Handle* temp = the_handle;

  the_handle = 0;

  // thread-safe handle detachment
  temp->decrement (active, auto_delete);
}

template<class Type, bool active>
void Reference::To<Type,active>::hook (const Able* pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::handle Able*="<< pointer << endl;
#endif

  if (!pointer)
    the_handle = 0;

  else
    the_handle = pointer->__reference (active);
}

template<class Type, bool active>
std::string Reference::To<Type,active>::name () const
{
#ifdef _DEBUG
  return "T";
#else
  return typeid(Type).name();
#endif
}

template<class Type, bool active>
Reference::To<Type,active>::To (Type* ref_pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::To (Type*="<< ref_pointer <<")"<< endl;
#endif

  hook (ref_pointer);
}

template<class Type, bool active>
Reference::To<Type,active>::~To ()
{ 
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::~To" << endl;
#endif

  unhook ();
}

template<class Type, bool active>
bool Reference::To<Type,active>::operator ! () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator !" << endl;
#endif

  return !the_handle || the_handle->pointer == 0;
}

template<class Type, bool active>
Reference::To<Type,active>::operator bool () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator bool" << endl;
#endif

  return the_handle && the_handle->pointer;
}

template<class Type, bool active>
Reference::To<Type,active>::To (const To& another_reference)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::To (To<Type>)" << endl;
#endif

  // thread-safe copy
  Able::Handle::copy (the_handle, another_reference.the_handle, active);

}


// operator =
template<class Type, bool active>
Reference::To<Type,active>&
Reference::To<Type,active>::operator = (const To& oref)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator = (To<Type>)" << endl;
#endif

  if (the_handle == oref.the_handle)
    return *this;

  unhook ();

  // thread-safe copy
  Able::Handle::copy (the_handle, oref.the_handle, active);

  return *this;
}


template<class Type, bool active>
Reference::To<Type,active>& 
Reference::To<Type,active>::operator = (Type* ref_pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator = (Type*=" << ref_pointer <<")"<<endl;
#endif

  if (the_handle && the_handle->pointer == ref_pointer)
    return *this;

  unhook ();

  hook (ref_pointer);

  return *this;
}

template<class Type, bool active>
Type* Reference::To<Type,active>::get () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::get" << endl;
#endif

  if (!the_handle || the_handle->pointer == 0)
    throw Error (InvalidPointer, "Reference::To<"+name()+">::get");

  return reinterpret_cast<Type*>( the_handle->pointer );
}


template<class Type, bool active>
Type* Reference::To<Type,active>::release ()
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::release" << endl;
#endif

  if (!the_handle || the_handle->pointer == 0)
    throw Error (InvalidPointer, "Reference::To<"+name()+">::release");

  Type* copy = reinterpret_cast<Type*>( the_handle->pointer );

  unhook (false);

  return copy;
}

    //! Return pointer without testing for validity
template<class Type, bool active>
const Type* Reference::To<Type,active>::ptr () const
{
  if (the_handle)
    return reinterpret_cast<const Type*>( the_handle->pointer );
  else
    return 0;
}

    //! Return pointer without testing for validity
template<class Type, bool active>
Type* Reference::To<Type,active>::ptr ()
{
  if (the_handle)
    return reinterpret_cast<Type*>( the_handle->pointer );
  else
    return 0;
}

template<class Type, bool active>
void swap (Reference::To<Type,active>& ref1, Reference::To<Type,active>& ref2)
{
#ifdef _DEBUG
  cerr << "swap (Reference::To<Type>, ditto)" << endl;
#endif

  Type* ref1_ptr = ref1.release();
  Type* ref2_ptr = ref2.release();

  ref1 = ref2_ptr;
  ref2 = ref1_ptr;
}

//! return true if two Reference::To objects refer to the same instance
template<class Type1, bool active1, class Type2, bool active2>
bool operator == (const Reference::To<Type1,active1>& ref1,
                  const Reference::To<Type2,active2>& ref2)
{
#ifdef _DEBUG
  cerr << "operator == (Reference::To<Type>&, Reference::To<Type2>&)" << endl;
#endif

  return ref1.ptr() == ref2.ptr();
}

//! return false if two Reference::To objects refer to the same instance
template<class Type1, bool active1, class Type2, bool active2>
bool operator != (const Reference::To<Type1,active1>& ref1,
                  const Reference::To<Type2,active2>& ref2)
{
#ifdef _DEBUG
  cerr << "operator != (Reference::To<Type>&, Reference::To<Type2>&)" << endl;
#endif

  return ref1.ptr() != ref2.ptr();
}


//! return true if Reference::To refers to instance
template<class Type, bool active, class Type2>
bool operator == (const Reference::To<Type,active>& ref, const Type2* instance)
{
#ifdef _DEBUG
  cerr << "operator == (Reference::To<Type>&, Type*)" << endl;
#endif

  return ref.ptr() == instance;
}

//! return true if Reference::To refers to instance
template<class Type, bool active, class Type2>
bool operator == (const Type2* instance, const Reference::To<Type,active>& ref)
{
#ifdef _DEBUG
  cerr << "operator == (T2*, Reference::To<T1>&)" << endl;
#endif

  return ref.ptr() == instance;
}

template<typename C, typename P, bool A>
C* dynamic_kast (Reference::To<P,A>& p)
{
  return dynamic_cast<C*> (p.ptr());
}

template<typename P, bool A>
P* const_kast (Reference::To<const P,A>& p)
{
  return const_cast<P*> (p.ptr());
}

#endif // #ifndef __ReferenceTo_h

