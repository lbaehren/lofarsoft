/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ThreadContext.h"
#include "Error.h"
#include <errno.h>

ThreadContext::ThreadContext ()
{
#if HAVE_PTHREAD

  cond = new pthread_cond_t;
  pthread_cond_init (reinterpret_cast<pthread_cond_t*>(cond), NULL);

  mutex = new pthread_mutex_t;
  pthread_mutex_init (reinterpret_cast<pthread_mutex_t*>(mutex), NULL);

#else

  // pthreads aren't available; throw an exception
  throw Error (InvalidState, "ThreadContext ctor",
	       "pthread support is not available");

#endif
}

ThreadContext::~ThreadContext ()
{
}

void ThreadContext::lock ()
{
#if HAVE_PTHREAD
  errno = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(mutex));
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_lock");
#endif
}

void ThreadContext::unlock ()
{
#if HAVE_PTHREAD
  errno = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(mutex));
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_unlock");
#endif
}

void ThreadContext::wait ()
{
#if HAVE_PTHREAD
  errno = pthread_cond_wait(reinterpret_cast<pthread_cond_t*>(cond),
			    reinterpret_cast<pthread_mutex_t*>(mutex));
  if (errno != 0) {
    pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(mutex));
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_wait");
  }
#endif
}

void ThreadContext::signal ()
{
#if HAVE_PTHREAD
  errno = pthread_cond_signal (reinterpret_cast<pthread_cond_t*>(cond));
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_signal");
#endif
}

void ThreadContext::broadcast ()
{
#if HAVE_PTHREAD
  errno = pthread_cond_broadcast (reinterpret_cast<pthread_cond_t*>(cond));
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_broadcast");
#endif
}

