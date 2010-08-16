//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/tempo++.h,v $
   $Revision: 1.29 $
   $Date: 2009/03/15 15:12:40 $
   $Author: straten $ */

#ifndef __TEMPO_PP_H
#define __TEMPO_PP_H

#include "Pulsar/Parameters.h"

#include "polyco.h"
#include "toa.h"
#include "Observatory.h"

#include <vector>

//! Interface to Tempo
namespace Tempo
{
  //! Return the Observatory data given the telescope name
  const Observatory* observatory (const std::string& telescope_name);

  //! Convert a telescope name to a single-character tempo code
  char code (const std::string& telescope_name);

  // get the tempo version
  float  get_version ();

  // get the specified tempo configuration parameter
  std::string get_configuration (const std::string& parameter);

  // set the system call used to run tempo
  void   set_system (const std::string& system_call);
  // get the system call used to run tempo
  std::string get_system ();

  // set the directory in which tempo system calls will be made
  void   set_directory (const std::string& directory);
  // get the directory in which tempo system calls will be made
  std::string get_directory ();

  // get the name of the file used to lock the tempo directory
  std::string get_lockfile ();

  // lock the tempo working directory
  void lock ();
  // unlock the tempo working directory
  void unlock ();
  // clean the tempo working directory
  void clean ();

  // convenience overloads
  void   set_system (const std::string& system_call);
  void   set_directory (const std::string& directory);

  // verbosity flag of functions working in the Tempo namespace
  extern bool verbose;
  extern bool debug;

  // extension added to temporary Model filenames when working
  extern std::string extension;

  // default value passed to predict functions
  extern MJD unspecified;

  // base directory in which tempo will work
  extern std::string tmpdir;

  // file to which tempo stdout output will be redirected
  extern std::string stdout_filename;

  // file to which tempo stderr output will be redirected
  extern std::string stderr_filename;

  // file to which tempo ephemeris files are written
  extern std::string ephem_filename;

  //! Run tempo using the given arguments and input
  /*! tempo is run with a working directory given by
    Tempo::get_directory, so input files should be written here and
    output files (created by tempo) can be found here. */
  void tempo (const std::string& arguments, const std::string& input);

  //! Run tempo to fit the model to the data
  void fit (const Pulsar::Parameters* model, 
	    std::vector<toa>& data,
	    Pulsar::Parameters* postfit = NULL,
	    bool track = false,
	    Tempo::toa::State min_state = Tempo::toa::Normal);

}

#endif

