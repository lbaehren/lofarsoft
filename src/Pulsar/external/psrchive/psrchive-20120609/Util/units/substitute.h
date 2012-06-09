//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/substitute.h,v $
   $Revision: 1.7 $
   $Date: 2009/04/07 07:37:11 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_SUBSTITUTE_H
#define __UTILS_UNITS_SUBSTITUTE_H

#include "TextInterfaceName.h"
#include "Functor.h"

#include <string>
#include <algorithm>
#include <ctype.h>

//! Return first character in text such that pred(c) is true
template<class Pred> std::string::size_type 
find_first_if (const std::string& text, Pred pred, std::string::size_type pos)
{
  std::string::const_iterator iter;
  iter = std::find_if (text.begin()+pos, text.end(), pred);
  if (iter == text.end())
    return std::string::npos;
  else
    return iter - text.begin();
}

template<class T>
std::string substitute (const std::string& text, const T* resolver,
			char substitution = '$',
			Functor< bool(char) > in_name = 
			Functor< bool(char) > (new TextInterface::Name,
					       &TextInterface::Name::valid) )
{
  std::string remain = text;
  std::string result;

  std::string::size_type start;

  while ( (start = remain.find(substitution)) != std::string::npos ) {

    // string preceding the variable substitution
    std::string before = remain.substr (0, start);

    // ignore the substitution symbol
    start ++;

    // find the end of the variable name
    std::string::size_type end;
    end = find_first_if (remain, std::not1(in_name), start);

    // length to end of variable name
    std::string::size_type length = std::string::npos;

    if (end != std::string::npos)
      length = end - start;

    // the variable name
    std::string name = remain.substr (start, length);

    // perform the substitution and add to the result
    result += before + resolver->get_value(name);

    // remainder of string following the variable name
    if (end != std::string::npos)
      remain = remain.substr (end);
    else
      remain.erase();

  }

  return result + remain;
}

#endif
