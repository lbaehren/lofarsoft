//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/ThreadContext.h,v $
   $Revision: 1.3 $
   $Date: 2010/05/20 19:49:58 $
   $Author: demorest $ */

#ifndef __ThreadContext_h
#define __ThreadContext_h

//! ThreadContext class
class ThreadContext {

public:

  //! Default constructor
  ThreadContext ();

  //! Destructor
  ~ThreadContext ();

  //! Obtain the mutual exclusion lock
  void lock ();

  //! Release the mutual exclusion lock
  void unlock ();

  //! Wait for a condition
  void wait ();

  //! Signal a condition to a single waiting thread
  void signal ();

  //! Signal a condition to all waiting threads
  void broadcast ();

  //! A simple mechanism for locking and automatically unlocking
  class Lock;

protected:

  void* cond;
  void* mutex;

};

//! Locks the mutex on construction and unlocks on deletion
/*!
  This class implements the "resource acquisition is initialization"
  paradigm (see http://www.research.att.com/~bs/glossary.html) of
  exception safe mutex locking and unlocking.
*/
class ThreadContext::Lock {

public:

  //! The constructor obtains the mutual exclusion lock ...
  Lock (ThreadContext* t) { if (t) t->lock(); context = t; }

  //! And the destructor releases it
  ~Lock () { if (context) context->unlock(); }

protected:

  //! The context in which the lock is held
  ThreadContext* context;

};

#endif // !defined(__ThreadContext_h)
