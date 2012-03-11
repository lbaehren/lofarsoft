//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/BatchQueue.h,v $
   $Revision: 1.4 $
   $Date: 2007/11/14 23:55:17 $
   $Author: straten $ */

#ifndef __BatchQueue_h
#define __BatchQueue_h

#include "ThreadContext.h"
#include "Functor.h"

#include <vector>

//! Runs multiple threads using a simple batch queue model
class BatchQueue {

public:

  //! Default constructor
  BatchQueue (unsigned nthread = 0);

  //! Destructor
  ~BatchQueue ();

  //! Set the number of tasks that may run at one time
  void resize (unsigned nthread);

  //! A single job in the queue
  class Job;

  //! Submit a job for processing
  void submit (Job*);

  //! Submit a job for processing
  template<class Class, typename Method>
  void submit (Class* instance, Method method);

  //! Submit a job for processing
  template<class Class, typename Method, typename Argument>
  void submit (Class* instance, Method method, Argument argument);

  //! Submit a job for processing
  template<class Class, typename Method, typename Arg1, typename Arg2>
  void submit (Class* instance, Method method, Arg1 a1, Arg2 a2);

  //! Wait for completion of all active jobs
  void wait ();

protected:

  //! Add a job to the active list
  void add (Job*);

  //! Remove a job from the active list
  void remove (Job*);

  //! Mutual exclusion and condition variables used to coordinate the queue
  ThreadContext* context;

  //! The active jobs
  std::vector<Job*> active;

};

class BatchQueue::Job {

public:

  virtual ~Job () {}

  //! Run the execute method and remove self from the active job list
  void run ();

  //! The job that is executed is defined by derived types
  virtual void execute () = 0;

protected:

  friend class BatchQueue;

  //! The batch queue in which this job is queued or running
  BatchQueue* queue;

};

template<class Class, typename Method>
void BatchQueue::submit (Class* instance, Method method)
{
  class Job0 : public Job {
    Functor< void() > functor;
    void execute () { functor (); }
  public:
    Job0 (Class* instance, Method method) : functor (instance, method) {}
  };
  
  submit (new Job0( instance, method ));
}

template<class Class, typename Method, typename Argument>
void BatchQueue::submit (Class* instance, Method method, Argument arg)
{
  class Job1 : public Job {
    Functor< void(Argument) > functor;
    Argument argument;
    void execute () { functor (argument); }
  public:
    Job1 (Class* instance, Method method, Argument arg)
      : functor (instance, method) { argument = arg; }
  };
  
  submit (new Job1( instance, method, arg ));
}

template<class Class, typename Method, typename Arg1, typename Arg2>
void BatchQueue::submit (Class* instance, Method method, Arg1 a1, Arg2 a2)
{
  class Job2 : public Job {
    Functor< void(Arg1, Arg2) > functor;
    Arg1 arg1;
    Arg2 arg2;
    void execute () { functor (arg1, arg2); }
  public:
    Job2 (Class* instance, Method method, Arg1 a1, Arg2 a2)
      : functor (instance, method) { arg1 = a1; arg2 = a2; }
  };
  
  submit (new Job2( instance, method, a1, a2 ));
}

#endif // !defined(__BatchQueue_h)
