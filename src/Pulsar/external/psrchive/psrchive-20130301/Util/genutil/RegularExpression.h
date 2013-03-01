//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/RegularExpression.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __RegularExpression_h
#define __RegularExpression_h

#include "Error.h"

//! Interface to regex
class RegularExpression {

 public:

  //! Default constructor
  RegularExpression ();

  //! Construct with regular expression
  RegularExpression (const std::string& regexp);

  //! Destructor
  ~RegularExpression ();

  //! Set the regular expression
  void set_regexp (const std::string& regexp);

  //! Check if the supplied string matches the regular expression
  bool get_match (const std::string& test);

 protected:

  //! Return an instance of the Error class with regerror message
  Error error (int err, const std::string& func, const std::string& message);

 private:

  //! Pointer to the regular expression pattern buffer storage area
  void* preg;

  void destroy ();
};

#endif

