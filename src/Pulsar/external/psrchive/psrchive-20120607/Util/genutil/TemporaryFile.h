//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/TemporaryFile.h,v $
   $Revision: 1.4 $
   $Date: 2008/07/14 00:01:38 $
   $Author: straten $ */

#ifndef __TemporaryFile_h
#define __TemporaryFile_h

#include "Reference.h"

#include <string>
#include <set>

//! Opens a temporary file with a unique name and deletes it when finished
class TemporaryFile : public Reference::Able
{

 public:

  //! Abort after processing any signals
  static bool abort;
  
  //! Construct a temporary file with the specified basename
  TemporaryFile (const std::string& basename);

  //! Close and remove the temporary file
  ~TemporaryFile ();

  //! Close the temporary file
  void close ();

  //! Unlink the temporary file
  void unlink ();

  //! Remove the temporary file (close and unlink)
  void remove ();

  //! Get the file descriptor of the temporary file
  int get_fd () const { return fd; }

  //! Get the name of the temporary file
  std::string get_filename () const { return filename; }

  //! Set to true if the temporary file need not be removed
  void set_removed (bool flag = true) { unlinked = flag; }

 private:

  //! The name of the temporary file
  std::string filename;

  //! The file descriptor of the temporary file
  int fd;

  //! Set true when the temporary file has been unlinked
  bool unlinked;

  //! Install the signal handler
  static void install_signal_handler ();

  //! Set true when the signal handler has been installed
  static bool signal_handler_installed;

  //! The signal handler ensures that all temporary files are removed
  static void signal_handler (int);

};

#endif

