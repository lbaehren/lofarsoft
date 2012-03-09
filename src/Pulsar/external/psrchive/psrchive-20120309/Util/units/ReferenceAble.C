/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ReferenceAble.h"
#include "Error.h"

#include <assert.h>

#ifdef HAVE_PTHREAD

#include <pthread.h>

/*
  Returns a new mutex that may be used recursively

  The mutex must be recursive because it is locked by both the
  Reference::Able::Handle::copy and Reference::Able::__reference methods,
  and the former calls the latter.
*/
static pthread_mutex_t* recursive_mutex ()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);
  pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);

  pthread_mutex_t* mutex = new pthread_mutex_t;
  pthread_mutex_init (mutex, &attr);
  
  return mutex;
}

/*
  Interface to the recursive mutex use to protect mutable attributes

  The order in which constructor functions are called at runtime cannot be
  guaranteed, but built-in types like pointers are initialized by the
  compiler.  The static pthread_mutex_t pointer is will be zero only on the
  first call to this method.
*/
static pthread_mutex_t* get_mutex ()
{
  static pthread_mutex_t* mutex = 0;
  if (!mutex)
    mutex = recursive_mutex ();
  return mutex;
}

#define LOCK_REFERENCE    pthread_mutex_lock (get_mutex());
#define UNLOCK_REFERENCE  pthread_mutex_unlock (get_mutex());

#else

#define LOCK_REFERENCE
#define UNLOCK_REFERENCE

#endif

// count of Reference::Able instances
static size_t instance_count = 0;

size_t Reference::Able::get_instance_count ()
{
  return instance_count;
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able ()
{
  instance_count++;
  __reference_count = 0;
  __reference_handle = 0;

#ifdef _DEBUG
  cerr << "Reference::Able ctor this=" << this
       << " instances=" << instance_count << endl;
#endif
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able (const Able&)
{
  instance_count++;
  __reference_count = 0;
  __reference_handle = 0;

#ifdef _DEBUG
  cerr << "Reference::Able copy ctor this=" << this
       << " instances=" << instance_count << endl;
#endif
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::~Able ()
{ 
  instance_count--;

  if (__reference_handle)
    __reference_handle->pointer = 0;

#ifdef _DEBUG
  cerr << "Reference::Able::~Able instances=" << instance_count << endl;
#endif
}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const T> */
Reference::Able::Handle*
Reference::Able::__reference (bool active) const
{
#ifdef _DEBUG
  cerr << "Reference::Able::__reference this=" << this 
       << " active=" << active << endl;
#endif

  LOCK_REFERENCE

  if (!__reference_handle)
  {
    /*
      optimization: calling __is_on_heap when first referenced reduces the
      size of the pending heap address list.
    */
    __is_on_heap();

    __reference_handle = new Handle;
    __reference_handle->pointer = const_cast<Able*>(this);

#ifdef _DEBUG
    cerr << "Reference::Able::__reference this=" << this 
         << " new handle=" << __reference_handle << endl;
#endif
  }

  __reference_handle->count ++;

  if (active)
    __reference_count ++;

  UNLOCK_REFERENCE

#ifdef _DEBUG
  cerr << "Reference::Able::__reference this=" << this 
       << " count=" << __reference_count << endl;
#endif

  assert (__reference_handle);

  return __reference_handle;
}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const Klass> */
void Reference::Able::__dereference (bool auto_delete) const
{ 
  assert (__reference_count > 0);

  __reference_count --;

#ifdef _DEBUG
  cerr << "Reference::Able::__dereference this=" << this
       << " count=" << __reference_count << endl;
#endif

  // delete when reference count reaches zero and instance is on heap
  if ( auto_delete && __reference_count == 0 && __is_on_heap() ) {

#ifdef _DEBUG
    cerr << "Reference::Able::__dereference delete this=" << this << endl;
#endif

    assert (__heap_state != 0x02);

    __heap_state = 0x02;

    if (__reference_handle)
      __reference_handle->pointer = 0;

    delete this;
    return;
  }

#ifdef _DEBUG
    cerr << "Reference::Able::__dereference exit this=" << this << endl;
#endif

}

void Reference::Able::Handle::decrement (bool active, bool auto_delete)
{
  LOCK_REFERENCE

  if (pointer)
  {
    if (count == 1)
      // this instance is about to be deleted, ensure that Able knows it
      pointer->__reference_handle = 0;

    if (active)
      // decrease the active reference count
      pointer->__dereference (auto_delete);
  }

  // decrease the total reference count
  count --;

#ifdef _DEBUG
  cerr << "Reference::Able::Handle::decrement count=" << count << endl;
#endif

  // delete the handle
  if (count == 0)
  {
    UNLOCK_REFERENCE

#ifdef _DEBUG
    cerr << "Reference::Able::Handle::decrement delete this=" << this << endl;
#endif

    delete this;
    return;
  }

  UNLOCK_REFERENCE
}


void
Reference::Able::Handle::copy (Handle* &to, Handle* const &from, bool active)
{
  LOCK_REFERENCE

#ifdef _DEBUG
  cerr << "Reference::Able::Handle::copy from=" << from << endl;
#endif

  if (!from)
  {
    to = 0;
    UNLOCK_REFERENCE
    return;
  }

  assert (from->count > 0);

  to = const_cast<Handle*>( from );

  if (to->pointer)
    to->pointer->__reference (active);

#ifdef _DEBUG
  cerr << "Reference::Able::Handle::copy count=" << to->count << endl;
#endif

  UNLOCK_REFERENCE
}

//! Default constructor
Reference::Able::Handle::Handle ()
{
  pointer = 0;
  count = 0;
}

//! Copy constructor
Reference::Able::Handle::Handle (const Handle&)
{
  throw Error (InvalidState, "Reference::Able::Handle copy ctor",
	       "unexpected copy of Reference::Able::Handle");
}

//! Assignment operator
Reference::Able::Handle& Reference::Able::Handle::operator = (const Handle&)
{
  throw Error (InvalidState, "Reference::Able::Handle::operator =",
	       "unexpected copy of Reference::Able::Handle");
}

//! Destructor
Reference::Able::Handle::~Handle()
{
}

