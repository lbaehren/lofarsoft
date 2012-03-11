//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/escape.h,v $
   $Revision: 1.1 $
   $Date: 2008/08/28 04:23:22 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_ESCAPE_H
#define __UTILS_UNITS_ESCAPE_H

#include <string>

//! Convert escape sequences to ASCII equivalents; e.g. "\n" -> '\n'
std::string escape (std::string);

#endif

