//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/tostring.h,v $
   $Revision: 1.20 $
   $Date: 2010/01/13 07:10:30 $
   $Author: straten $ */

#ifndef __TOSTRING_H
#define __TOSTRING_H

#include "Error.h"

#include <string>
#include <sstream>
#include <limits>

/* the following global variables are not nested-call or multi-thread
   safe and should be used only when it is extremely difficult to pass
   the relevant arguments directly to the tostring function */

extern unsigned tostring_precision;
extern std::ios_base::fmtflags tostring_setf; 
extern std::ios_base::fmtflags tostring_unsetf; 

#define FMTFLAGS_ZERO std::ios_base::fmtflags(0)

template<class T>
std::string tostring (const T& input,
		      unsigned precision = std::numeric_limits<T>::digits10,
		      std::ios_base::fmtflags set = FMTFLAGS_ZERO,
		      std::ios_base::fmtflags unset = FMTFLAGS_ZERO)
{
  std::ostringstream ost;

  if (tostring_setf)
    ost.setf (tostring_setf);
  else if (set)
    ost.setf (set);
  
  if (tostring_unsetf)
    ost.unsetf (tostring_unsetf);
  else if (unset)
    ost.unsetf (unset);
  
  if (tostring_precision)
    ost.precision (tostring_precision);
  else
    ost.precision (precision);

  ost << input;

  if (ost.fail())
  {
    Error error (InvalidState, "tostring");
    error << "failed to convert " << input << " to string:";
    throw error;
  }

  return ost.str();
}

template<class T>
T fromstring (const std::string& input)
{
  std::istringstream ist;

  ist.clear();
  ist.str(input);

  T retval;
  ist >> retval;

  if (ist.fail())
    throw Error (InvalidState, "fromstring", "failed to parse '"+ input +"'");

  return retval;
}

// string class specializations
template<>
inline std::string tostring (const std::string& input, unsigned,
			     std::ios_base::fmtflags, std::ios_base::fmtflags)
{
  return input;
}

template<>
inline std::string fromstring<std::string> (const std::string& input)
{
  return input;
}

// char* specialization
inline std::string tostring (const char* input)
{
  return input;
}

/*
  If you've already written a function that converts string to Type,
  and this function throws an exception of type Error when the string
  cannot be parsed, then you can use this template to implement an
  extraction operator (operator >>).  See Util/genutil/Types.C for an
  example. 
*/
template<typename Type, typename String2Type>
std::istream& extraction (std::istream& is, Type& t, String2Type string2type)
{
  std::streampos pos = is.tellg();
  std::string ss;

  try {
    is >> ss;
    t = string2type (ss);
  }
  catch (Error& e) {
    is.setstate(std::istream::failbit);
    is.seekg(pos);
  }
  return is;
}

#endif
