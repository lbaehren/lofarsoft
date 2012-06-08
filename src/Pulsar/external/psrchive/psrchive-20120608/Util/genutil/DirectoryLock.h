//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* Util/genutil/DirectoryLock.h */

#ifndef __DirectoryLock_h
#define __DirectoryLock_h

#include "ThreadContext.h"
#include <string>

//! Working directory in which only one process may operate at one time
class DirectoryLock
{

 public:

  //! Default constructor
  DirectoryLock (const char* path = 0);

  // set the directory in which system calls will be made
  void set_directory (const std::string&);
  // get the directory in which system calls will be made
  std::string get_directory () const;

  // get the name of the file used to lock the directory
  std::string get_lockfile () const;

  // lock the working directory
  void lock ();
  // unlock the working directory
  void unlock ();
  // clean the working directory
  void clean ();

  //! Implements RAII for DirectoryLock and current working directory
  class Push;

protected:

  std::string path;
  std::string lockfile;

private:

  int lock_fd;
  bool have_lock;
  ThreadContext* context;

  void open_lockfile ();

};

/*! 
  On construction, this class
  1) saves the name of the current working directory
  2) locks the target working directory; and
  3) changes the current working directory to the target working directory

  On destruction, this class
  A) unlocks the target working directory; and
  B) changes the current working directory to the saved working directory
*/

class DirectoryLock::Push
{
  std::string current;
  DirectoryLock& lock;

  // disable copy constructor and assignment operator
  Push (const Push&);
  const Push& operator= (const Push&);

public:

  //! Constructor locks the target working directory and changes to it
  Push (DirectoryLock& lock);

  //! Unlocks the target working directory and restores 
  ~Push ();
  
};

#endif

