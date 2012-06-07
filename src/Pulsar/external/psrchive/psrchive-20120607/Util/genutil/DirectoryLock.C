/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "DirectoryLock.h"
#include "Error.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

//! Default constructor
DirectoryLock::DirectoryLock (const char* path)
{
  if (path)
    set_directory (path);

  lock_fd = -1;
  have_lock = false;

#if HAVE_PTHREAD
  context = new ThreadContext;
#else
  context = 0;
#endif
}

// set the directory in which system calls will be made
void DirectoryLock::set_directory (const std::string& dir)
{
  path = dir;
}

// get the directory in which system calls will be made
std::string DirectoryLock::get_directory () const
{
  return path;
}

// get the name of the file used to lock the directory
std::string DirectoryLock::get_lockfile () const
{
  return get_directory() + "/.lock";
}


// lock the working directory
void DirectoryLock::lock ()
{
  if (context)
    context->lock();

  if (have_lock)
    return;

  open_lockfile ();

  struct flock lock;
  lock.l_whence = SEEK_SET;
  lock.l_type   = F_WRLCK;
  lock.l_start  = 0;
  lock.l_len    = 0;

  if (fcntl (lock_fd, F_SETLKW, &lock) < 0)
    throw Error (FailedSys, "DirectoryLock::lock", 
		 "failed fcntl(" + get_lockfile() + ",F_SETLKW F_WRLCK)");

  have_lock = true;
}

// unlock the working directory
void DirectoryLock::unlock ()
{
  if (context)
    context->unlock();

  if (!have_lock)
    return;

  open_lockfile ();

  struct flock lock;
  lock.l_whence = SEEK_SET;
  lock.l_type   = F_UNLCK;
  lock.l_start  = 0;
  lock.l_len    = 0;

  if (fcntl (lock_fd, F_SETLK, &lock) < 0)
    throw Error (FailedSys, "DirectoryLock::unlock",
		 "failed fcntl(" + get_lockfile() + ",F_SETLK F_UNLCK)");
  
  have_lock = false;
}


void DirectoryLock::clean ()
{
  if (!have_lock)
    throw Error (InvalidState, "DirectoryLock::clean", "not locked");

  string clean_command = "rm -f " + get_directory() + "/*";
  system (clean_command.c_str());
}


void DirectoryLock::open_lockfile ()
{
  string filename = get_lockfile();
  const char* fname = filename.c_str();

  if (lock_fd < 0)
    lock_fd = open (fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  if (lock_fd < 0)
    throw Error (FailedSys, "DirectoryLock::open_lockfile",
		 "failed open(%s)", fname);
}

  //! Constructor locks the target working directory and changes to it
DirectoryLock::Push::Push (DirectoryLock& _lock)
  : lock(_lock)
{
  lock.lock();

  char cwd[MAXPATHLEN];

  if (getcwd (cwd, MAXPATHLEN) == NULL)
    throw Error (FailedSys, "DirectoryPush ctor", "failed getcwd");

  current = cwd;

  if (chdir (lock.get_directory().c_str()) != 0)
    throw Error (FailedSys, "DirectoryPush ctor",
		 "failed chdir(" + lock.get_directory() + ")");
}

//! Unlocks the target working directory and restores 
DirectoryLock::Push::~Push ()
{
  lock.unlock();

  if (chdir (current.c_str()) != 0)
    throw Error (FailedSys, "DirectoryPush ctor",
		 "failed chdir(" + current + ")");
}
