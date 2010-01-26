//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Warning.h,v $
   $Revision: 1.1 $
   $Date: 2006/10/23 15:23:26 $
   $Author: straten $ */

#ifndef __Warning_h
#define __Warning_h

#include <map>
#include <string>
#include <sstream>
#include <iostream>

typedef std::ostream& (*manipulator) ( std::ostream& os );

//! Handles warning messages in a standardized fashion
/*!
  Warning messages can be annoying when they are repeated too many times.
  This class provides a standard means of handling warning messages.  The
  user can select to:
  <UL>
  <LI> issue a warning only once (the first time it occurs);
  <LI> print a summary of the number of times the warning was issued; and/or
  <LI> issue the warning every time
  </UL>
*/
class Warning {

  public:

  //! Print debugging messages
  static bool verbose;

  //! Construct with a specified output stream
  Warning (std::ostream& output = std::cerr);

  //! Destructor prints summary, if requested
  virtual ~Warning ();

  //! The prefix printed before each warning message
  std::string prefix;

  //! Print first message
  bool first;

  //! Print every message
  bool all;

  //! Print a summary of the number of times a message was repeated
  bool summary;

  //! Print a variable to the output stream
  template<class T>
  friend Warning& operator<< (Warning& warning, const T& t)
  {
    warning.buffer << t;
    warning.message += "*";
    return warning;
  }

  //! Print text to the output stream
  friend Warning& operator<< (Warning& warning, const char* t);

  //! Print text to the output stream
  friend Warning& operator<< (Warning& warning, manipulator m);

protected:

  //! Warning messages printed so far
  std::map<std::string, unsigned> messages;

  //! The output stream to which messages are sent
  std::ostream& output;

  //! The buffer used to store the message until a manipulator is inserted
  std::ostringstream buffer;

  //! The text of the message, without variables
  std::string message;

  //! Does the work for the insertion operator
  void insertion (manipulator m);

};


#endif
