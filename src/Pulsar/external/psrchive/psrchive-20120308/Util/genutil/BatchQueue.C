/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BatchQueue.h"
#include "Error.h"

#include <errno.h>
#include <assert.h>

using namespace std;

BatchQueue::BatchQueue (unsigned nthread)
{
#if HAVE_PTHREAD
  context = new ThreadContext;
  resize (nthread);
#else
  context = 0;
  if (nthread != 1)
    throw Error (InvalidState, "BatchQueue constructor",
		 "threads are not availalbe and nthread=%u", nthread);
#endif
}

BatchQueue::~BatchQueue ()
{
  if (context)
    delete context;
}


#if HAVE_PTHREAD

void BatchQueue::resize (unsigned nthread)
{
  ThreadContext::Lock lock (context);

  unsigned jthread = 0;

  // move the currently active jobs to the front of the array
  for (unsigned ithread = 0; ithread < active.size(); ithread++)
    if ( active[ithread] != 0 ) {
      active[jthread] = active[ithread];
      jthread ++;
    }

  if ( nthread < jthread )
    throw Error (InvalidState, "BatchQueue::resize",
		 "cannot decrease nthreads to %u because there are %u active",
		 nthread, jthread);

  active.resize (nthread);

  for (unsigned ithread = jthread; ithread < nthread; ithread++)
    active[ithread] = 0;
}

static void* solve_thread (void* instance)
{
  BatchQueue::Job* thiz = static_cast<BatchQueue::Job*>(instance);

#ifdef _DEBUG
  cerr << "BatchQueue::solve_thread this=" << thiz << endl;
#endif

  thiz -> run ();

  // exit
  pthread_exit(NULL);
}

void BatchQueue::Job::run ()
{

  try {
    execute ();
  }
  catch (Error& e) {
    cerr << e.get_message() << endl;
  }

  queue->remove (this);

}

void BatchQueue::remove (Job* job)
{
  ThreadContext::Lock lock (context);

  unsigned ithread = 0;

  for (ithread = 0; ithread < active.size(); ithread++)
    if ( active[ithread] == job ) {
      delete job;
      active[ithread] = 0;
      break;
    }

  assert ( ithread < active.size() );

  // signal completion to waiting scheduler
  context->signal();
}

void BatchQueue::add (Job* job)
{
  ThreadContext::Lock lock (context);

  unsigned ithread = 0;

  while ( active.size() ) {

    bool currently_solving = false;

    for (ithread = 0; ithread < active.size(); ithread++)
      if ( active[ithread] == job )
	currently_solving = true;
    
    if (currently_solving)
      cerr << "BatchQueue::solve_wait job=" << job
	   << " is currently being solved" << endl;

    else {

      for (ithread = 0; ithread < active.size(); ithread++)
	if ( active[ithread] == 0 )
	  break;

      if ( ithread < active.size() )
	break;

    }

#ifdef _DEBUG
    cerr << "BatchQueue::solve waiting for next available thread" << endl;
#endif

    context->wait ();

  }

  if ( !active.size() )
    throw Error (InvalidState, "BatchQueue::solve",
		 "cannot solve with zero available threads");

  active[ithread] = job;
}

//! Wait for all of the solutions to return
void BatchQueue::wait ()
{
  ThreadContext::Lock lock (context);
  
  while ( active.size() ) {

    unsigned current = 0;
    
    for (unsigned ithread = 0; ithread < active.size(); ithread++)
      if ( active[ithread] )
	current ++;
    
    if (!current)
      break;

#ifdef _DEBUG
    cerr << "BatchQueue::wait waiting for " << current << " threads" << endl;
#endif

    context->wait ();

  }
}

void BatchQueue::submit (Job* job)
{
#ifdef _DEBUG
  cerr << "BatchQueue::solve using " << active.size() << " threads" << endl;
#endif

  if (active.size() == 0) {
#ifdef _DEBUG
    cerr << "BatchQueue::solve calling Job::execute" << endl;
#endif
    job->execute();
    return;
  }

  add (job);

  job->queue = this;

  pthread_attr_t pat;
  pthread_attr_init (&pat);
  pthread_attr_setdetachstate (&pat, PTHREAD_CREATE_DETACHED);
  pthread_t thread_ID;

#ifdef _DEBUG
  cerr << "BatchQueue::solve creating solve thread job=" << job << endl;
#endif

  errno = pthread_create (&thread_ID, &pat, solve_thread, job);

  if (errno != 0)
    throw Error (FailedSys, "BatchQueue::solve", "pthread_create");
}

#else // ! HAVE_PTHREAD

//! Set the number of instances that may be solved simultaneously
void BatchQueue::submit (Job* job)
{
  job->execute();
}

void BatchQueue::wait ()
{
}

void BatchQueue::resize (unsigned nthread)
{                 
  if (nthread > 0)
    throw Error (InvalidParam, "BatchQueue::resize", "threads unavailable");
}

#endif

