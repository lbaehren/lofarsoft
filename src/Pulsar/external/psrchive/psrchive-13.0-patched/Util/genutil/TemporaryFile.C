/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TemporaryFile.h"
#include "Error.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <algorithm>

using namespace std;

#ifdef HAVE_PTHREAD
#include <pthread.h>
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

/*
  The order in which static variable constructors are called is
  indeterminate.  However, pointers can be initialized to zero by the
  compiler.  This function hides a static pointer, intializing it on
  the first call.
*/

typedef std::set<TemporaryFile*> Instances;

static Instances* get_instances()
{
  static Instances* instances = 0;
  
  if (!instances)
    instances = new Instances;

  return instances;
}

//! Set true when the signal handler has been installed
bool TemporaryFile::signal_handler_installed = false;

//! Abort after processing any signals
bool TemporaryFile::abort = true;

void TemporaryFile::install_signal_handler ()
{
  struct sigaction sa;

  sa.sa_handler = TemporaryFile::signal_handler;

  // allow interrupted system calls to continue
  sa.sa_flags = SA_RESTART;

  // block all signals during execution of the signal handler
  sigfillset (&(sa.sa_mask));

  // "man 7 signal" to see which signals are being handled
  for (unsigned i=1; i<16; i++)
  {
    // cannot catch SIGKILL
    if (i != SIGKILL)
      sigaction (i, &sa, NULL);
  }

  signal_handler_installed = true;
}

void remove_instance (TemporaryFile* file)
{
  file->remove ();
}

//! The signal handler ensures that all temporary files are removed
void TemporaryFile::signal_handler (int sig)
{
#ifdef _DEBUG
  cerr << "TemporaryFile::signal_handler received " << strsignal(sig) << endl;
#else
  cerr << strsignal(sig) << endl;
#endif

  for_each (get_instances()->begin(), get_instances()->end(), remove_instance);

  if (abort)
    exit (-1);
}

//! Construct with regular expression
TemporaryFile::TemporaryFile (const string& basename)
{
  // open the temporary file
  filename = basename + ".XXXXXXXX";

  fd = mkstemp (const_cast<char*> (filename.c_str()));
  if (fd < 0)
    throw Error (FailedSys, "TemporaryFile",
                 "mkstemp(" + filename + ")");

#if 0
  // close and re-open just in case mkstemp returns a file descriptor that
  // is incapable of going past 2GB in size

  if (::close (fd) < 0)
    throw Error (FailedSys, "TemporaryFile",
                 "close (%d)", fd);

  fd = ::open (filename.c_str(), O_RDWR);
  if (fd < 0)
    throw Error (FailedSys, "TemporaryFile",
                 "open (" + filename + ")");
#endif

  unlinked = false;

#ifdef HAVE_PTHREAD
  pthread_mutex_lock (&mutex);
#endif

  if (!signal_handler_installed)
    install_signal_handler ();

  get_instances()->insert (this);

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&mutex);
#endif
}

//! Destructor
TemporaryFile::~TemporaryFile ()
{
  remove ();

#ifdef HAVE_PTHREAD
  pthread_mutex_lock (&mutex);
#endif

  get_instances()->erase (this);

#ifdef HAVE_PTHREAD
  pthread_mutex_unlock (&mutex);
#endif
}

void TemporaryFile::close ()
{
  if (fd < 0)
    return;

  if (::close (fd) < 0)
    throw Error (FailedSys, "TemporaryFile",
		 "failed close (%d)", fd);

  fd = -1;
}

void TemporaryFile::remove ()
{
  close ();
  unlink ();
}

void TemporaryFile::unlink ()
{
  if (unlinked)
    return;

  if (::unlink (filename.c_str()) < 0)
    throw Error (FailedSys, "TemporaryFile",
		 "failed unlink(" + filename + ")");

  unlinked = true;
}


